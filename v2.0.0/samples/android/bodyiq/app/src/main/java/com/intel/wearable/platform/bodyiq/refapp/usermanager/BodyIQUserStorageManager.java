package com.intel.wearable.platform.bodyiq.refapp.usermanager;

import android.content.Context;
import android.content.SharedPreferences;


/**
 * BodyIQUserStorageManager stores and retrieves active BodyIQUser from Shared Preferences
 */
public class BodyIQUserStorageManager {

    private static final String USER_UUID              = "uuid";
    private static final String USER_ID                = "userId";
    private static final String USER_NAME              = "name";
    private static final String USER_EMAIL             = "email";
    private static final String USER_PASSWORD          = "password";
    private static final String USER_GENDER            = "gender";
    private static final String USER_HEIGHT            = "height";
    private static final String USER_WEIGHT            = "weight";
    private static final String USER_DESIRED_WEIGHT    = "desiredWeight";
    private static final String USER_DEVICE_NAME       = "deviceName";
    private static final String USER_DEVICE_ADDRESS    = "deviceAddress";
    private static final String USER_PHONE             = "phone";
    private static final String USER_SIGNED            = "userSignedIn";


    public static boolean saveUser(Context context, BodyIQUser user) {

        SharedPreferences sp = context.getSharedPreferences(BodyIQUser.class
                .getName(), Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();

        editor.putString(USER_ID,user.getUserId());
        editor.putString(USER_NAME,user.getName());
        editor.putString(USER_EMAIL,user.getEmail());
        editor.putString(USER_PASSWORD,user.getPassword());
        editor.putInt(USER_GENDER, user.getGender());
        editor.putFloat(USER_HEIGHT, user.getHeight());
        editor.putFloat(USER_WEIGHT,user.getWeight());
        editor.putString(USER_DEVICE_NAME, user.getDeviceName());
        editor.putString(USER_DEVICE_ADDRESS, user.getDeviceAddress());
        editor.putString(USER_PHONE,user.getPhone());
        editor.putBoolean(USER_SIGNED,true);

        return editor.commit();
    }


    public static BodyIQUser getUser(Context context)
    {
        BodyIQUser user = null;

        SharedPreferences sp = context.getSharedPreferences(BodyIQUser.class
                .getName(), Context.MODE_PRIVATE);

        if(sp.getBoolean(USER_SIGNED,false))
        {
            user = new BodyIQUser();
            user.setUserId(sp.getString(USER_ID, ""));
            user.setName(sp.getString(USER_NAME, ""));
            user.setEmail(sp.getString(USER_EMAIL, ""));
            user.setPassword(sp.getString(USER_PASSWORD, ""));
            user.setHeight(sp.getFloat(USER_HEIGHT, 0f));
            user.setWeight(sp.getFloat(USER_WEIGHT, 0f));
            user.setGender(sp.getInt(USER_GENDER, BodyIQUser.MALE));
            user.setPhone(sp.getString(USER_PHONE, ""));
            user.setDeviceName(sp.getString(USER_DEVICE_NAME, ""));
            user.setDeviceAddress(sp.getString(USER_DEVICE_ADDRESS, ""));
        }

        return user;

    }



}
