package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.design.widget.NavigationView;
import android.support.v4.app.FragmentTabHost;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TabHost;
import android.widget.TextView;
import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.bodymanager.BodyIQManager;
import com.intel.wearable.platform.bodyiq.refapp.fragments.ActivitiesTabFragment;
import com.intel.wearable.platform.bodyiq.refapp.fragments.SessionTabFragment;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUser;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUserStorageManager;


/**
 * Created by fissaX on 12/2/15.
 */
public class TrackerActivity extends AppCompatActivity {

    private DrawerLayout mDrawerLayout;
    private FragmentTabHost tabHost;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.trackeractivity);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitle(getResources().getString(R.string.dailytracker));
        toolbar.setTitleTextColor(ContextCompat.getColor(this, R.color.white));
        setSupportActionBar(toolbar);

        final ActionBar ab = getSupportActionBar();
        ab.setHomeAsUpIndicator(R.drawable.ic_menu);
        ab.setDisplayHomeAsUpEnabled(true);

        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        setupDrawerListener();


        BodyIQUser user = BodyIQAPIManager.getInstance().getUser();
        ((TextView) findViewById(R.id.name)).setText(user.getName());
        ((TextView)findViewById(R.id.email)).setText(user.getEmail());
        ((TextView)findViewById(R.id.devicename)).setText(BodyIQAPIManager.getInstance().getDisplayName());

        setupTabs();

        setupDrawerOptionSelectedListeners();

        if(BodyIQAPIManager.getInstance().isConnected())
            findViewById(R.id.connectstatus).setBackgroundResource(R.drawable.connectedcircle);
        else
            findViewById(R.id.connectstatus).setBackgroundResource(R.drawable.disconnectedcircle);


        findViewById(R.id.dailytracker).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDrawerLayout.closeDrawers();
                tabHost.setCurrentTab(1);
            }
        });

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        if (navigationView != null) {
            setupDrawerContent(navigationView);
        }


    }


    @Override
    protected void onResume() {
        super.onResume();
        BodyIQManager.setBodyIQUserProfile();
        invalidateOptionsMenu();

    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
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
                mDrawerLayout.openDrawer(GravityCompat.START);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void setupTabs()
    {
        tabHost = (FragmentTabHost)findViewById(android.R.id.tabhost);

        tabHost.setup(this, getSupportFragmentManager(), android.R.id.tabcontent);

        tabHost.addTab(
                tabHost.newTabSpec("tab1").setIndicator(getResources().getString(R.string.allactivity), null),
                ActivitiesTabFragment.class, null);

        tabHost.addTab(
                tabHost.newTabSpec("tab2").setIndicator(getResources().getString(R.string.sessions), null),
                SessionTabFragment.class, null);



        for(int i=0;i<tabHost.getTabWidget().getChildCount();i++)
        {
            TextView tv = (TextView) tabHost.getTabWidget().getChildAt(i).findViewById(android.R.id.title);
            if(tv!=null)
                tv.setTextColor(ContextCompat.getColor(this, R.color.white));
        }

        tabHost.setOnTabChangedListener(new TabHost.OnTabChangeListener() {
            @Override
            public void onTabChanged(String tabId) {
                if(tabId.equals("tab1"))
                    BodyIQAPIManager.getInstance().unsubscribeToBodyActivities();



            }
        });

    }

    private void setupDrawerListener()
    {
        mDrawerLayout.setDrawerListener(new DrawerLayout.DrawerListener() {
            @Override
            public void onDrawerSlide(View drawerView, float slideOffset) {

                if(slideOffset==1) // opening, set latest battery value
                {
                    updateBatteryAndConnectIcon();
                }
            }

            @Override
            public void onDrawerOpened(View drawerView) {
                updateBatteryAndConnectIcon();
            }

            @Override
            public void onDrawerClosed(View drawerView) {

            }

            @Override
            public void onDrawerStateChanged(int newState) {

            }
        });

    }


    private void setupDrawerContent(NavigationView navigationView) {
        navigationView.setNavigationItemSelectedListener(
                new NavigationView.OnNavigationItemSelectedListener() {
                    @Override
                    public boolean onNavigationItemSelected(MenuItem menuItem) {
                        menuItem.setChecked(true);
                        mDrawerLayout.closeDrawers();
                        return true;
                    }
                });
    }

    private void setupDrawerOptionSelectedListeners()
    {
        findViewById(R.id.deviceinfo).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(TrackerActivity.this, DeviceOptionsActivity.class);
                startActivity(intent);
                mDrawerLayout.closeDrawers();

            }
        });

        findViewById(R.id.allactivity).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDrawerLayout.closeDrawers();
                tabHost.setCurrentTab(0);

            }
        });

        findViewById(R.id.dailytracker).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDrawerLayout.closeDrawers();
                tabHost.setCurrentTab(1);

            }
        });
    }


    private void updateBatteryAndConnectIcon()
    {
        ((TextView)findViewById(R.id.batteryvalue)).setText(BodyIQAPIManager.getInstance().getBatteryStatus()+" %");
        if(BodyIQAPIManager.getInstance().isConnected())
            findViewById(R.id.connectstatus).setBackgroundResource(R.drawable.connectedcircle);
        else
            findViewById(R.id.connectstatus).setBackgroundResource(R.drawable.disconnectedcircle);

    }


}
