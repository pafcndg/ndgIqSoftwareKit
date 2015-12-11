package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.common.result.Result;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderType;
import com.intel.wearable.platform.timeiq.api.reminders.doReminder.DoReminder;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.callReminder.CallReminder;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.notificationReminder.NotificationReminder;
import com.intel.wearable.platform.timeiq.api.triggers.ITrigger;
import com.intel.wearable.platform.timeiq.api.triggers.TriggerType;
import com.intel.wearable.platform.timeiq.api.triggers.place.PlaceTrigger;
import com.intel.wearable.platform.timeiq.api.triggers.place.PlaceTriggerType;
import com.intel.wearable.platform.timeiq.api.triggers.time.TimeTrigger;
import com.intel.wearable.platform.timeiq.refapp.ColoredStatusBarActivity;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQRemindersUtils;
import com.intel.wearable.platform.timeiq.refapp.googleAnalytics.GoogleAnalyticsTrackers;

public class ActivityEditReminder extends ColoredStatusBarActivity {

    private static final String TAG = "ActivityEditReminder";

    public static final String REMINDER_KEY_ID = "reminder_key_id";

    private String mReminderId;
    private BaseFragmentEditReminder mReminderFragment;
    private IReminder mReminder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_edit_reminder);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);

        toolbar.setTitle(getString(R.string.reminder_title_edit));

        toolbar.inflateMenu(R.menu.refapp_activity_title_action_bar);

        toolbar.setNavigationIcon(R.drawable.ra_ic_action_cancel);
        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });

        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int id = item.getItemId();
                if (id == R.id.edit_ok) {
                    saveChanges();
                    return true;
                }

                return false;
            }
        });

        getAndSetInput();
        getAndSetReminderAndTrigger();
        mReminderFragment.enableDeleteAndRegister(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (deleteReminder()) {
                    finish();
                }
            }
        });
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

    private void saveChanges() {

        ResultObject<IReminder> newReminderResultObject = mReminderFragment.createReminder();
        String toastMessage;
        if(newReminderResultObject.isSuccess()) {
            IReminder newReminder = newReminderResultObject.getData();
            toastMessage = TimeIQRemindersUtils.editReminder(this, newReminder, mReminderId);
            GoogleAnalyticsTrackers.getInstance().trackEvent(R.string.google_analytics_reminder, R.string.google_analytics_edit, newReminder.getReminderType().name() + " (" + newReminder.getTrigger().getTriggerType().name() + ")");
        } else {
            toastMessage = newReminderResultObject.getMsg();
        }

        if(toastMessage != null && !toastMessage.isEmpty()) {
            Toast.makeText(getApplicationContext(), toastMessage, Toast.LENGTH_LONG).show();
        }
        finish();
    }

    private void getAndSetInput() {
        try {
            Bundle bundle = getIntent().getExtras();
            mReminderId = bundle.getString(REMINDER_KEY_ID);
            if (mReminderId == null) {
                Log.e(TAG, getString(R.string.reminder_wrong_id));
            }
        } catch (Exception e) {
            Log.e(TAG, "failed to get input");
        }
    }

    private void getAndSetReminderAndTrigger() {
        ResultData<IReminder> reminderResultData = TimeIQRemindersUtils.getReminder(mReminderId);
        if (reminderResultData.isSuccess()) {
            mReminder = reminderResultData.getData();
            if (mReminder == null) {
                String text = getString(R.string.reminder_wrong_id);
                Log.e(TAG, text);
                Toast.makeText(getApplicationContext(), text, Toast.LENGTH_LONG).show();
            } else {
                ReminderType reminderType = mReminder.getReminderType();
                switch (reminderType) {
                    case CALL:
                        CallReminder callReminder = (CallReminder) mReminder;
                        mReminderFragment = new FragmentEditCall();
                        mReminderFragment.setContactInfo(callReminder.getContactInfo());
                        break;
                    case NOTIFY:
                        NotificationReminder notificationReminder = (NotificationReminder) mReminder;
                        mReminderFragment = new FragmentEditNotify();
                        mReminderFragment.setContactInfo(notificationReminder.getContactInfo());
                        mReminderFragment.setNotificationMessage(notificationReminder.getNotificationMessage());
                        break;
                    case DO:
                        DoReminder doReminder = (DoReminder) mReminder;
                        mReminderFragment = new FragmentEditDo();
                        ((FragmentEditDo) mReminderFragment).setAction(doReminder.getDoAction());
                        break;
                }

                ITrigger trigger = mReminder.getTrigger();
                TriggerType triggerType = trigger.getTriggerType();
                switch (triggerType) {
                    case CHARGE:
                        mReminderFragment.setTriggerToBattery();
                        break;
                    case MOT:
                        mReminderFragment.setTriggerToNextDrive();
                        break;
                    case PLACE:
                        PlaceTrigger placeTrigger = (PlaceTrigger) trigger;
                        PlaceTriggerType placeTriggerType = placeTrigger.getPlaceTriggerType();
                        switch (placeTriggerType) {
                            case ARRIVE:
                                mReminderFragment.setTriggerToArrivePlace(placeTrigger.getPlaceId());
                                break;
                            case LEAVE:
                                mReminderFragment.setTriggerToLeavePlace(placeTrigger.getPlaceId());
                                break;
                        }
                        break;
                    case TIME:
                        TimeTrigger timeTrigger = (TimeTrigger) trigger;
                        mReminderFragment.setTriggerToReminderTime(timeTrigger.getTriggerTime());
                        break;
                }

                FragmentManager fragmentManager = getSupportFragmentManager();
                FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
                fragmentTransaction.replace(R.id.container, mReminderFragment);
                fragmentTransaction.commit();
            }
        } else {
            String text = reminderResultData.getMessage();
            Log.e(TAG, text);
            Toast.makeText(getApplicationContext(), text, Toast.LENGTH_LONG).show();
        }
    }

    private boolean deleteReminder() {
        String toastMessage;
        boolean deleted = false;
        if (mReminder == null) {
            toastMessage = getString(R.string.reminder_wrong_id);
        } else {
            Result result = TimeIQRemindersUtils.removeReminder(mReminderId);
            if (result.isSuccess()) {
                toastMessage = getString(R.string.reminder_was_deleted);
                deleted = true;
            } else {
                toastMessage = getString(R.string.reminder_was_not_deleted, result.getMessage());
            }
        }

        Toast.makeText(getApplicationContext(), toastMessage, Toast.LENGTH_LONG).show();

        return deleted;
    }

}
