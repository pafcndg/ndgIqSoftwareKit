package com.intel.wearable.platform.timeiq.refapp.events;

import android.content.res.ColorStateList;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.api.events.IEvent;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.TimeIQBGService;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQCalendarUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQEventsUtils;
import com.intel.wearable.platform.timeiq.refapp.googleAnalytics.GoogleAnalyticsTrackers;
import com.intel.wearable.platform.timeiq.refapp.main.IMainFragment;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by adura on 22/10/2015.
 */


/**
 * Fragment holding the Event list
 */
public class UpcomingEventsFragment extends Fragment implements IMainFragment {

    private static final String TAG = UpcomingEventsFragment.class.getSimpleName();
    private ArrayList<IEvent> mEventsArray;
    private UpcomingEventsAdapter mUpcomingEventsAdapter;
    private TextView mEmptyListMsgView;


    public UpcomingEventsFragment() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_upcomming_events, container, false);
        mEmptyListMsgView = (TextView) rootView.findViewById(R.id.emptyListText);

        mEventsArray = new ArrayList<>();

        mUpcomingEventsAdapter = new UpcomingEventsAdapter(getActivity(), R.id.remindersListView, mEventsArray);

        ListView listView = (ListView) rootView.findViewById(R.id.eventListView);
        listView.setAdapter(mUpcomingEventsAdapter);

        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
        if (isAdded()) {
            refreshUiEventList();
        }
    }

    /**
     * Refresh the event list. display empty list message if none
     */
    private void refreshUiEventList() {
        // Update the mEventsArray
        updateEvents();

        // notify the adapter on data change
        mUpcomingEventsAdapter.notifyDataSetChanged();

        if (mUpcomingEventsAdapter.isEmpty()) {
            mEmptyListMsgView.setVisibility(View.VISIBLE);
            if (TimeIQCalendarUtils.isThereAnyReadableCalendarsSet()) {
                mEmptyListMsgView.setText(getString(R.string.nothing_to_display_events));
            } else {
                mEmptyListMsgView.setText(getString(R.string.nothing_to_display_events_no_readable_calendars));
            }
        } else {
            mEmptyListMsgView.setVisibility(View.GONE);
        }
    }


    /**
     * Update the event list from the EventsEngine
     */
    private void updateEvents() {
        mEventsArray.clear();
        Log.d(TAG, "updateEvents - SDK is initialized = " + TimeIQBGService.mTimeIQApi.isInitialized());
        List<IEvent> upcomingEvents = TimeIQEventsUtils.getUpcomingEvents();
        if(upcomingEvents != null && !upcomingEvents.isEmpty()){
            mEventsArray.addAll(upcomingEvents);
        }
    }

    @Override
    public void onFragmentShown() {
        Log.d(TAG, "onFragmentShown");
        // Refresh event list on the UI
        refreshUiEventList();

        // Set the Floating Action Button to refresh the list
        FragmentActivity activity = getActivity();
        if (activity != null) {
            FloatingActionButton fab = (FloatingActionButton) activity.findViewById(R.id.fab);
            if (fab != null) {
                fab.setImageResource(R.drawable.e_ref);
                int color = getResources().getColor(R.color.upcoming_events_fab_bg_color);
                fab.setBackgroundTintList(ColorStateList.valueOf(color));
            }
        }
    }

    @Override
    public void onFloatingButtonPressed() {
        updateEvents();

        // Refresh the UI using the adapter
        mUpcomingEventsAdapter.notifyDataSetChanged();
        GoogleAnalyticsTrackers.getInstance().trackEvent(R.string.google_analytics_event, R.string.google_analytics_refresh, "");
    }
}
