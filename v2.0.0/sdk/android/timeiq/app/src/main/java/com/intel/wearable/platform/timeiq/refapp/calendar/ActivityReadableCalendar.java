package com.intel.wearable.platform.timeiq.refapp.calendar;

import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.common.calendar.CalendarDetails;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQCalendarUtils;

import java.util.List;

public class ActivityReadableCalendar extends ActivityBaseCalendars {

    public ActivityReadableCalendar() {
        super(true, R.string.readable_calendars_title);
    }

    @Override
    protected boolean setCalendars(List<String> selectedCalendars) {
        boolean success = false;
        if (selectedCalendars.size() > 0) {
            TimeIQCalendarUtils.setReadCalendars(selectedCalendars);
            success = true;
        } else {
            Toast.makeText(this, R.string.error_readable_calendars_none_selected_text, Toast.LENGTH_LONG).show();
        }

        return success;
    }

    @Override
    protected ResultObject<List<CalendarDetails>> getSelectedCalendars() {
        return TimeIQCalendarUtils.getReadCalendars();
    }

}