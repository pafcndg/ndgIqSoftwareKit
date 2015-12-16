package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.content.Intent;
import android.graphics.Paint;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;
import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.listeners.BodyIQConnectListener;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUser;
import com.intel.wearable.platform.core.device.WearableToken;

/**
 * Created by fissaX on 11/22/15.
 */
public class DeviceSyncActivity extends AppCompatActivity {

    ProgressBar progressBar = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        setContentView(R.layout.syncdeviceactivity);

        TextView privacyText = ((TextView)findViewById(R.id.privacy));
        privacyText.setPaintFlags(privacyText.getPaintFlags() | Paint.UNDERLINE_TEXT_FLAG);

        TextView termsText = ((TextView)findViewById(R.id.terms));
        termsText.setPaintFlags(privacyText.getPaintFlags() | Paint.UNDERLINE_TEXT_FLAG);

        progressBar = (ProgressBar)findViewById(R.id.progressbar);
        progressBar.getIndeterminateDrawable().setColorFilter(ContextCompat.getColor(this, R.color.blue), android.graphics.PorterDuff.Mode.MULTIPLY);


        ColorDrawable colorDrawable = new ColorDrawable(ContextCompat.getColor(this, R.color.blue));
        getSupportActionBar().setBackgroundDrawable(colorDrawable);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setTitle(getResources().getString(R.string.syncdevicetitle));

        findViewById(R.id.syncdevice).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                BodyIQUser user = BodyIQAPIManager.getInstance().getUser();
                if(user.getDeviceAddress().length()==0) {
                    Intent intent = new Intent(DeviceSyncActivity.this, DevicesListActivity.class);
                    startActivity(intent);
                }
                else
                {
                    WearableToken wearableToken = new WearableToken(user.getDeviceAddress(), user.getName());
                    BodyIQAPIManager.getInstance().connect(wearableToken, new BodyIQConnectListener(DeviceSyncActivity.this,progressBar));
                }
            }
        });

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