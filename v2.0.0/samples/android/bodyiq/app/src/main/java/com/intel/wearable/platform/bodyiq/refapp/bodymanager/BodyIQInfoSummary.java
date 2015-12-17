package com.intel.wearable.platform.bodyiq.refapp.bodymanager;

/**
 * Created by fissaX on 12/8/15.
 */
public class BodyIQInfoSummary extends BodyIQInfo {

    public String distance;
    public String duration;
    public String avgSpeed;
    public String longestActivity;
    public int calories;
    public int steps;


    public BodyIQInfoSummary()
    {
        calories = steps = 0;
        distance = duration = "0";
        avgSpeed = longestActivity = "-";
    }

}
