package com.intel.wearable.platform.bodyiq.refapp.listeners;
import android.content.Intent;

import com.intel.wearable.platform.bodyiq.refapp.activities.AccountActivity;
import com.intel.wearable.platform.bodyiq.refapp.activities.SplashActivity;
import com.intel.wearable.platform.core.ICoreInitListener;

/**
 * Created by fissaX on 12/11/15.
 */
public class BodyIQCoreInitListener implements ICoreInitListener {

    SplashActivity activity = null;

    public BodyIQCoreInitListener(SplashActivity activity)
    {
        this.activity = activity;
    }

    @Override
    public void onInitialized() {

        Intent intent = new Intent(activity.getApplicationContext(), AccountActivity.class);
        activity.startActivity(intent);
        activity.finish();

    }
}
