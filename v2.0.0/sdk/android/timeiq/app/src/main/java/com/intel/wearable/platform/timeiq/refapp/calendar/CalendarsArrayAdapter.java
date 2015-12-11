package com.intel.wearable.platform.timeiq.refapp.calendar;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.CheckedTextView;

import com.intel.wearable.platform.timeiq.api.common.calendar.CalendarDetails;
import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.ArrayList;
import java.util.Arrays;

/*package*/ class CalendarsArrayAdapter extends ArrayAdapter<CalendarDetails> {

    private final Context mContext;
    private final ArrayList<CalendarDetails> mCalendarDetailsArray;
    private final boolean[] mSelected;
    private final int mListItemResource;

    public CalendarsArrayAdapter(Context context, int listResource, int listItemResource, ArrayList<CalendarDetails> placesArray, boolean[] selected) {
        super(context, listResource, placesArray);
        mContext = context;
        mCalendarDetailsArray = placesArray;
        mSelected = selected;
        mListItemResource = listItemResource;
    }

    @Override
    public View getView(int position, View view, ViewGroup parent) {
        if (view == null) {
            LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            view = inflater.inflate(mListItemResource, null);
        }

        if (view != null) {
            CheckedTextView checkedTextView = (CheckedTextView) view;
            CalendarDetails calendarDetails = mCalendarDetailsArray.get(position);
            Boolean selected = mSelected[position];
            String calendarName = calendarDetails.getCalendarName();
            String accountName = calendarDetails.getAccountName();
            String text;
            if (calendarName != null) {
                text = mContext.getString(R.string.readable_calendars_text_with_calendar_name, calendarName, accountName);
            } else {
                text = mContext.getString(R.string.readable_calendars_text_without_calendar_name, accountName);
            }

            checkedTextView.setText(text);
            checkedTextView.setChecked(selected);
        }

        return view;
    }

    public void toggleItemAt(int position) {
        mSelected[position] = !mSelected[position];
    }

    public void selectSingle(int position) {
        Arrays.fill(mSelected, false);
        mSelected[position] = true;
    }

}
