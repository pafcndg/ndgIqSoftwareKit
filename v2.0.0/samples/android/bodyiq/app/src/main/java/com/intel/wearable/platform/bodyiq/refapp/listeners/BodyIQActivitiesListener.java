package com.intel.wearable.platform.bodyiq.refapp.listeners;

import com.intel.wearable.platform.body.listen.ActivityIntervalListener;
import com.intel.wearable.platform.body.model.ActivityInterval;
import com.intel.wearable.platform.body.model.ActivityStepInterval;
import com.intel.wearable.platform.bodyiq.refapp.bodymanager.BodyIQInfo;
import com.intel.wearable.platform.bodyiq.refapp.fragments.SessionTabFragment;
import com.intel.wearable.platform.core.body.WearableBodyActivity;
import com.intel.wearable.platform.core.body.WearableBodyActivity.ActivityType;

import java.text.DecimalFormat;
import java.util.concurrent.TimeUnit;

/**
 * Created by fissaX on 12/9/15.
 */
public class BodyIQActivitiesListener implements ActivityIntervalListener {


    SessionTabFragment sessionsHandler = null;

    public BodyIQActivitiesListener(SessionTabFragment sessionsHandler)
    {
       this.sessionsHandler = sessionsHandler;
    }


    @Override
    public void onActivityInterval(ActivityInterval interval) {
        BodyIQInfo info = new BodyIQInfo();

        if(interval instanceof ActivityStepInterval) {
            if (interval.type.id == ActivityType.WALKING.id) {
                info.type = "Walking";

            } else {
                if (interval.type.id == ActivityType.RUNNING.id)
                    info.type = "Running";
                else
                    info.type = "Biking";
            }

            long millisDuration = interval.end.getMillis()-interval.start.getMillis();
            info.duration = String.format("%d min, %d sec",
                    TimeUnit.MILLISECONDS.toMinutes(millisDuration),
                    TimeUnit.MILLISECONDS.toSeconds(millisDuration) -
                            TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes(millisDuration))
            );

            info.status = interval.status.name();

            DecimalFormat format = new DecimalFormat("#.##");

            info.distance = String.valueOf(format.format(((ActivityStepInterval) interval).metersTraveled));

        }

        sessionsHandler.addItem(info);
    }
}
