package com.intel.wearable.platform.timeiq.refapp.utils;

import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.widget.DatePicker;
import android.widget.TimePicker;

import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.Calendar;

/**
 * Created by smoradof on 10/21/2015.
 */
public class TimeAndDatePicker {

    private int mMinute      ;
    private int mHourOfDay   ;
    private int mDayOfMonth  ;
    private int mMonthOfYear ;
    private int mYear        ;


    public interface TimePickerListener{
        void onTimePicked(long time);
        void onCancel();
    }

    public void handleTimePicker(final Context ctx, final TimePickerListener timePickerListener)
    {
        final Calendar currentTime = Calendar.getInstance();
        mMinute      = currentTime.get(Calendar.MINUTE);
        mHourOfDay   = currentTime.get(Calendar.HOUR_OF_DAY);
        mDayOfMonth  = currentTime.get(Calendar.DAY_OF_MONTH);
        mMonthOfYear = currentTime.get(Calendar.MONTH);
        mYear        = currentTime.get(Calendar.YEAR);

        final Resources resources = ctx.getResources();

        DatePickerDialog datePicker = new DatePickerDialog(ctx, new DatePickerDialog.OnDateSetListener()
        {
            @Override
            public void onDateSet(DatePicker view, int year, int monthOfYear, int dayOfMonth) {
                mYear = year;
                mMonthOfYear = monthOfYear;
                mDayOfMonth = dayOfMonth;

                TimePickerDialog timePicker;
                timePicker = new TimePickerDialog(ctx, new TimePickerDialog.OnTimeSetListener()
                {
                    @Override
                    public void onTimeSet(TimePicker timePicker, int selectedHour, int selectedMinute) {
                        mHourOfDay = selectedHour;
                        mMinute = selectedMinute;
                        Calendar selectedTime = Calendar.getInstance();
                        selectedTime.set(mYear, mMonthOfYear, mDayOfMonth, mHourOfDay, mMinute, 0);
                        timePickerListener.onTimePicked(selectedTime.getTimeInMillis());
                    }
                }, mHourOfDay, mMinute, true);//Yes 24 hour time
                timePicker.setOnCancelListener(new DialogInterface.OnCancelListener() {
                    @Override
                    public void onCancel(DialogInterface dialog) {
                        timePickerListener.onCancel();
                    }
                });
                timePicker.setTitle(resources.getString(R.string.reminder_edit_select_time));
                timePicker.show();
            }
        }, mYear, mMonthOfYear, mDayOfMonth);

        datePicker.setOnCancelListener(new DialogInterface.OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                timePickerListener.onCancel();
            }
        });
        datePicker.setTitle(resources.getString(R.string.reminder_edit_select_date));
        datePicker.getDatePicker().setMinDate(System.currentTimeMillis() - 1000);
        datePicker.show();

    }
}
