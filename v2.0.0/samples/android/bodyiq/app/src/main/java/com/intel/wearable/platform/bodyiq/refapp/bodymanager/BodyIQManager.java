package com.intel.wearable.platform.bodyiq.refapp.bodymanager;

import com.intel.wearable.platform.body.model.ActivityInterval;
import com.intel.wearable.platform.body.model.ActivityStepInterval;
import com.intel.wearable.platform.body.model.BiologicalSex;
import com.intel.wearable.platform.body.model.Profile;
import com.intel.wearable.platform.body.model.TimeSeries;
import com.intel.wearable.platform.body.persistence.BodyDataStore;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUser;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUserStorageManager;
import com.intel.wearable.platform.core.model.datastore.UserIdentity;
import java.text.DecimalFormat;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.TimeUnit;

/**
 * Created by fissaX on 12/8/15.
 */
public class BodyIQManager {

    List<TimeSeries<ActivityInterval>> seriesList;

    final float MS_TO_KMH = 60.0f * 60.0f / 1000.0f;

    public BodyIQManager(long start, long end)
    {
        // Query for activity
        seriesList = BodyIQAPIManager.getInstance().getBodyActivityAccess().fetchSeries(start, end);
    }

    public BodyIQInfoSummary getBodyInfo()
    {

        BodyIQInfoSummary info = new BodyIQInfoSummary();
        long        duration = 0;
        float       distance = 0;
        long        longestActivity = 0;
        int         calories = 0;
        int         steps = 0;

        // Only steps activities

        for(TimeSeries<ActivityInterval> series : seriesList) {

            @SuppressWarnings({"unchecked"})
            Iterator<ActivityInterval> it = (Iterator<ActivityInterval>) series.intervals.iterator();
            while (it.hasNext()) {
                ActivityInterval activity = it.next();
                if (activity instanceof ActivityStepInterval) {

                    long temp = ((ActivityStepInterval) activity).end.getMillis() - ((ActivityStepInterval) activity).start.getMillis();
                    duration += temp;
                    distance += ((ActivityStepInterval) activity).metersTraveled;
                    longestActivity = (temp > longestActivity) ? temp : longestActivity;
                    calories += ((ActivityStepInterval) activity).caloriesBurned;
                    steps += ((ActivityStepInterval) activity).stepCount;
                }
            }
        }


        DecimalFormat format = new DecimalFormat("#.##");

        info.distance = String.valueOf(format.format(distance));
        info.calories = calories;
        info.steps = steps;
        info.longestActivity = String.format("%02d:%02d:%02d",
                TimeUnit.MILLISECONDS.toHours(longestActivity),
                TimeUnit.MILLISECONDS.toMinutes(longestActivity) -
                        TimeUnit.HOURS.toMinutes(TimeUnit.MILLISECONDS.toHours(longestActivity)),
                TimeUnit.MILLISECONDS.toSeconds(longestActivity) -
                        TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes(longestActivity)));



        if(distance>0)
            info.avgSpeed = format.format((distance*MS_TO_KMH)/(duration/1000.0));

               info.duration = String.format("%02d:%02d:%02d",
                       TimeUnit.MILLISECONDS.toHours(duration),
                       TimeUnit.MILLISECONDS.toMinutes(duration) -
                               TimeUnit.HOURS.toMinutes(TimeUnit.MILLISECONDS.toHours(duration)),
                       TimeUnit.MILLISECONDS.toSeconds(duration) -
                               TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes(duration)));

        return info;

    }


    public static void setBodyIQUserProfile() {
        final Profile currentProfile = BodyDataStore.getCurrentProfile();
        BodyIQUser user = BodyIQAPIManager.getInstance().getUser();
        String names[] = user.getName().split(" ");
        final UserIdentity identity = new UserIdentity(currentProfile.user.getUuid(), currentProfile.user.getUserId(), names[0], names[1], user.getEmail(), "111111");
        BodyDataStore.setCurrentUserIdentity(identity);

        switch (user.getGender())
        {
            case BodyIQUser.MALE:
                BodyDataStore.setBiologicalSex(BiologicalSex.MALE);
                break;
            case BodyIQUser.FEMALE:
                BodyDataStore.setBiologicalSex(BiologicalSex.FEMALE);
                break;
            case BodyIQUser.UNKNOWN:
                BodyDataStore.setBiologicalSex(BiologicalSex.UNKNOWN);
                break;

        }
        BodyDataStore.setBiologicalSex(BiologicalSex.MALE);
        BodyDataStore.setHeight((double) user.getHeight());
        BodyDataStore.setWeight((double) user.getWeight());
    }



}
