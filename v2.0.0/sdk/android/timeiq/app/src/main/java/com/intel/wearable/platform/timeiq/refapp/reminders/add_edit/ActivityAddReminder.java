package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.os.Bundle;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.events.BeEvent;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQEventsUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQRemindersUtils;
import com.intel.wearable.platform.timeiq.refapp.googleAnalytics.GoogleAnalyticsTrackers;

public class ActivityAddReminder extends BaseActivityEditAndAddBeEvent {

    /**
     * The {@link android.support.v4.view.PagerAdapter} that will provide
     * fragments for each of the sections. We use a
     * {@link FragmentPagerAdapter} derivative, which will keep every
     * loaded fragment in memory. If this becomes too memory intensive, it
     * may be best to switch to a
     * {@link android.support.v4.app.FragmentStatePagerAdapter}.
     */
    private EditReminderPagerAdapter mSectionsPagerAdapter;

    /**
     * The {@link ViewPager} that will host the section contents.
     */
    private ViewPager mViewPager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        createContentView(R.layout.activity_add_reminder, R.string.reminder_title_new);

        setCreateReminderIcon(getString(R.string.reminder_error_missing_data));

        // Create the adapter that will return a fragment for each of the three
        // primary sections of the activity.
        mSectionsPagerAdapter = new EditReminderPagerAdapter(getSupportFragmentManager(), this, this);

        // Set up the ViewPager with the sections adapter.
        mViewPager = (ViewPager) findViewById(R.id.container);
        mViewPager.setAdapter(mSectionsPagerAdapter);

        mViewPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) { }

            @Override
            public void onPageSelected(int position) {
                Fragment currentFragment = getSupportFragmentManager().findFragmentByTag("android:switcher:" + R.id.container + ":" + mViewPager.getCurrentItem());
                if (currentFragment instanceof IReminderFragment) {
                    String okToCreateReminder = ((IReminderFragment) currentFragment).isOkToCreateReminder();
                    setCreateReminderIcon(okToCreateReminder);
                }
            }

            @Override
            public void onPageScrollStateChanged(int state) { }
        });

        TabLayout tabLayout = (TabLayout) findViewById(R.id.tabs);
        tabLayout.setupWithViewPager(mViewPager);
        setTabIconsAndText(tabLayout);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_edit_reminder, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void setTabIconsAndText(TabLayout tabLayout)
    {
        for ( EditReminderPagerAdapter.EDIT_REMINDER_FRAGMENTS reminderFragment : EditReminderPagerAdapter.EDIT_REMINDER_FRAGMENTS.values())
        {
            TextView tabTitle = (TextView) LayoutInflater.from(this).inflate(R.layout.tab_reminder, null);
            int pos = reminderFragment.ordinal();
            int icon = mSectionsPagerAdapter.getPageIcon(pos);
            CharSequence title = mSectionsPagerAdapter.getPageTitle(pos);

            tabTitle.setText(title);
            tabTitle.setCompoundDrawablesWithIntrinsicBounds(0, icon, 0, 0);
            tabTitle.setGravity(Gravity.CENTER);
            tabLayout.getTabAt(pos).setCustomView(tabTitle);
        }
    }

    @Override
    protected void actionOnOk() {
        addReminder();
    }

    private void addReminder() {
        String toastMessage;
        Fragment currentFragment = getSupportFragmentManager().findFragmentByTag("android:switcher:" + R.id.container + ":" + mViewPager.getCurrentItem());
        if(currentFragment instanceof FragmentEditBe){
            FragmentEditBe fragmentEditBe = (FragmentEditBe)currentFragment;
            BeEvent beEvent = fragmentEditBe.createBeEvent( );
            if(beEvent != null) {
                String errorMsg = TimeIQEventsUtils.addBeEvent(this, beEvent);
                if(errorMsg == null){
                    toastMessage = getString(R.string.be_event_was_added);
                }
                else{
                    toastMessage = errorMsg;
                }
            }
            else{
                toastMessage = getString(R.string.be_event_was_not_added);
            }
        } else { // there are only two types of fragments
            BaseFragmentEditReminder baseFragmentEditReminder = (BaseFragmentEditReminder) currentFragment;

            ResultObject<IReminder> reminderResult = baseFragmentEditReminder.createReminder();
            if (reminderResult.isSuccess()) {
                // add the reminder
                IReminder reminder = reminderResult.getData();
                String errorMsg = TimeIQRemindersUtils.addReminder(reminder);
                if (errorMsg == null) { // reminder added OK
                    toastMessage = getString(R.string.reminder_was_added);
                    GoogleAnalyticsTrackers.getInstance().trackEvent(R.string.google_analytics_reminder, R.string.google_analytics_add, reminder.getReminderType().name() + " (" + reminder.getTrigger().getTriggerType().name() + ")");
                } else { // reminder was not added with the error:
                    toastMessage = getString(R.string.reminder_was_not_added) + errorMsg;
                }
            } else {
                toastMessage = getString(R.string.could_not_create_reminder, reminderResult.getMsg());
            }

        }

        Toast.makeText(ActivityAddReminder.this, toastMessage, Toast.LENGTH_LONG).show();
        finish();
    }

}
