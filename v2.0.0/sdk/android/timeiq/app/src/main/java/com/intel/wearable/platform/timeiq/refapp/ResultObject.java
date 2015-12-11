package com.intel.wearable.platform.timeiq.refapp;

/**
 * Created by smoradof on 11/3/2015.
 */
public class ResultObject<T> {

    private final T mObject;
    private final String mMsg;
    private final boolean mSuccess;

    public ResultObject(boolean success, String msg, T object) {
        mObject = object;
        mMsg = msg;
        mSuccess = success;
    }


    public T getData() {
        return mObject;
    }

    public String getMsg(){
        return mMsg;
    }

    public boolean isSuccess(){
        return mSuccess;
    }

}
