package com.intel.wearable.platform.timeiq.refapp.bodyiq;

import android.content.res.ColorStateList;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.intel.wearable.platform.core.body.WearableBodyActivity;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.main.IMainFragment;

import java.util.concurrent.TimeUnit;

/**
 * Created by adura on 22/10/2015.
 */


/**
 * Fragment holding the Event list
 */
public class BodyIQFragment extends Fragment implements IMainFragment, BodyIQUtil.IBodyIqListener {

    private static final String TAG = BodyIQFragment.class.getSimpleName();

    private TextView mDailyActivityTitleTextView;
    private TextView mDailyLine_1_TextView;
    private TextView mDailyLine_2_TextView;
    private TextView mDailyLine_3_TextView;

    public BodyIQFragment() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_daily_activity, container, false);

        mDailyActivityTitleTextView = (TextView) rootView.findViewById(R.id.daily_activity_title);

        mDailyLine_1_TextView = (TextView) rootView.findViewById(R.id.daily_activity_line1);
        mDailyLine_2_TextView = (TextView) rootView.findViewById(R.id.daily_activity_line2);
        mDailyLine_3_TextView = (TextView) rootView.findViewById(R.id.daily_activity_line3);

        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
        BodyIQUtil.getInstance().setBodyIqListener(this);
        if (isAdded()) {
            refreshUi();
        }
    }

    /**
     * Refresh the event list. display empty list message if none
     */
    private synchronized void refreshUi() {
        BodyIQUtil bodyIQUtil = BodyIQUtil.getInstance();
        final boolean connected = bodyIQUtil.isConnected();
        final int totalSteps = bodyIQUtil.getTotalSteps();
        final int totalMeters = (int) bodyIQUtil.getTotalMeters();
        final boolean currentlyActive = bodyIQUtil.isCurrentlyActive();
        final long currentStartTime = bodyIQUtil.getCurrentStartTime();
        final int currentStepCount = bodyIQUtil.getCurrentStepCount();
        final int currentMetersTraveled = (int) bodyIQUtil.getCurrentMetersTraveled();
        final WearableBodyActivity.ActivityType currentType = bodyIQUtil.getCurrentType();

        FragmentActivity activity = getActivity();
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (connected) {

                    if (totalSteps > 0 || totalMeters > 0) {
                        mDailyActivityTitleTextView.setText(getString(R.string.activity_today, totalSteps, totalMeters));
                    } else {
                        mDailyActivityTitleTextView.setText(R.string.no_steps_today);
                    }

                    if (currentlyActive) {
                        int speed = 0;
                        long duration = 0;
                        if (currentStartTime > 0) {
                            duration = System.currentTimeMillis() - currentStartTime;
                        }

                        int durationMinutes = (int) TimeUnit.MILLISECONDS.toMinutes(duration);
                        long durationSeconds = TimeUnit.MILLISECONDS.toSeconds(duration);
                        if (durationMinutes > 0 && currentStepCount > 0) {
                            speed = (int) (currentStepCount / durationMinutes);
                        }
                        String currentTypeStr = currentType.name().toLowerCase();
                        currentTypeStr = Character.toUpperCase(currentTypeStr.charAt(0)) + currentTypeStr.substring(1);
                        mDailyLine_1_TextView.setText(getString(R.string.activity_past, currentTypeStr, "" + (speed > 0 ? speed : "--")));

                        mDailyLine_2_TextView.setText(getString(R.string.activity_duration, "" + durationMinutes, "" + (durationSeconds - (durationMinutes * 60))));
                        mDailyLine_3_TextView.setText(getString(R.string.activity_steps_and_distance, "" + currentStepCount, "" + currentMetersTraveled));
                    } else {
                        mDailyLine_1_TextView.setText(R.string.start_moving);
                        mDailyLine_2_TextView.setText("");
                        mDailyLine_3_TextView.setText("");
                    }
                } else {
                    mDailyActivityTitleTextView.setText(R.string.not_connected);
                    mDailyLine_1_TextView.setText(R.string.please_connect_to_see_activity);
                    mDailyLine_2_TextView.setText("");
                    mDailyLine_3_TextView.setText("");
                }
            }
        });
    }

    @Override
    public void onFragmentShown() {
        Log.d(TAG, "onFragmentShown");
        BodyIQUtil.getInstance().setBodyIqListener(this);
        refreshUi();
        FragmentActivity activity = getActivity();
        if(activity != null) {
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
        // Refresh the UI
        BodyIQUtil.getInstance().reCalcActivitySeriesForToday();
        refreshUi();
    }

    @Override
    public void onPause() {
        Log.d(TAG, "onPause ");
        BodyIQUtil.getInstance().setBodyIqListener(null);
        super.onPause();
    }

    @Override
    public void onUpdate() {
        refreshUi();
    }
}
