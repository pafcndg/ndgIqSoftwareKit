package com.intel.wearable.platform.timeiq.refapp.notifications;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.support.v4.app.NotificationCompat;

import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.Vector;

/**
 * Created by adura on 20/10/2015.
 */


/**
 * Helper class to display notifications
 */
public class NotificationHelper {

    /**
     * Set a new notification
     * @param context the Context from which the notification is build
     * @param notificationData the data to display
     */
    public static void setNotification(Context context, NotificationData notificationData) {
        setNotification(context, notificationData, null);
    }

    /**
     * Set a new notification, using delete intent
     * @param context the Context from which the notification is build
     * @param notificationData the data to display
     * @param deleteIntent A {@link PendingIntent} to send when the notification is cleared by the user
     * directly from the notification panel
     */
    public static void setNotification(Context context, NotificationData notificationData, PendingIntent deleteIntent) {
        NotificationCompat.Builder builder =
                new NotificationCompat.Builder(context)
                        .setSmallIcon(notificationData.getSmallIcon())
                        .setColor(context.getResources().getColor(R.color.colorPrimary))
                        .setContentTitle(notificationData.mTitle)
                        .setContentText(notificationData.mSubText);

        NotificationCompat.BigTextStyle notificationStyle = new NotificationCompat.BigTextStyle();
        notificationStyle.setBigContentTitle(notificationData.mTitle);
        notificationStyle.bigText(notificationData.mSubText);
        builder.setStyle(notificationStyle);

        Vector<NotificationAction> notificationActions = notificationData.getNotificationActions();
        if (notificationActions != null) {
            for (NotificationAction notificationAction: notificationActions) {
                builder.addAction(notificationAction.mActionIcon, notificationAction.mActionName, notificationAction.mActionPendingIntent);
            }
        }

        if (deleteIntent != null) {
            builder.setDeleteIntent(deleteIntent);
        }

        builder.setContentIntent(notificationData.getResultPendingIntent());

        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

        notificationManager.notify(notificationData.mID.hashCode(), builder.build());
    }



    /**
     *
     * @param context A context the Context from which the notification is build
     * @param notificationId the notification unique identifier to cancel
     */
    public static void cancelNotification(Context context, String notificationId) {
        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.cancel(notificationId.hashCode());
    }


}
