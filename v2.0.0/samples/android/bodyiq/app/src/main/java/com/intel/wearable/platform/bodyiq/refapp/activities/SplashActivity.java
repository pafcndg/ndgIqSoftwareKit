package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.listeners.BodyIQCoreInitListener;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUserStorageManager;


/**
 * Created by fissaX on 11/22/15.
 */
public class SplashActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.splashactivity);

        BodyIQAPIManager.getInstance().init(getApplicationContext(),new BodyIQCoreInitListener(this));

    }
}