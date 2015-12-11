package com.intel.wearable.platform.timeiq.refapp.notifications;

import android.app.PendingIntent;

/**
 * Created by adura on 20/10/2015.
 */

/**
 * NotificationAction is the Class to hold Notification Action Data.
 * Each action must contain action name, icon and execution intent (Pending Intent)
 */
public class NotificationAction {

    public final String        mActionName;
    public final int           mActionIcon;
    public final PendingIntent mActionPendingIntent;


    /**
     * Construct a new NotificationAction
     * @param name Action name
     * @param icon Resource ID
     * @param pendingIntent Intent to execute on action click
     */
    public NotificationAction(String name, int icon, PendingIntent pendingIntent)
    {
        mActionName = name;
        mActionIcon = icon;
        mActionPendingIntent = pendingIntent;
    }
}
