package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;


/**
 * Created by fissaX on 11/22/15.
 */
public class DeviceDetailsActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        setContentView(R.layout.devicesdetails);

        ColorDrawable colorDrawable = new ColorDrawable(ContextCompat.getColor(this, R.color.blue));
        getSupportActionBar().setBackgroundDrawable(colorDrawable);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setTitle(BodyIQAPIManager.getInstance().getDisplayName());


        ((TextView)findViewById(R.id.hardwarerevision)).setText(BodyIQAPIManager.getInstance().getHardwareRevision());
        ((TextView)findViewById(R.id.devicename)).setText(BodyIQAPIManager.getInstance().getDisplayName());
        ((TextView)findViewById(R.id.deviceaddress)).setText(BodyIQAPIManager.getInstance().getAddress());
        ((TextView)findViewById(R.id.softwarerevision)).setText(BodyIQAPIManager.getInstance().getSoftwareRevision());
        ((TextView)findViewById(R.id.serialnumber)).setText(BodyIQAPIManager.getInstance().getSerialNumber());
        ((TextView)findViewById(R.id.firmwarerevision)).setText(BodyIQAPIManager.getInstance().getFirmwareRevision());
        ((TextView)findViewById(R.id.modelname)).setText(BodyIQAPIManager.getInstance().getModelName());
        ((TextView)findViewById(R.id.manufacturer)).setText(BodyIQAPIManager.getInstance().getManufacturer());

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
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