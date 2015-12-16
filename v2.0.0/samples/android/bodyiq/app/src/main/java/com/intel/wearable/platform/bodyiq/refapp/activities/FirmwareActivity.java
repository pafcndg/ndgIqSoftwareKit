package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQFirmwareManager;


/**
 * Created by fissaX on 11/22/15.
 */
public class FirmwareActivity extends AppCompatActivity {

    String currentFirmware = "";
    ProgressBar progressBar = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        setContentView(R.layout.firmwareactivity);
        ColorDrawable colorDrawable = new ColorDrawable(ContextCompat.getColor(this, R.color.blue));
        getSupportActionBar().setBackgroundDrawable(colorDrawable);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setTitle(getResources().getString(R.string.firmwareupdate));

        currentFirmware = BodyIQAPIManager.getInstance().getFirmwareInstallController().getWearableFirmwareVersion();

        progressBar = (ProgressBar)findViewById(R.id.progressbar);
        progressBar.getIndeterminateDrawable().setColorFilter(ContextCompat.getColor(this, R.color.blue), android.graphics.PorterDuff.Mode.MULTIPLY);

        final BodyIQFirmwareManager firmwareHandler = new BodyIQFirmwareManager(FirmwareActivity.this);

        ((TextView)findViewById(R.id.firmwarestatus)).setText(getResources().getString(R.string.checkupdatedesc));
        ((Button)findViewById(R.id.handlefirmware)).setText(getResources().getString(R.string.checkupdate));

        findViewById(R.id.handlefirmware).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                progressBar.setVisibility(View.VISIBLE);

                if(((Button)v).getText().equals(getResources().getString(R.string.checkupdate)))
                {
                   firmwareHandler.checkForUpdate();

                }
                else
                {
                    if(((Button)v).getText().equals(getResources().getString(R.string.download)))
                    {
                        firmwareHandler.dowanload();
                    }
                    else
                    {
                        if(((Button)v).getText().equals(getResources().getString(R.string.upload)))
                        {
                            firmwareHandler.upload();
                        }
                    }
                }
            }
        });

    }


    public void askToDownloadNewVersion()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((TextView) findViewById(R.id.firmwarestatus)).setText(getResources().getString(R.string.newversion));
                ((Button) findViewById(R.id.handlefirmware)).setText(getResources().getString(R.string.download));
                progressBar.setVisibility(View.GONE);
            }
        });
    }

    public void askToUploadNewVersion()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((TextView) findViewById(R.id.firmwarestatus)).setText(getResources().getString(R.string.newversiondownloaded));
                ((Button) findViewById(R.id.handlefirmware)).setText(getResources().getString(R.string.upload));
                progressBar.setVisibility(View.GONE);
            }
        });
    }

    public void finishedUpload(final boolean isSuccessful)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(isSuccessful) {
                    ((TextView) findViewById(R.id.firmwarestatus)).setText(getResources().getString(R.string.versioninstalled));
                    progressBar.setVisibility(View.GONE);
                    finish();
                }
                else
                {
                    ((TextView) findViewById(R.id.firmwarestatus)).setText(getResources().getString(R.string.failedversion));
                    progressBar.setVisibility(View.GONE);
                    finish();
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