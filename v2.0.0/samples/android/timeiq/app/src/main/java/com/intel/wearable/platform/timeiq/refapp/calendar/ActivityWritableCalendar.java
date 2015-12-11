package com.intel.wearable.platform.timeiq.refapp.calendar;

import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.common.calendar.CalendarDetails;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQCalendarUtils;

import java.util.ArrayList;
import java.util.List;

public class ActivityWritableCalendar extends ActivityBaseCalendars {

    public ActivityWritableCalendar() {
        super(false, R.string.writable_calendars_title);
    }

    @Override
    protected boolean setCalendars(List<String> selectedCalendars) {
        boolean success = false;
        if (selectedCalendars.size() == 1) {
            TimeIQCalendarUtils.setWriteCalendar(selectedCalendars.get(0));
            success = true;
        } else {
            Toast.makeText(this, R.string.error_writable_calendars_exactly_one_text, Toast.LENGTH_LONG).show();
        }

        return success;
    }

    @Override
    protected ResultObject<List<CalendarDetails>> getSelectedCalendars() {
    // transfer from single CalendarDetails to List<CalendarDetails>
        ResultObject<List<CalendarDetails>> result;
        ResultObject<CalendarDetails> writeCalendarResultData = TimeIQCalendarUtils.getWriteCalendar();
        CalendarDetails writeCalendar = writeCalendarResultData.getData();
        if (writeCalendar == null) {
            result = new ResultObject<>(false, writeCalendarResultData.getMsg(), null);
        } else {
            ArrayList<CalendarDetails> writeCalendars = new ArrayList<>(1);
            writeCalendars.add(writeCalendar);
            result = new ResultObject<List<CalendarDetails>>(writeCalendarResultData.isSuccess(), writeCalendarResultData.getMsg(), writeCalendars);
        }

        return result;
    }

}