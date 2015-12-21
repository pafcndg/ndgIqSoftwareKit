package com.intel.wearable.platform.timeiq.refapp.auth;

import android.content.Context;

/**
 * Created by smoradof on 11/10/2015.
 */
public class AuthUtil {


    private static final String CSW_CLOUD_SERVER_URL = "https://tsosdk.iqsoftwarekit.intel.com";

    public static ICredentialsProvider getAuthProvider(Context context) {
        return CSWAuthCredentialsProvider.getInstance(context);
    }


    public static String getCloudServerURL() {
        return CSW_CLOUD_SERVER_URL;
    }

}
