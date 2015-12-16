package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.listeners.BodyIQSystemEventListener;
import com.intel.wearable.platform.bodyiq.refapp.listeners.BodyIQUserEventListener;
import com.intel.wearable.platform.core.notification.WearableNotification;

/**
 * Created by fissaX on 11/22/15.
 */
public class DeviceOptionsActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        setContentView(R.layout.deviceoptions);

        ColorDrawable colorDrawable = new ColorDrawable(ContextCompat.getColor(this, R.color.blue));
        getSupportActionBar().setBackgroundDrawable(colorDrawable);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setTitle(BodyIQAPIManager.getInstance().getDisplayName());


        findViewById(R.id.deviceinfo).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(DeviceOptionsActivity.this, DeviceDetailsActivity.class);
                startActivity(intent);
            }
        });

        findViewById(R.id.disconnect).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(BodyIQAPIManager.getInstance().isConnected()) {
                    BodyIQAPIManager.getInstance().disconnect();
                    invalidateOptionsMenu();
                }
            }

        });

        ((CheckBox)findViewById(R.id.userevents)).setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked)
                    BodyIQAPIManager.getInstance().subscribeToUserEvents(new BodyIQUserEventListener(DeviceOptionsActivity.this));
                else
                    BodyIQAPIManager.getInstance().unsubscribeToUserEvents();

            }
        });


        ((CheckBox)findViewById(R.id.systemevents)).setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked)
                    BodyIQAPIManager.getInstance().subscribeToSystemEvents(new BodyIQSystemEventListener(DeviceOptionsActivity.this));
                else
                    BodyIQAPIManager.getInstance().unsubscribeToSystemEvents();

            }
        });


        findViewById(R.id.notifications).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                          // BodyIQAPIManager.getInstance().sendLedNotification(WearableNotification.LedPattern.Type.LED_BLINK,colors,2,250,500,500);
                new Handler().post(new Runnable() {
                    @Override
                    public void run() {
                        BodyIQAPIManager.getInstance().sendHapticNotification(WearableNotification.VibrationPattern.Type.VIBRA_SQUARE, 2, 100, 500, 500);
                        int colors[] = new int[2];
                        colors[0] = ContextCompat.getColor(DeviceOptionsActivity.this, R.color.darkred);
                        colors[1] = ContextCompat.getColor(DeviceOptionsActivity.this, R.color.green);

                        BodyIQAPIManager.getInstance().sendLedNotification(WearableNotification.LedPattern.Type.LED_BLINK, colors, 2, 250, 500, 500);

                    }
                });
            }

        });


        findViewById(R.id.checkfirmware).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(DeviceOptionsActivity.this, FirmwareActivity.class);
                startActivity(intent);

            }

        });


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        if(BodyIQAPIManager.getInstance().isConnected())
            getMenuInflater().inflate(R.menu.menu_main, menu);
        else
            getMenuInflater().inflate(R.menu.menu_main_disconnected, menu);
        return true;
    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}