package com.intel.wearable.platform.timeiq.refapp.notifications;

import android.app.PendingIntent;

import com.intel.wearable.platform.timeiq.api.events.TSOEventType;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderType;
import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.Vector;

/**
 * Created by adura on 20/10/2015.
 */


/**
 * NotificationData Class hold tha needed data to create and display application notification
 */
public class NotificationData {

    public enum NOTIFICATION_TYPE{
        TYPE_ID_DO,
        TYPE_ID_CALL,
        TYPE_ID_NOTIFY,
        TYPE_ID_BE
    }


    final public String mID;                  // Notification ID
    final public String mTitle;               // Notification title
    final public String mSubText;             // 2nd line

    private final NOTIFICATION_TYPE mTypeId;          // set different group for different kind of reminders
    private Vector<NotificationAction> mActions;// A Collection of action, might be empty
    private final int mSmallIcon;                     // The Notification icon
    private final PendingIntent mResultPendingIntent; // the intent to execute on notification click. might be null


    /**
     * Construct a new NotificationData Object.
     *
     * @param notificationId A unique notification id. As we want a notification per reminder we use the reminder ID
     * @param title     The title (first row) of the notification
     * @param subText   The text (second row) of the notification
     * @param typeId    The notification will be set its icon by the {@link NOTIFICATION_TYPE}
     */
    public NotificationData(String notificationId, String title, String subText, NOTIFICATION_TYPE typeId)
    {
        mID         = notificationId;
        mTitle      = title;
        mSubText    = subText;
        mTypeId     = typeId;
        mSmallIcon  = getIconByType();

        mActions    = null;
        mResultPendingIntent = null;
    }

    /**
     * @return Drawable Icon set using the {@link NOTIFICATION_TYPE } mTypeId
     */
    private int getIconByType()
    {
        int iconByType ;
        switch (mTypeId){
            case TYPE_ID_DO:
                iconByType = R.drawable.notification_do;
                break;
            case TYPE_ID_CALL:
                iconByType = R.drawable.notification_call;
                break;
            case TYPE_ID_NOTIFY:
                iconByType = R.drawable.notification_notif;
                break;
            case TYPE_ID_BE:
                iconByType = R.drawable.notification_be;
                break;
            default:
                iconByType = R.drawable.ra_ic_action_ok;
        }

        return iconByType;
    }


    /**
     * Add a new action to the notification
     *
     * @param action {@link NotificationAction} data
     */
    public void addAction(NotificationAction action){
        if (action == null)
            return;

        if(mActions == null){
            mActions = new Vector< >();
        }

        mActions.add(action);
    }


    /**
     *
     * @return A collection of all notification action.
     */
    public Vector<NotificationAction> getNotificationActions(){
        return mActions;
    }


    /**
     *
     * @return The notification icon
     */
    public int getSmallIcon() {
        return mSmallIcon;
    }


    public PendingIntent getResultPendingIntent() {
        return mResultPendingIntent;
    }


    /**
     * @param eventType the event type to convert
     * @return A notification {@link NOTIFICATION_TYPE} type using the given {@link TSOEventType } type
     */
    public static NOTIFICATION_TYPE getTypeByEventType(TSOEventType eventType) {
        NOTIFICATION_TYPE type = null;
        switch (eventType)
        {
            case BE:
                type = NOTIFICATION_TYPE.TYPE_ID_BE;
                break;
            case CALENDAR:
                type = NOTIFICATION_TYPE.TYPE_ID_NOTIFY;
                break;
        }

        return type;
    }

    /**
     *
     * @param reminderType The reminder type to convert
     * @return A notification {@link NOTIFICATION_TYPE} type using the given {@link ReminderType } type
     */
    public static NOTIFICATION_TYPE getTypeByReminderType(ReminderType reminderType) {
        NOTIFICATION_TYPE type = null;
        switch (reminderType)
        {
            case CALL:
                type = NOTIFICATION_TYPE.TYPE_ID_CALL;
                break;
            case DO:
                type = NOTIFICATION_TYPE.TYPE_ID_DO;
                break;
            case NOTIFY:
                type = NOTIFICATION_TYPE.TYPE_ID_NOTIFY;
        }
        return type;
    }


}
