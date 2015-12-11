package com.intel.wearable.platform.timeiq.refapp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.result.Result;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.events.IEvent;
import com.intel.wearable.platform.timeiq.api.events.UserActionType;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderEndReason;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.callReminder.CallReminder;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQEventsUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQRemindersUtils;
import com.intel.wearable.platform.timeiq.refapp.notifications.NotificationHelper;

public class NotificationsBroadcastReceiver extends BroadcastReceiver {

    public static final String INPUT_STRING_ID = "INPUT_STRING_ID";
    public static final String INPUT_STRING_NOTIFY_REMINDER_PHONE_NUMBER = "INPUT_STRING_NOTIFY_REMINDER_PHONE_NUMBER";
    public static final String INPUT_STRING_NOTIFY_REMINDER_MESSAGE = "INPUT_STRING_NOTIFY_REMINDER_MESSAGE";
    public static final String ACTION_EVENT_DISMISS_ACTION = "NotificationsBroadcastReceiver.ACTION_EVENT_DISMISS_ACTION";
    public static final String ACTION_EVENT_GO_NOW_ACTION = "NotificationsBroadcastReceiver.ACTION_EVENT_GO_NOW_ACTION";
    public static final String ACTION_EVENT_TIME_SNOOZE_ACTION = "NotificationsBroadcastReceiver.ACTION_EVENT_TIME_SNOOZE_ACTION";
    public static final String ACTION_NOTIFY_REMINDER_DONE_ACTION = "NotificationsBroadcastReceiver.ACTION_NOTIFY_REMINDER_DONE_ACTION";
    public static final String ACTION_CALL_REMINDER_DONE_ACTION = "NotificationsBroadcastReceiver.ACTION_CALL_REMINDER_DONE_ACTION";
    public static final String ACTION_DO_REMINDER_DONE_ACTION = "NotificationsBroadcastReceiver.ACTION_DO_REMINDER_DONE_ACTION";
    public static final String ACTION_REMINDER_DISMISSED_ACTION = "NotificationsBroadcastReceiver.ACTION_REMINDER_DISMISSED_ACTION";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent != null) {
            String id = intent.getStringExtra(INPUT_STRING_ID);
            if (id != null) {
                String action = intent.getAction();
                if (action != null) {
                    switch (action) {
                        case ACTION_REMINDER_DISMISSED_ACTION:
                            endReminderAndCancelNotification(context, id, ReminderEndReason.Dismiss);
                            break;
                        case ACTION_DO_REMINDER_DONE_ACTION:
                            endReminderAndCancelNotification(context, id, ReminderEndReason.Done);
                            closeStatusBar(context);
                            break;
                        case ACTION_CALL_REMINDER_DONE_ACTION:
                            ResultData<IReminder> reminderResultData = TimeIQBGService.mTimeIQApi.getRemindersManager().getReminder(id);
                            if (reminderResultData.isSuccess()) {
                                IReminder reminder = reminderResultData.getData();
                                if (reminder != null) {
                                    startCall(context, (CallReminder) reminder);
                                } else {
                                    Toast.makeText(context, context.getString(R.string.failed_to_get_reminder_wrong_id, id), Toast.LENGTH_LONG).show();
                                }
                            } else {
                                Toast.makeText(context, context.getString(R.string.failed_to_get_reminder, reminderResultData.getMessage()), Toast.LENGTH_LONG).show();
                            }

                            endReminderAndCancelNotification(context, id, ReminderEndReason.Done);
                            closeStatusBar(context);
                            break;
                        case ACTION_NOTIFY_REMINDER_DONE_ACTION:
                            // already ended the notify reminder when the SMS was sent
                            String phoneNumber = intent.getStringExtra(INPUT_STRING_NOTIFY_REMINDER_PHONE_NUMBER);
                            String message = intent.getStringExtra(INPUT_STRING_NOTIFY_REMINDER_MESSAGE);
                            NotificationHelper.cancelNotification(context, id);
                            openFeed(context, phoneNumber, message);
                            closeStatusBar(context);
                            break;
                        case ACTION_EVENT_GO_NOW_ACTION:
                            ResultData<IEvent> result = TimeIQBGService.mTimeIQApi.getEventsEngine().getEvent(id);
                            if (result.isSuccess()) {
                                IEvent event = result.getData();
                                TSOCoordinate coordinate = event.getLocation().getCoordinate();
                                openGoogleMapsNavigation(context, coordinate.getLatitude(), coordinate.getLongitude());
                                if (TimeIQEventsUtils.chooseUserAction(event.getId(), UserActionType.GO_NOW)) {
                                    Toast.makeText(context, context.getString(R.string.event_marked_as, UserActionType.GO_NOW.name()), Toast.LENGTH_LONG).show();
                                } else {
                                    Toast.makeText(context, context.getString(R.string.failed_to_choose_event_user_action, UserActionType.GO_NOW.name()), Toast.LENGTH_LONG).show();
                                }
                            } else {
                                Toast.makeText(context, context.getString(R.string.failed_to_get_event, result.getMessage()), Toast.LENGTH_LONG).show();
                            }

                            NotificationHelper.cancelNotification(context, id);
                            closeStatusBar(context);
                            break;
                        case ACTION_EVENT_DISMISS_ACTION:
                            if (TimeIQEventsUtils.chooseUserAction(id, UserActionType.DISMISS)) {
                                Toast.makeText(context, context.getString(R.string.event_dismissed), Toast.LENGTH_LONG).show();
                            } else {
                                Toast.makeText(context, context.getString(R.string.failed_to_choose_event_user_action, UserActionType.DISMISS.name()), Toast.LENGTH_LONG).show();
                            }
                            break;
                        case ACTION_EVENT_TIME_SNOOZE_ACTION:
                            if (TimeIQEventsUtils.snoozeEventForFiveMinutes(id)) {
                                Toast.makeText(context, context.getString(R.string.event_snoozed), Toast.LENGTH_LONG).show();
                            } else {
                                Toast.makeText(context, context.getString(R.string.failed_to_snooze_event), Toast.LENGTH_LONG).show();
                            }

                            NotificationHelper.cancelNotification(context, id);
                            closeStatusBar(context);
                            break;
                        default:
                            Toast.makeText(context, R.string.notification_error_unknown_action, Toast.LENGTH_LONG).show();
                            break;
                    }
                } else {
                    Toast.makeText(context, R.string.notification_error_action_is_null, Toast.LENGTH_LONG).show();
                }
            } else {
                Toast.makeText(context, R.string.notification_error_notification_id_is_null, Toast.LENGTH_LONG).show();
            }
        } else {
            Toast.makeText(context, R.string.notification_error_intent_is_null, Toast.LENGTH_LONG).show();
        }
    }

    private void closeStatusBar(Context context) {
        Intent closeStatusBarIntent = new Intent(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
        context.sendBroadcast(closeStatusBarIntent);
    }

    private void endReminderAndCancelNotification(Context context, String id, ReminderEndReason reminderEndReason) {
        Result result = TimeIQRemindersUtils.endReminder(id, reminderEndReason);
        if (result.isSuccess()) {
            Toast.makeText(context, context.getString(R.string.end_reminder_success, reminderEndReason.name()), Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(context, context.getString(R.string.end_reminder_failed, result.getMessage()), Toast.LENGTH_LONG).show();
        }

        NotificationHelper.cancelNotification(context, id);
    }

    private void startCall(Context context, CallReminder callReminder) {
        Intent callIntent = new Intent(Intent.ACTION_DIAL);
        callIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        callIntent.setData(Uri.parse("tel:" + TimeIQRemindersUtils.getPhoneNumber(callReminder)));
        context.startActivity(callIntent);
    }

    private void openFeed(Context context, String phoneNumber, String message) {
        Intent notificationIntent = new Intent(Intent.ACTION_SENDTO);
        notificationIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        notificationIntent.setData(Uri.parse("smsto:" + phoneNumber));
        notificationIntent.putExtra("sms_body", message);
        context.startActivity(notificationIntent);
    }

    private void openGoogleMapsNavigation(Context context, double latitude, double longitude) {
        String uriGoogle = "google.navigation:q=" + latitude + "," + longitude;
        Intent navigationIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(uriGoogle));
        navigationIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(navigationIntent);
    }

}
