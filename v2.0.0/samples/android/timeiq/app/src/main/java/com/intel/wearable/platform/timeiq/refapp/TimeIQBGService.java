package com.intel.wearable.platform.timeiq.refapp;

import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.Address;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.telephony.SmsManager;
import android.util.Log;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.TimeIQApi;
import com.intel.wearable.platform.timeiq.api.common.messageHandler.IMessage;
import com.intel.wearable.platform.timeiq.api.common.messageHandler.IMessageListener;
import com.intel.wearable.platform.timeiq.api.common.messageHandler.MessageType;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.events.IEvent;
import com.intel.wearable.platform.timeiq.api.events.TSOEventTriggeredMsg;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderEndReason;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderType;
import com.intel.wearable.platform.timeiq.api.reminders.RemindersResult;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.notificationReminder.NotificationReminder;
import com.intel.wearable.platform.timeiq.api.route.IRouteData;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQInitUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQPlacesUtils;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQRemindersUtils;
import com.intel.wearable.platform.timeiq.refapp.auth.AuthUtil;
import com.intel.wearable.platform.timeiq.refapp.auth.ICredentialsProvider;
import com.intel.wearable.platform.timeiq.refapp.coreiq.CoreIQUtil;
import com.intel.wearable.platform.timeiq.refapp.notifications.NotificationAction;
import com.intel.wearable.platform.timeiq.refapp.notifications.NotificationData;
import com.intel.wearable.platform.timeiq.refapp.notifications.NotificationHelper;
import com.intel.wearable.platform.timeiq.refapp.places.PlaceType;
import com.intel.wearable.platform.timeiq.refapp.reminders.snooze.SnoozeOptionsActivity;
import com.intel.wearable.platform.timeiq.refapp.utils.LocationProviderUtil;
import com.intel.wearable.platform.timeiq.refapp.utils.RgcProviderUtil;
import com.intel.wearable.platform.timeiq.refapp.utils.TextUtil;

import java.util.Collection;


/**
 * Created by smoradof on 10/11/2015.
 */
public class TimeIQBGService extends Service implements IMessageListener, CoreIQUtil.ITappingEventListener {

    private static final String TAG = TimeIQBGService.class.getSimpleName();
    public static final TimeIQApi mTimeIQApi = new TimeIQApi();

    private NotificationsBroadcastReceiver mNotificationsBroadcastReceiver;


    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "onCreate");

        Context context = getApplicationContext();
        ICredentialsProvider authProvider = AuthUtil.getAuthProvider(context);
        authProvider.loadData();
        if ( ! authProvider.isUserLoggedIn() ) {
            //In case running from the background service, make sure we are trying to login before initializing the SDK
            authProvider.login();
        }

        // init the timeIq engine and start it's engines:
        Log.d(TAG, "before SDK init");
        boolean initWasSuccessful = TimeIQInitUtils.initTimeIQ(context, authProvider, AuthUtil.getCloudServerURL());
        Log.d(TAG, "after SDK init (success = " + initWasSuccessful + ")");

        if (initWasSuccessful) {
            CoreIQUtil coreIQUtil = CoreIQUtil.getInstance();
            coreIQUtil.init(context); // init coreIq
            coreIQUtil.setTappingEventListener(this);
            TimeIQInitUtils.initTimeIQMessageListener(this);
            setNotificationsBroadcastReceiver();
        }
    }

    private void setNotificationsBroadcastReceiver() {
        mNotificationsBroadcastReceiver = new NotificationsBroadcastReceiver();
        IntentFilter remindersBroadcastReceiverFilter = new IntentFilter();
        remindersBroadcastReceiverFilter.addAction(NotificationsBroadcastReceiver.ACTION_EVENT_DISMISS_ACTION);
        remindersBroadcastReceiverFilter.addAction(NotificationsBroadcastReceiver.ACTION_EVENT_GO_NOW_ACTION);
        remindersBroadcastReceiverFilter.addAction(NotificationsBroadcastReceiver.ACTION_EVENT_TIME_SNOOZE_ACTION);
        remindersBroadcastReceiverFilter.addAction(NotificationsBroadcastReceiver.ACTION_NOTIFY_REMINDER_DONE_ACTION);
        remindersBroadcastReceiverFilter.addAction(NotificationsBroadcastReceiver.ACTION_CALL_REMINDER_DONE_ACTION);
        remindersBroadcastReceiverFilter.addAction(NotificationsBroadcastReceiver.ACTION_DO_REMINDER_DONE_ACTION);
        remindersBroadcastReceiverFilter.addAction(NotificationsBroadcastReceiver.ACTION_REMINDER_DISMISSED_ACTION);
        registerReceiver(mNotificationsBroadcastReceiver, remindersBroadcastReceiverFilter);
    }


    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand");
        if ( mTimeIQApi.isInitialized()) {

            Log.d(TAG , "Service onStartCommand - init Ok - STICKY");
            return Service.START_STICKY;
        }

        Log.d(TAG, "Service onStartCommand - init failed - NOT STICKY");
        return Service.START_NOT_STICKY;

    }


    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        unregisterReceiver(mNotificationsBroadcastReceiver);
        TimeIQInitUtils.onDestroyService(this);
        super.onDestroy();
    }


    private void onEventTriggered(TSOEventTriggeredMsg eventTriggeredResult) {
        IEvent event = eventTriggeredResult.getEvent();
        if(event != null) {
            String subMessage = "";
            Log.d(TAG, "onEventTriggered: eventTriggeredResult event is OK");
            IRouteData routeData = eventTriggeredResult.getRouteData();
            if (routeData != null) {
                Log.d(TAG, "onEventTriggered: eventTriggeredResult routeData is OK");
                String etaOrTtlString;
                TextUtil.LateType lateType;
                if (routeData.isTtlRoute()) {
                    lateType = TextUtil.LateType.NOT_LATE;
                    long departureTime = routeData.getDepartureTime();
                    etaOrTtlString = TextUtil.getHourAsString(departureTime);
                } else {
                    lateType = TextUtil.LateType.LATE;
                    long arrivalTime = routeData.getArrivalTime();
                    etaOrTtlString = TextUtil.getHourAsString(arrivalTime);
                }

                subMessage = TextUtil.getRouteString(getApplicationContext(), routeData, lateType, etaOrTtlString);
            } else {
                Log.d(TAG, "onEventTriggered: eventTriggeredResult routeData is null");
            }

            NotificationData.NOTIFICATION_TYPE typeID = NotificationData.getTypeByEventType(event.getEventType());
            NotificationData notificationData = new NotificationData(event.getId() ,event.getSubject(), subMessage, typeID);
            notificationData.addAction(getDoneActionIntent(this, event));
        // add a snooze action on the notification
            notificationData.addAction(getSnoozeEventActionIntent(this, event));
        // add a delete action on the notification - will set user action with UserActionType.GO_NOW
            PendingIntent dismissPendingIntent = getSimplePendingIntent(this, NotificationsBroadcastReceiver.ACTION_EVENT_DISMISS_ACTION, event.getId());
            NotificationHelper.setNotification(this, notificationData, dismissPendingIntent);
            sendToDevice();
        } else {
            Log.e(TAG, "onEventTriggered but event is null");
        }
    }


    private void onRemindersTriggered(Collection<IReminder> remindersCollection){
        for (IReminder reminder : remindersCollection) {

            String message = TextUtil.getReminderNotifyText(this, reminder);
            String subMessage = TextUtil.getReminderNotifySubText(this, reminder);

            Log.d(TAG, "onRemindersTriggered: msg="+message+" subMessage="+subMessage);

            NotificationData.NOTIFICATION_TYPE typeID = NotificationData.getTypeByReminderType(reminder.getReminderType());
            NotificationData notificationData = new NotificationData(reminder.getId(), message, subMessage, typeID);
        // add a done action on the notification (confirm done, call, open SMS feed and navigate)
            notificationData.addAction(getDoneActionIntent(this, reminder));
            if (reminder.getReminderType() == ReminderType.NOTIFY) {
            // send text message and end reminder
                sendSms((NotificationReminder) reminder);
                TimeIQRemindersUtils.endReminder(reminder, ReminderEndReason.Done);
            // the notify reminder has already been ended, no need to add dismiss intent
                NotificationHelper.setNotification(this, notificationData);
                sendToDevice();
            } else {
            // add a snooze action on the notification (not for notify because it was already performed)
                notificationData.addAction(getSnoozeActionIntent(this, reminder));
            // add a delete action on the notification - will end a reminder with ReminderEndReason.Dismiss
                PendingIntent dismissPendingIntent = getSimplePendingIntent(this, NotificationsBroadcastReceiver.ACTION_REMINDER_DISMISSED_ACTION, reminder.getId());
                NotificationHelper.setNotification(this, notificationData, dismissPendingIntent);
                sendToDevice();
            }
            Log.d(TAG, "onRemindersTriggered: show notification: "+reminder.getReminderType().name());
        }
    }

    private void sendToDevice(){
        String resultStr = CoreIQUtil.getInstance().sendToDevice(0x00, 0x00, 0xFF, 255, (byte) 100);
        /*if(resultStr != null){
            sendToast(resultStr);
        }*/
    }

    private NotificationAction getDoneActionIntent(Context context, IEvent event) {
        String name = context.getString(R.string.notification_action_1_event_name);
        int icon = R.drawable.notif_nav;
        PendingIntent pendingIntent = getSimplePendingIntent(context, NotificationsBroadcastReceiver.ACTION_EVENT_GO_NOW_ACTION, event.getId());
        return new NotificationAction(name, icon, pendingIntent);
    }

    private NotificationAction getDoneActionIntent(Context context, IReminder reminder) {
        NotificationAction result = null;

        if (reminder != null) {
            String name;
            int icon;
            PendingIntent pendingIntent;
            switch (reminder.getReminderType()) {
                case CALL:
                    name = context.getString(R.string.notification_action_1_reminder_call_name);
                    icon = R.drawable.notif_call;
                    pendingIntent = getSimplePendingIntent(context, NotificationsBroadcastReceiver.ACTION_CALL_REMINDER_DONE_ACTION, reminder.getId());
                    result = new NotificationAction(name, icon, pendingIntent);
                    break;
                case NOTIFY:
                    name = context.getString(R.string.notification_action_1_reminder_notify_name);
                    icon = R.drawable.notif_openfeed;
                    pendingIntent = getNotifyReminderDonePendingIntent(context, (NotificationReminder) reminder);
                    result = new NotificationAction(name, icon, pendingIntent);
                    break;
                case DO:
                    name = context.getString(R.string.notification_action_1_reminder_do_name);
                    icon = R.drawable.notif_done;
                    pendingIntent = getSimplePendingIntent(context, NotificationsBroadcastReceiver.ACTION_DO_REMINDER_DONE_ACTION, reminder.getId());
                    result = new NotificationAction(name, icon, pendingIntent);
                    break;
            }
        }

        return result;
    }

    private void sendSms(NotificationReminder notificationReminder) {
        String phoneNumber = TimeIQRemindersUtils.getPhoneNumber(notificationReminder);
        SmsManager.getDefault().sendTextMessage(phoneNumber, null, notificationReminder.getNotificationMessage(), null, null);
    }

    private PendingIntent getSimplePendingIntent(Context context, String type, String id) {
        Intent intent = new Intent(type);
        intent.putExtra(NotificationsBroadcastReceiver.INPUT_STRING_ID, id);
        return PendingIntent.getBroadcast(context, (id + type).hashCode(), intent, PendingIntent.FLAG_UPDATE_CURRENT);
    }

    private PendingIntent getNotifyReminderDonePendingIntent(Context context, NotificationReminder notificationReminder) {
        String id = notificationReminder.getId();
        Intent doneIntent = new Intent(NotificationsBroadcastReceiver.ACTION_NOTIFY_REMINDER_DONE_ACTION);
        doneIntent.putExtra(NotificationsBroadcastReceiver.INPUT_STRING_ID, id);
        doneIntent.putExtra(NotificationsBroadcastReceiver.INPUT_STRING_NOTIFY_REMINDER_PHONE_NUMBER, TimeIQRemindersUtils.getPhoneNumber(notificationReminder));
        doneIntent.putExtra(NotificationsBroadcastReceiver.INPUT_STRING_NOTIFY_REMINDER_MESSAGE, ""); // only want to open the feed, without a message (since we've already sent it)
        return PendingIntent.getBroadcast(context, (id + NotificationsBroadcastReceiver.ACTION_NOTIFY_REMINDER_DONE_ACTION).hashCode(), doneIntent, PendingIntent.FLAG_UPDATE_CURRENT);
    }

    private NotificationAction getSnoozeActionIntent(Context context, IReminder reminder) {
        String name = context.getString(R.string.notification_action_2_snooze_name);
        int icon = R.drawable.notif_snooze;
        String id = reminder.getId();
        Intent snoozeIntent = new Intent(context, SnoozeOptionsActivity.class);
        snoozeIntent.putExtra(SnoozeOptionsActivity.INPUT_STRING_REMINDER_ID, id);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, (id + SnoozeOptionsActivity.INPUT_STRING_REMINDER_ID).hashCode(), snoozeIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        return new NotificationAction(name, icon, pendingIntent);
    }

    private NotificationAction getSnoozeEventActionIntent(Context context, IEvent event) {
        String name = context.getString(R.string.notification_action_2_snooze_event_name);
        int icon = R.drawable.notif_snooze;
        String id = event.getId();
        String type = NotificationsBroadcastReceiver.ACTION_EVENT_TIME_SNOOZE_ACTION;
        Intent snoozeIntent = new Intent(type);
        snoozeIntent.putExtra(NotificationsBroadcastReceiver.INPUT_STRING_ID, id);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context, (id + type).hashCode(), snoozeIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        return new NotificationAction(name, icon, pendingIntent);
    }

    /**
     * A message received from TimeIQ, the message contains a type, and a data that correspond to that type
     * @param message - The SDK message
     */
    @Override
    public void onReceive(IMessage message) {
        MessageType messageType = (MessageType) message.getType();
        Log.d(TAG, "A message received from TimeIQ with type: " + messageType.name());

        switch (messageType){
            case ON_REMINDERS_TRIGGERED:
                RemindersResult remindersResult = (RemindersResult) message.getData();
                // The data contains a collection of reminders, each of them was triggered:
                Collection<IReminder> remindersCollection = remindersResult.getResult();
                onRemindersTriggered(remindersCollection);
                break;
            case ON_EVENT_TRIGGERED:
                TSOEventTriggeredMsg eventTriggeredResult = (TSOEventTriggeredMsg) message.getData();
                onEventTriggered(eventTriggeredResult);
                break;
            case ON_EVENT_START:
                break;
            case ON_EVENT_END:
                break;
        }
    }

    private void sendToast(final String msg){
        Log.d(TAG, "toast:  " + msg);
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_LONG).show();
            }
        });
    }

    @Override
    public void onDoubleTap() {
        Log.d(TAG, "onDoubleTap");
        boolean placeWasSaved = false;
        Context context = getApplicationContext();
        ResultData<TSOCoordinate> currentLocationResult = LocationProviderUtil.getCurrentLocation(context);
        if(currentLocationResult.isSuccess()){
            Log.d(TAG, "onDoubleTap: got current location");
            TSOCoordinate coordinate = currentLocationResult.getData();
            Address address = RgcProviderUtil.getRGCFromLocation(context, coordinate.getLatitude(), coordinate.getLongitude());
            if (address != null) {
                String addressString = RgcProviderUtil.getAddressString(address);
                String name = context.getString(R.string.current_location_name, TextUtil.getReminderDateString(context, System.currentTimeMillis()));
                placeWasSaved = TimeIQPlacesUtils.savePlace(context, PlaceType.OTHER, addressString, name, coordinate).isSuccess();
                Log.d(TAG, "onDoubleTap: got current address: " + addressString + " name: " + name + " placeWasSaved: " + placeWasSaved);
            }
            else{
                Log.d(TAG, "onDoubleTap: no address for location: "+coordinate.toString());
            }
        }
        else{
            String errorMessage = currentLocationResult.getMessage();
            if(errorMessage != null){
                errorMessage = currentLocationResult.getResultCode().name();
            }
            Log.d(TAG, "onDoubleTap: could not get current location: "+errorMessage);
        }
        if(placeWasSaved){
            CoreIQUtil.getInstance().sendToDevice(0x00, 0xFF, 0x00, 255, (byte) 0);
        }
        else{
            CoreIQUtil.getInstance().sendToDevice( 0xFF, 0x00, 0x00, 255, (byte) 0);
        }
    }

    @Override
    public void onTrippleTap() {
        Log.d(TAG, "onTrippleTap");
    }
}
