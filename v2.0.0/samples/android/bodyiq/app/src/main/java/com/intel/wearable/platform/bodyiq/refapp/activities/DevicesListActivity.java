package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.listeners.BodyIQConnectListener;
import com.intel.wearable.platform.bodyiq.refapp.listeners.BodyIQScannerListener;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUserStorageManager;
import com.intel.wearable.platform.core.device.WearableToken;


import java.util.ArrayList;

/**
 * Created by fissaX on 12/2/15.
 */
public class DevicesListActivity extends AppCompatActivity {

    private ViewGroup                   containerView   = null;
    private TextView                    emptyListText   = null;
    private ProgressBar                 progressBar     = null;
    private ArrayList<WearableToken>    foundDevices    = new ArrayList<>();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.deviceslist);
        containerView = (ViewGroup) findViewById(R.id.container);

        ColorDrawable colorDrawable = new ColorDrawable(ContextCompat.getColor(this, R.color.blue));
        getSupportActionBar().setBackgroundDrawable(colorDrawable);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setTitle(getResources().getString(R.string.bledevices));

        emptyListText = (TextView)findViewById(R.id.empty);
        progressBar = (ProgressBar)findViewById(R.id.progressbar);
        progressBar.getIndeterminateDrawable().setColorFilter(ContextCompat.getColor(this, R.color.blue), android.graphics.PorterDuff.Mode.MULTIPLY);

        triggerScanner();

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

    public void addFoundDevice(WearableToken token)
    {
        addItem(token.getDisplayName());
        foundDevices.add(token);
    }


    private void triggerScanner()
    {
        BodyIQAPIManager.getInstance().startScan(new BodyIQScannerListener(this,this,progressBar));
    }



    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return true;
    }

    private void addItem(String devicename) {

        final ViewGroup newView = (ViewGroup) LayoutInflater.from(this).inflate(
                R.layout.devicerow, containerView, false);

        if(emptyListText.getVisibility()==View.VISIBLE)
            emptyListText.setVisibility(View.GONE);

        devicename = (devicename!=null && devicename.length()>0) ? devicename : getResources().getString(R.string.noname);

        ((TextView) newView.findViewById(android.R.id.text1)).setText(devicename);
        newView.setTag(new Integer(containerView.getChildCount()));

        newView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                int number = ((Integer) v.getTag()).intValue();
                if (BodyIQAPIManager.getInstance().isConnected()) {
                    Intent intent = new Intent(DevicesListActivity.this, TrackerActivity.class);
                    startActivity(intent);
                } else {
                    BodyIQAPIManager bodyIQAPIManager = BodyIQAPIManager.getInstance();
                    bodyIQAPIManager.getUser().setDeviceName(foundDevices.get(number).getDisplayName());
                    bodyIQAPIManager.getUser().setDeviceAddress(foundDevices.get(number).getAddress());
                    bodyIQAPIManager.saveUser();
                    bodyIQAPIManager.connect(foundDevices.get(number), new BodyIQConnectListener(DevicesListActivity.this, progressBar));
                }

            }
        });

        containerView.addView(newView, 0);
    }


}
