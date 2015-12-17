package com.intel.wearable.platform.bodyiq.refapp.fragments;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.bodymanager.BodyIQInfoSummary;
import com.intel.wearable.platform.bodyiq.refapp.bodymanager.BodyIQManager;
import com.intel.wearable.platform.bodyiq.refapp.customui.CustomRoundChart;

import java.util.Calendar;

/**
 * Created by fissaX on 12/2/15.
 */
public class ActivitiesTabFragment extends Fragment {


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.activitiestabfragment, container, false);

        ((TextView) view.findViewById(R.id.active_date)).setText(getDateDisplay());


        long end = System.currentTimeMillis();

        Calendar c = Calendar.getInstance();
        c.set(Calendar.HOUR_OF_DAY, 0);
        c.set(Calendar.MINUTE, 0);
        c.set(Calendar.SECOND, 0);
        c.set(Calendar.MILLISECOND, 0);
        long start = c.getTimeInMillis();  // get midnight time in milliseconds


        BodyIQManager bodyManager = new BodyIQManager(start,end);

        BodyIQInfoSummary info = bodyManager.getBodyInfo();


        ((TextView) view.findViewById(R.id.totaldistance)).setText(info.distance + " m");
        ((TextView)view.findViewById(R.id.totalcalories)).setText(String.valueOf(info.calories));
        ((TextView)view.findViewById(R.id.totalduration)).setText(info.duration);
        ((TextView)view.findViewById(R.id.totalsteps)).setText(String.valueOf(info.steps));
        ((TextView)view.findViewById(R.id.avgspeed)).setText(info.avgSpeed +" kmh");
        ((TextView)view.findViewById(R.id.maxactivity)).setText(info.longestActivity);

        ((CustomRoundChart)view.findViewById(R.id.stepschart)).init(info.steps, 4000);
        ((CustomRoundChart)view.findViewById(R.id.calorieschart)).init(info.calories,1600);


        return view;
    }


    private String getDateDisplay()
    {
        String[] days = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
        String[] months = {"January","February","March","April","May","June","July","August","September","October","November","December"};

        Calendar c = Calendar.getInstance();

        int dayinweek = c.get(Calendar.DAY_OF_WEEK);
        String dayName = "";
        switch (dayinweek)
        {
            case Calendar.SUNDAY:
                dayName = days[0];
                break;
            case Calendar.MONDAY:
                dayName = days[1];
                break;
            case Calendar.TUESDAY:
                dayName = days[2];
                break;
            case Calendar.WEDNESDAY:
                dayName = days[3];
                break;
            case Calendar.THURSDAY:
                dayName = days[4];
                break;
            case Calendar.FRIDAY:
                dayName = days[5];
                break;
            case Calendar.SATURDAY:
                dayName = days[6];
                break;
        }

        int month = c.get(Calendar.MONTH);
        int dayInMonth = c.get(Calendar.DAY_OF_MONTH);

        return dayName + ", " + months[month] + " " + String.valueOf(dayInMonth);
    }
}
