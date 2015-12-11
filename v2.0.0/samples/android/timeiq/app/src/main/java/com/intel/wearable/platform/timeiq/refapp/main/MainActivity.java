package com.intel.wearable.platform.timeiq.refapp.main;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.view.ViewPager;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.intel.wearable.platform.timeiq.refapp.AboutActivity;
import com.intel.wearable.platform.timeiq.refapp.ColoredStatusBarActivity;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.TimeIQBGService;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQInitUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQPlacesUtils;
import com.intel.wearable.platform.timeiq.refapp.auth.AuthUtil;
import com.intel.wearable.platform.timeiq.refapp.auth.ICredentialsProvider;
import com.intel.wearable.platform.timeiq.refapp.calendar.ActivityReadableCalendar;
import com.intel.wearable.platform.timeiq.refapp.calendar.ActivityWritableCalendar;
import com.intel.wearable.platform.timeiq.refapp.coreiq.CoreIQUtil;
import com.intel.wearable.platform.timeiq.refapp.coreiq.ScanningActivity;
import com.intel.wearable.platform.timeiq.refapp.googleAnalytics.GoogleAnalyticsTrackers;

public class MainActivity extends ColoredStatusBarActivity {

    private static final String TAG = MainActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d(TAG, "onCreate");

        GoogleAnalyticsTrackers.initialize(this);
        // start coreIQ:
        CoreIQUtil.getInstance().init(getApplicationContext());

        // start the timeIQ bg service:
        Intent intent = new Intent(this, TimeIQBGService.class);
        Log.d(TAG, "send start BG service intent");
        startService(intent);

        ICredentialsProvider authProvider = AuthUtil.getAuthProvider(getApplicationContext());
        authProvider.loadData();

        // start the TimeIQ api

        Log.d(TAG, "before SDK init");
        boolean initWasSuccessful = TimeIQInitUtils.initTimeIQ(getApplicationContext(), authProvider, AuthUtil.getCloudServerURL());
        Log.d(TAG, "after SDK init (success = " + initWasSuccessful + ")");

        if ( initWasSuccessful ) {

            setContentView(R.layout.activity_main);

            Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
            setSupportActionBar(toolbar);
            // Create the adapter that will return a fragment for each of the three
            // primary sections of the activity.
            SectionsPagerAdapter sectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager(), this);

            // Set up the ViewPager with the sections adapter.
            final ViewPager viewPagerContainer = (ViewPager) findViewById(R.id.container);
            viewPagerContainer.setAdapter(sectionsPagerAdapter);

            TabLayout tabLayout = (TabLayout) findViewById(R.id.tabs);
            tabLayout.setupWithViewPager(viewPagerContainer);

            viewPagerContainer.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
                @Override
                public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) { }

                @Override
                public void onPageSelected(int position) {
                    Fragment fragment = getSupportFragmentManager().findFragmentByTag("android:switcher:" + R.id.container + ":" + position);
                    if (fragment instanceof IMainFragment) {
                        ((IMainFragment) fragment).onFragmentShown();
                    }
                }

                @Override
                public void onPageScrollStateChanged(int state) { }
            });

            FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
            fab.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    Fragment currentFragment = getSupportFragmentManager().findFragmentByTag("android:switcher:" + R.id.container + ":" + viewPagerContainer.getCurrentItem());
                    if (currentFragment instanceof IMainFragment) {
                        ((IMainFragment) currentFragment).onFloatingButtonPressed();
                    }
                }
            });

            setHomeAndWorkIfNeeded();
        } else {
            //Failed to initialize
            new AlertDialog.Builder(this)
                    .setTitle("Error!")
                    .setMessage("Failed to initialize")
                    .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface arg0, int arg1) {
                            finish();
                        }
                    })
                    .show();
        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_readable_calendars) {
            startReadableCalendarDialog();
        } else if (id == R.id.action_writable_calendars) {
            startWritableCalendarDialog();
        }  else if (id == R.id.action_about) {
            startAboutActivity();
        } else if(id == R.id.action_pairing){
            startScanningActivity();
        }

        return super.onOptionsItemSelected(item);
    }

    private void startScanningActivity() {
        Intent scanningActivity = new Intent(getApplicationContext(), ScanningActivity.class);
        startActivity(scanningActivity);
    }

    private void startReadableCalendarDialog() {
        Intent readableCalendarActivity = new Intent(getApplicationContext(), ActivityReadableCalendar.class);
        startActivity(readableCalendarActivity);
    }

    private void startWritableCalendarDialog() {
        Intent writableCalendarActivity = new Intent(getApplicationContext(), ActivityWritableCalendar.class);
        startActivity(writableCalendarActivity);
    }

    private void startAboutActivity() {
        Intent aboutActivity = new Intent(getApplicationContext(), AboutActivity.class);
        startActivity(aboutActivity);
    }


    private void setHomeAndWorkIfNeeded() {
        TimeIQPlacesUtils.setPlaceFromAutoDetectedIfNotSet(this, TimeIQPlacesUtils.PlaceToAutoDetect.HOME);
        TimeIQPlacesUtils.setPlaceFromAutoDetectedIfNotSet(this, TimeIQPlacesUtils.PlaceToAutoDetect.WORK);
    }

}
