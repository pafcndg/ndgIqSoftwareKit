package com.intel.wearable.platform.timeiq.refapp.reminders;

import android.content.Context;
import android.text.format.DateUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.api.events.BeEvent;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.triggers.ITrigger;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.utils.TextUtil;

import java.util.ArrayList;

/**
 * Created by smoradof on 10/15/2015.
 */
/*package*/ class RemindersListAdapter extends ArrayAdapter<Object> {

    private final ArrayList<Object> mRemindersAndEventsArray;

    public RemindersListAdapter(Context context, int resource, ArrayList<Object> remindersAndEventsArray) {
        super(context, resource, remindersAndEventsArray);
        mRemindersAndEventsArray = remindersAndEventsArray;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Context ctx = getContext();
        if (convertView == null) {
            LayoutInflater inflater = (LayoutInflater) ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.item_reminder, null);
        }

        if (convertView != null) {
            long addedTimeLong;
            Object object = mRemindersAndEventsArray.get(position);
            if (object instanceof IReminder) {
                IReminder reminder = (IReminder) object;

                String reminderText = TextUtil.getReminderText(ctx, reminder);

                ITrigger trigger = reminder.getTrigger();
                String triggerText = TextUtil.getTriggerText(ctx, trigger);

                addedTimeLong = reminder.getAddedTime();

                ((TextView) convertView.findViewById(R.id.main_text)).setText(reminderText);

                ((TextView) convertView.findViewById(R.id.secondary_text)).setText(triggerText);

                int iconSrc = -1;
                switch (reminder.getReminderType()) {

                    case CALL:
                        iconSrc = R.drawable.list_call;
                        break;
                    case NOTIFY:
                        iconSrc = R.drawable.list_notif;
                        break;
                    case DO:
                        iconSrc = R.drawable.list_do;
                        break;
                }

                if (iconSrc >= 0) {
                    ((ImageView) convertView.findViewById(R.id.reminder_icon)).setImageResource(iconSrc);
                }
            } else /*we know we only add IReminder or BeEvent*/ {
                BeEvent event = (BeEvent) object;
                String description = event.getSubject();
                String arrivalTime = getBeTime(ctx, event.getArrivalTime());
                addedTimeLong = event.getCreationTime();

                // TODO add required text
                String timeFromTo = ""; // from - to";

                convertView.findViewById(R.id.events_layout).setVisibility(View.VISIBLE);

                ((TextView)convertView.findViewById(R.id.main_text)).setText(description);

                ((TextView)convertView.findViewById(R.id.secondary_text)).setText(arrivalTime);

                ((TextView)convertView.findViewById(R.id.from_to)).setText(timeFromTo);

                ((ImageView) convertView.findViewById(R.id.reminder_icon)).setImageResource(R.drawable.list_be);

                //((ImageView) convertView.findViewById(R.id.place_icon)).setImageResource(iconSrc);
            }

            String addedTimeStr;
            if (addedTimeLong > 0) {
                addedTimeStr = getAddedTime(addedTimeLong);
            } else {
                addedTimeStr = ctx.getString(R.string.na);
            }

            ((TextView) convertView.findViewById(R.id.added_time)).setText(addedTimeStr);
        }

        return convertView;
    }

    private String getAddedTime(long time) {
        return DateUtils.getRelativeTimeSpanString(time, System.currentTimeMillis(), DateUtils.MINUTE_IN_MILLIS, DateUtils.FORMAT_ABBREV_ALL).toString();
    }

    private String getBeTime(Context context, long time) {
        return DateUtils.getRelativeDateTimeString(context, time, DateUtils.MINUTE_IN_MILLIS, DateUtils.WEEK_IN_MILLIS, DateUtils.FORMAT_ABBREV_ALL).toString();
    }
}
