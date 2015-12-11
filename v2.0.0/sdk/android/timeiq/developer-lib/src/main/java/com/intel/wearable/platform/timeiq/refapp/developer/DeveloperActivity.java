package com.intel.wearable.platform.timeiq.refapp.developer;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.common.db.IDaoCommonMethods;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.IDeveloperFragment;


public class DeveloperActivity extends AppCompatActivity {

    private int m_previousSelectedTab = -1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Window window = getWindow();
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        window.setStatusBarColor(getResources().getColor(R.color.colorPrimary));
        setContentView(R.layout.activity_developer);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        DeveloperSectionsPagerAdapter developerSectionsPagerAdapter = new DeveloperSectionsPagerAdapter(getSupportFragmentManager(), this);

        final ViewPager viewPager = (ViewPager) findViewById(R.id.pager);
        viewPager.setAdapter(developerSectionsPagerAdapter);

        viewPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) { }

            @Override
            public void onPageSelected(int position) {
                Fragment fragment;
                if (m_previousSelectedTab != -1) {
                    fragment = getSupportFragmentManager().findFragmentByTag("android:switcher:" + R.id.pager + ":" + m_previousSelectedTab);
                    if (fragment instanceof IDeveloperFragment) {
                        ((IDeveloperFragment) fragment).onFragmentHidden();
                    }
                }

                m_previousSelectedTab = position;
                fragment = getSupportFragmentManager().findFragmentByTag("android:switcher:" + R.id.pager + ":" + position);
                if (fragment instanceof IDeveloperFragment) {
                    ((IDeveloperFragment) fragment).onFragmentShown();
                }
            }

            @Override
            public void onPageScrollStateChanged(int state) { }
        });

        TabLayout tabLayout = (TabLayout) findViewById(R.id.tabs);
        tabLayout.setupWithViewPager(viewPager);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.developer_menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_force_sync) {
            forceSync();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void forceSync() {
        final Context context = this;
        new AlertDialog.Builder(this)
                .setTitle(R.string.developer_fragment_force_sync_verification_title_text)
                .setMessage(R.string.developer_fragment_force_sync_verification_message_text)
                .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        try {
                            IDaoCommonMethods resolve = ClassFactory.getInstance().resolve(IDaoCommonMethods.class);
                            resolve.syncAllUserLocalMasterDBWithRemote();
                            resolve.syncFromAllRemoteMasterToLocalSlaves();
                            Toast.makeText(context, getString(R.string.developer_fragment_force_sync_working_text), Toast.LENGTH_SHORT).show();
                        } catch(Exception e) {
                            Toast.makeText(context, getString(R.string.developer_fragment_force_sync_failed_text, e.getMessage()), Toast.LENGTH_LONG).show();
                        }
                    }
                })
                .setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        Toast.makeText(context, getString(android.R.string.cancel), Toast.LENGTH_SHORT).show();
                        dialogInterface.cancel();
                    }
                })
                .show();
    }

}
