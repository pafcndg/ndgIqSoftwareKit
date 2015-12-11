package com.intel.wearable.platform.timeiq.refapp.reminders.snooze;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.widget.ArrayAdapter;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.common.result.Result;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.reminders.snooze.SnoozeOption;
import com.intel.wearable.platform.timeiq.api.reminders.snooze.SnoozeType;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQRemindersUtils;
import com.intel.wearable.platform.timeiq.refapp.notifications.NotificationHelper;
import com.intel.wearable.platform.timeiq.refapp.utils.TextUtil;

import java.util.Iterator;
import java.util.List;

public class SnoozeOptionsActivity extends FragmentActivity {

    public static final String INPUT_STRING_REMINDER_ID = "REMINDER_ID";
    private String mReminderId;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        AlertDialog.Builder builder =  new  AlertDialog.Builder(this)
                .setTitle(getResources().getString(R.string.snooze_option_title))
                .setNegativeButton(getResources().getString(android.R.string.no), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                dialog.dismiss();
                            }
                        }
                );

        Bundle bundle = getIntent().getExtras();
        mReminderId = bundle.getString(INPUT_STRING_REMINDER_ID);
        if (mReminderId != null) {
            ResultData<List<SnoozeOption>> snoozeOptionsResultData = TimeIQRemindersUtils.getSnoozeOptions(mReminderId);
            if (snoozeOptionsResultData.isSuccess()) {
                final List<SnoozeOption> snoozeOptions = snoozeOptionsResultData.getData();
                if (snoozeOptions.size() > 0) {
                    String snoozeOptionDescription;
                    ArrayAdapter<String> snoozeOptionsArrayAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1);

                    Iterator<SnoozeOption> snoozeIterator = snoozeOptions.iterator();
                    while (snoozeIterator.hasNext()) {
                        SnoozeOption snoozeOption = snoozeIterator.next();
                        SnoozeType snoozeType = snoozeOption.getType();
                        if(snoozeType.equals(SnoozeType.DEFINE_HOME) || snoozeType.equals(SnoozeType.DEFINE_WORK)) {
                            snoozeIterator.remove();
                        } else {
                            snoozeOptionDescription = TextUtil.getSnoozeOptionDescription(getApplicationContext(), snoozeOption);
                            if (snoozeOptionDescription != null) {
                                snoozeOptionsArrayAdapter.add(snoozeOptionDescription);
                            } else { // there was an error
                                snoozeIterator.remove();
                            }
                        }
                    }

                    builder.setAdapter(snoozeOptionsArrayAdapter, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int position) {
                            onSnoozeOptionSelected(snoozeOptions.get(position));
                        }
                    });
                } else {
                    Toast.makeText(getApplicationContext(), R.string.snooze_option_error_no_snooze_options_to_choose_from, Toast.LENGTH_LONG).show();
                }
            } else {
                Toast.makeText(getApplicationContext(), snoozeOptionsResultData.getMessage(), Toast.LENGTH_LONG).show();
            }
        } else {
            Toast.makeText(getApplicationContext(), R.string.snooze_option_error_reminder_id_null, Toast.LENGTH_LONG).show();
        }

        builder.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface dialogInterface) {
                finish();
            }
        });

        builder.create().show();
    }

    private void onSnoozeOptionSelected(SnoozeOption snoozeOption) {
        Result result = TimeIQRemindersUtils.snoozeReminder(mReminderId, snoozeOption);
        if (result.isSuccess()) {
            Toast.makeText(getApplicationContext(), R.string.snooze_option_snoozed, Toast.LENGTH_LONG).show();
            NotificationHelper.cancelNotification(getApplicationContext(), mReminderId); // cancel the notification
        } else {
            Toast.makeText(getApplicationContext(), result.getMessage(), Toast.LENGTH_LONG).show();
        }

        finish();
    }

}
