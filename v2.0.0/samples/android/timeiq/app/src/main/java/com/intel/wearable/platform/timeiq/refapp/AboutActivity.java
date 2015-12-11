package com.intel.wearable.platform.timeiq.refapp;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

public class AboutActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_about);
        TextView versionTextView = (TextView) findViewById(R.id.app_versionTextView);
        TextView buildDateTextView = (TextView) findViewById(R.id.build_date_textView);
        buildDateTextView.setText(BuildConfig.BUILD_DATE_FULL);

        try {
            String versionName = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
            versionTextView.setText(versionName);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
    }
}
