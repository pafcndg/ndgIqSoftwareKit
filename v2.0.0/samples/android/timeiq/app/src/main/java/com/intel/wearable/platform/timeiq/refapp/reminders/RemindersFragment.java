package com.intel.wearable.platform.timeiq.refapp.reminders;

import android.content.Intent;
import android.content.res.ColorStateList;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.api.common.messageHandler.IMessage;
import com.intel.wearable.platform.timeiq.api.common.messageHandler.IMessageListener;
import com.intel.wearable.platform.timeiq.api.common.messageHandler.MessageType;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.events.BeEvent;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.TimeIQBGService;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQEventsUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQRemindersUtils;
import com.intel.wearable.platform.timeiq.refapp.main.IMainFragment;
import com.intel.wearable.platform.timeiq.refapp.reminders.add_edit.ActivityAddReminder;
import com.intel.wearable.platform.timeiq.refapp.reminders.add_edit.ActivityEditBeEvent;
import com.intel.wearable.platform.timeiq.refapp.reminders.add_edit.ActivityEditReminder;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * Created by adura on 22/10/2015.
 */


/**
 * Fragment holding the Reminders list
 */
public class RemindersFragment extends Fragment implements IMainFragment, IMessageListener {

    private static final String TAG = "RemindersFragment";
    private ArrayList<Object> mRemindersAndEventsArray;
    private RemindersListAdapter mRemindersListAdapter;
    private TextView mEmptyListMsg;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_reminders, container, false);
        mEmptyListMsg = (TextView) rootView.findViewById(R.id.emptyListText);

        mRemindersAndEventsArray = new ArrayList<>();

        mRemindersListAdapter = new RemindersListAdapter(getActivity(), R.id.remindersListView, mRemindersAndEventsArray);
        ListView listView = (ListView) rootView.findViewById(R.id.remindersListView);
        listView.setAdapter(mRemindersListAdapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long l) {
                Object object = parent.getItemAtPosition(position);
                if (object instanceof IReminder) {
                    IReminder reminder = (IReminder) object;
                    String reminderId = reminder.getId();
                    FragmentActivity activity = getActivity();
                    Intent intentActivity = new Intent(activity, ActivityEditReminder.class);

                    intentActivity.putExtra(ActivityEditReminder.REMINDER_KEY_ID, reminderId);

                    getActivity().startActivity(intentActivity);
                } else /*we know we only add IReminder or BeEvent*/ {
                    BeEvent beEvent = (BeEvent) object;
                    String beEventId = beEvent.getId();
                    FragmentActivity activity = getActivity();
                    Intent intentActivity = new Intent(activity, ActivityEditBeEvent.class);

                    intentActivity.putExtra(ActivityEditBeEvent.BE_EVENT_KEY_ID, beEventId);

                    getActivity().startActivity(intentActivity);
                }
            }
        });

        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume - SDK is initialized = " + TimeIQBGService.mTimeIQApi.isInitialized());
        TimeIQBGService.mTimeIQApi.getMessageHandler().register(this);
        if (isAdded()) {
            refreshRemindersAndEvents();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.d(TAG, "onPause - SDK is initialized = " + TimeIQBGService.mTimeIQApi.isInitialized());
        TimeIQBGService.mTimeIQApi.getMessageHandler().unRegister(this);
    }

    private void refreshRemindersAndEvents() {
        updateRemindersAndEvents();
        mRemindersListAdapter.notifyDataSetChanged();

        if (mRemindersListAdapter.isEmpty()) {
            mEmptyListMsg.setVisibility(View.VISIBLE);
        } else {
            mEmptyListMsg.setVisibility(View.GONE);
        }
    }

    private void updateRemindersAndEvents() {
        // update the reminders list from the TimeIQ's RemindersManager:
        ResultData<Collection<IReminder>> allActiveRemindersResultData = TimeIQRemindersUtils.getAllActiveReminders();
        if (allActiveRemindersResultData.isSuccess()) {
            Collection<IReminder> remindersCollection = allActiveRemindersResultData.getData();
            mRemindersAndEventsArray.clear();
            mRemindersAndEventsArray.addAll(remindersCollection);

            // update the events list from the TimeIQ's RemindersManager:
            List<BeEvent> beEventsForTheNextYear = TimeIQEventsUtils.getBeEventsForTheNextYear();
            if (beEventsForTheNextYear != null && !beEventsForTheNextYear.isEmpty()) {
                mRemindersAndEventsArray.addAll(beEventsForTheNextYear);
            }
        }
    }

    @Override
    public void onFragmentShown() {
        Log.d(TAG, "onFragmentShown");
        refreshRemindersAndEvents();
        FragmentActivity activity = getActivity();
        if(activity != null) {
            FloatingActionButton fab = (FloatingActionButton) activity.findViewById(R.id.fab);
            if (fab != null) {
                fab.setImageResource(R.drawable.e_add);
                int color = getResources().getColor(R.color.reminders_fab_bg_color);
                fab.setBackgroundTintList(ColorStateList.valueOf(color));
            }
        }
    }

    @Override
    public void onFloatingButtonPressed() {
        Log.d(TAG, "RemindersFragment: onFloatingButtonPressed");

        FragmentActivity activity = getActivity();
        Intent intentActivityReminder = new Intent(activity ,ActivityAddReminder.class);

        getActivity().startActivity(intentActivityReminder);
    }

    @Override
    public void onReceive(IMessage message) {
        FragmentActivity activity = getActivity();
        if (activity != null) {
            if(message.getType() == MessageType.ON_REMINDERS_TRIGGERED || message.getType() == MessageType.ON_EVENT_END) {
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        refreshRemindersAndEvents();
                    }
                });
            }
        }
    }
}
