package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.google.gson.Gson;
import com.intel.wearable.platform.timeiq.api.common.calendar.CalendarDetails;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.events.BeEvent;
import com.intel.wearable.platform.timeiq.api.events.IEvent;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQCalendarUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQEventsUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQPlacesUtils;
import com.intel.wearable.platform.timeiq.refapp.googleAnalytics.GoogleAnalyticsTrackers;
import com.intel.wearable.platform.timeiq.refapp.places.add_edit.ActivityPlacePicker;
import com.intel.wearable.platform.timeiq.refapp.utils.TextUtil;
import com.intel.wearable.platform.timeiq.refapp.utils.TimeAndDatePicker;

import java.util.ArrayList;
import java.util.List;

/**
 * A placeholder fragment containing a simple view.
 */
public class FragmentEditBe extends Fragment implements IReminderFragment {

    public static final String SELECT_PLACE_ID_DATA_KEY = "SELECT_PLACE_ID_DATA_KEY";

    private TextView mWhenBeView;
    private TextView mWhereBeView;
    private long mBeEventTime;
    private static final int PLACE_PICKER = 2;
    private TSOPlace mSelectedPlace;
    private IFragmentFormDataChangedListener mDataChangeListener;
    private Switch mAddToCalendarSwitch;
    private boolean mIsWriteCalendarAvailable;
    private final List<String> mEventIdsToIgnoreForConflicts = new ArrayList<>();
    private LinearLayout mConflictsLayout;
    private TextView mNothingToDisplayTextView;
    private ListView mConflictListView;

    /**
     * Returns a new instance of this fragment for the given section
     * number.
     */
    public static FragmentEditBe newInstance(IFragmentFormDataChangedListener dataChangedListener) {
        FragmentEditBe fragment = new FragmentEditBe();
        fragment.setDataChangeListener(dataChangedListener);
        return fragment;
    }

    public FragmentEditBe() {
    }

    public TSOPlace getSelectedPlace() {
        return mSelectedPlace;
    }

    public long getEventTime() {
        return mBeEventTime;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_edit_be, container, false);


        mWhenBeView = (TextView)rootView.findViewById(R.id.be_when);
        mWhenBeView.setClickable(true);
        mBeEventTime = -1;

        final FragmentActivity activity = getActivity();

        mWhenBeView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TimeAndDatePicker timeAndDatePicker = new TimeAndDatePicker();
                timeAndDatePicker.handleTimePicker(getActivity(), new TimeAndDatePicker.TimePickerListener() {
                    @Override
                    public void onTimePicked(long time) {
                        Log.d("Time picker", "picked time = " + time + " ");
                        setWhen(time);
                    }

                    @Override
                    public void onCancel() { }
                });
            }
        });
        mWhereBeView = (TextView)rootView.findViewById(R.id.be_where);

        mWhereBeView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intentActivity = new Intent(activity, ActivityPlacePicker.class);
                startActivityForResult(intentActivity, PLACE_PICKER);
            }
        });

        initAddToCalendarSwitch(rootView);

        mConflictsLayout = (LinearLayout) rootView.findViewById(R.id.be_conflict_layout);
        mNothingToDisplayTextView = (TextView) rootView.findViewById(R.id.nothing_to_display_be);
        mConflictListView = (ListView) rootView.findViewById(R.id.be_conflicts_list);

        return rootView;
    }

    public void setWhen(long whenTime) {
        final FragmentActivity activity = getActivity();
        final Resources resources = activity.getResources();

        mBeEventTime = whenTime;
        String mBeEventTimeStr = TextUtil.getReminderDateString(activity, mBeEventTime);
        mWhenBeView.setText(resources.getString(R.string.be_at_time, mBeEventTimeStr));
        mDataChangeListener.setCreateReminderIcon(isOkToCreateReminder());
    }

    private void initAddToCalendarSwitch(View rootView) {
        mAddToCalendarSwitch = (Switch) rootView.findViewById(R.id.be_add_to_calendar_switch);
        ResultObject<CalendarDetails> writeCalendarResultData = TimeIQCalendarUtils.getWriteCalendar();
        mIsWriteCalendarAvailable = writeCalendarResultData.isSuccess() && writeCalendarResultData.getData() != null;
        mAddToCalendarSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (!mIsWriteCalendarAvailable) {
                    mAddToCalendarSwitch.toggle();
                    Toast.makeText(getActivity(), R.string.be_event_no_writable_calendar, Toast.LENGTH_LONG).show();
                }
            }
        });
    }


    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        boolean callCreateReminderIcon = true;
        if (requestCode == PLACE_PICKER) {
            if (resultCode == Activity.RESULT_OK){
                if(data.getExtras().containsKey(SELECT_PLACE_ID_DATA_KEY)){
                    String stringExtra = data.getStringExtra(SELECT_PLACE_ID_DATA_KEY);
                    PlaceID placeId = new Gson().fromJson(stringExtra, PlaceID.class);
                    TSOPlace place = TimeIQPlacesUtils.getPlace(placeId);
                    setWhere(place);
                    callCreateReminderIcon = false;
                }
            }
        }

        if (callCreateReminderIcon) {
            mDataChangeListener.setCreateReminderIcon(isOkToCreateReminder());
        }
    }

    public void setWhere(TSOPlace where) {
        if(where != null){
            mSelectedPlace = where;
            String name = mSelectedPlace.getName();
            mWhereBeView.setText(getString(R.string.where_be_at_location, name));
        }

        mDataChangeListener.setCreateReminderIcon(isOkToCreateReminder());
    }

    public BeEvent createBeEvent() {
        boolean addToCalendar = mAddToCalendarSwitch.isChecked();
        BeEvent beEvent = TimeIQEventsUtils.createBeEvent(getActivity(), mSelectedPlace, mBeEventTime, addToCalendar);

        GoogleAnalyticsTrackers.getInstance().trackEvent(R.string.google_analytics_event, R.string.google_analytics_add, beEvent.getEventType().name());

        return beEvent;
    }


    @Override
    public String isOkToCreateReminder() {
        String validationData = "";
        if (mSelectedPlace == null) {
            validationData = getString(R.string.reminder_error_missing_target_location);
        }

        if ( TextUtils.isEmpty(validationData)) {
            if (mBeEventTime <= 0) {
                validationData = getString(R.string.reminder_error_missing_event_time);
            }
        }

        if (TextUtils.isEmpty(validationData)) {
            showConflicts();
        }

        return validationData;
    }

    public void addEventIdToIgnoreForConflicts(String eventID) {
        mEventIdsToIgnoreForConflicts.add(eventID);
    }

    private void showConflicts() {
    // Check if there are any events in the time of the new Be event
        List<IEvent> conflictingEvents = TimeIQEventsUtils.getEventsBetweenTimes(mBeEventTime, mBeEventTime);
        List<IEvent> nonIgnoredConflictingEvents = getNonIgnoredConflictingEvents(conflictingEvents);
        if (!nonIgnoredConflictingEvents.isEmpty()) {
            mNothingToDisplayTextView.setVisibility(View.GONE);
            mConflictsLayout.setVisibility(View.VISIBLE);

            ConflictingEventsAdapter conflictingEventsAdapter = new ConflictingEventsAdapter(getActivity(), new ArrayList<>(nonIgnoredConflictingEvents));
            mConflictListView.setAdapter(conflictingEventsAdapter);
        } else {
            mConflictsLayout.setVisibility(View.GONE);
            if (!TimeIQCalendarUtils.isThereAnyReadableCalendarsSet()) { // no calendars selected
                mNothingToDisplayTextView.setVisibility(View.VISIBLE);
            }
        }
    }

    private List<IEvent> getNonIgnoredConflictingEvents(List<IEvent> conflictingEvents) {
        List<IEvent> nonIgnoredConflictingEvents;
        if (mEventIdsToIgnoreForConflicts == null || mEventIdsToIgnoreForConflicts.isEmpty()) {
            nonIgnoredConflictingEvents = conflictingEvents;
        } else {
            nonIgnoredConflictingEvents = new ArrayList<>();
            for (IEvent conflictingEvent : conflictingEvents) {
                if (!mEventIdsToIgnoreForConflicts.contains(conflictingEvent.getId())) {
                    nonIgnoredConflictingEvents.add(conflictingEvent);
                }
            }
        }

        return nonIgnoredConflictingEvents;
    }

    @Override
    public void setDataChangeListener(IFragmentFormDataChangedListener dataChangeListener) {
        mDataChangeListener = dataChangeListener;
    }

}
