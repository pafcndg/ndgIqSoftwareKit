package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.events.BeEvent;
import com.intel.wearable.platform.timeiq.api.events.IEvent;
import com.intel.wearable.platform.timeiq.api.events.RecurrenceDetails;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQEventsUtils;
import com.intel.wearable.platform.timeiq.refapp.googleAnalytics.GoogleAnalyticsTrackers;

public class ActivityEditBeEvent extends BaseActivityEditAndAddBeEvent implements View.OnClickListener {

    public static final String BE_EVENT_KEY_ID = "be_event_key_id";

    private String mEventId;
    private BeEvent mBeEvent;
    private Switch mDeleteAllRecurrencesSwitch;
    private FragmentEditBe mFragmentEditBe;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        createContentView(R.layout.activity_edit_be, R.string.be_edit_title);

        mFragmentEditBe = (FragmentEditBe) getSupportFragmentManager().findFragmentById(R.id.fragment_edit_be);
        mFragmentEditBe.setDataChangeListener(this);
        getAndValidateInput();
        mFragmentEditBe.addEventIdToIgnoreForConflicts(mEventId);
        setDeleteButton();
        setDeleteAllRecurrences();
        setGivenBeEventToView();

        setCreateReminderIcon(getString(R.string.be_default_error));
    }

    private void getAndValidateInput() {
        try {
            Bundle bundle = getIntent().getExtras();
            mEventId = bundle.getString(BE_EVENT_KEY_ID);
            if (mEventId != null) {
                IEvent event = TimeIQEventsUtils.getEventById(mEventId);
                if (event != null) {
                    if (event instanceof BeEvent) {
                        mBeEvent = (BeEvent) event;
                    } else {
                        showMessageAndClose(R.string.be_not_be_event);
                    }
                } else {
                    showMessageAndClose(R.string.be_wrong_id);
                }
            } else {
                showMessageAndClose(R.string.be_failed_to_get_event_id);
            }
        } catch (Exception e) {
            showMessageAndClose(R.string.be_failed_to_get_input);
        }
    }

    private void setDeleteButton() {
        Button deleteButton = (Button) findViewById(R.id.delete_button);
        deleteButton.setOnClickListener(this);
    }

    private void setDeleteAllRecurrences() {
        RecurrenceDetails recurrenceDetails = mBeEvent.getRecurrenceDetails();
        LinearLayout deleteAllRecurrencesLayout = (LinearLayout) findViewById(R.id.be_delete_all_recurrences_layout);
        mDeleteAllRecurrencesSwitch = (Switch) findViewById(R.id.be_delete_all_recurrences_switch);
        if (recurrenceDetails == null) {
            deleteAllRecurrencesLayout.setVisibility(View.INVISIBLE);
        } else {
            deleteAllRecurrencesLayout.setVisibility(View.VISIBLE);
        }
    }

    private void setGivenBeEventToView() {
        setAddCalendarStateToView();
        setWhereYouNeedToBeToView();
        setWhenYouNeedToBeToView();
    }

    private void setAddCalendarStateToView() {
        boolean isAddedToCalendar = mBeEvent.isAddToCalendar();
        Switch addToCalendarSwitch = (Switch) findViewById(R.id.be_add_to_calendar_switch);
        addToCalendarSwitch.setChecked(isAddedToCalendar);
        addToCalendarSwitch.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                Toast.makeText(mContext, R.string.be_add_to_calendar_state, Toast.LENGTH_SHORT).show();
                return true;
            }
        });
    }

    private void setWhereYouNeedToBeToView() {
        TSOPlace location = mBeEvent.getLocation();
        mFragmentEditBe.setWhere(location);
    }

    private void setWhenYouNeedToBeToView() {
        long arrivalTime = mBeEvent.getArrivalTime();
        mFragmentEditBe.setWhen(arrivalTime);
    }

    @Override
    protected void actionOnOk() {
        editBe();
    }

    private void editBe() {
        BeEvent newBeEvent = TimeIQEventsUtils.createBeEvent(mContext, mBeEvent, mFragmentEditBe.getSelectedPlace(), mFragmentEditBe.getEventTime());
        String errorMessage = TimeIQEventsUtils.updateEvent(mContext, newBeEvent);
        if (errorMessage != null) {
            showMessageAndClose(errorMessage);
        } else {
            GoogleAnalyticsTrackers.getInstance().trackEvent(R.string.google_analytics_event, R.string.google_analytics_edit, newBeEvent.getEventType().name());
            showMessageAndClose(R.string.be_edit_successful);
        }
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();
        if (id == R.id.delete_button) {
            delete();
        }
    }

    private void delete() {
        String errorMessage;
        if (mDeleteAllRecurrencesSwitch.isChecked()) {
            errorMessage = TimeIQEventsUtils.deleteEvent(mContext, mEventId, true);
        } else {
            errorMessage = TimeIQEventsUtils.deleteEvent(mContext, mEventId);
        }

        if (errorMessage != null) {
            showMessageAndClose(errorMessage);
        } else {
            showMessageAndClose(R.string.be_delete_successful);
        }
    }

    private void showMessageAndClose(int stringResource) {
        Toast.makeText(mContext, stringResource, Toast.LENGTH_LONG).show();
        finish();
    }

    private void showMessageAndClose(String string) {
        Toast.makeText(mContext, string, Toast.LENGTH_LONG).show();
        finish();
    }

}