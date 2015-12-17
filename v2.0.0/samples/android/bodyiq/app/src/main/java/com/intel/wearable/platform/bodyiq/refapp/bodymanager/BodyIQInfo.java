package com.intel.wearable.platform.bodyiq.refapp.bodymanager;

/**
 * Created by fissaX on 12/8/15.
 */
public class BodyIQInfo {

    public String type;
    public String status;
    public String duration;
    public String distance;


    public BodyIQInfo()
    {
        distance = duration = "0";
        type = status = "-";
    }

}
