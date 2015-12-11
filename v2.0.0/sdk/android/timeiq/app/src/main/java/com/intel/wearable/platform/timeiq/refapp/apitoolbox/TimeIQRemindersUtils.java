package com.intel.wearable.platform.timeiq.refapp.apitoolbox;

import android.content.Context;

import com.intel.wearable.platform.timeiq.api.common.result.Result;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.reminders.IRemindersManager;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderEndReason;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderStatus;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.BasePhoneReminder;
import com.intel.wearable.platform.timeiq.api.reminders.snooze.SnoozeOption;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.TimeIQBGService;

import java.util.Collection;
import java.util.EnumSet;
import java.util.List;

/**
 * Created by smoradof on 10/28/2015.
 */
public final class TimeIQRemindersUtils {

    /**
     *
     * @return a collection of reminders that are active (added to the TimeIQApi, and are not yet triggered)
     */
    public static ResultData<Collection<IReminder>> getAllActiveReminders(){
        IRemindersManager remindersManager = TimeIQBGService.mTimeIQApi.getRemindersManager();
        return remindersManager.getReminders(EnumSet.of(ReminderStatus.Active));
    }


    /**
     * add a reminder
     * @param reminder
     * @return error message or null if success
     */
    public static String addReminder(IReminder reminder){
        String errorMsg = null;
        Result result = TimeIQBGService.mTimeIQApi.getRemindersManager().addReminder(reminder);
        if( ! result.isSuccess() ){
            errorMsg = result.getMessage();
            if(errorMsg == null){
                errorMsg = "";
            }
        }
        return errorMsg;
    }


    /**
     * Remove an existing reminder by its unique ID
     * @param reminderId    the unique ID of the reminder
     * @return a success result if the reminder was successfully removed and error result otherwise
     */
    public static Result removeReminder(String reminderId){
        return TimeIQBGService.mTimeIQApi.getRemindersManager().removeReminder(reminderId);
    }


    /**
     * Get an existing reminder by its unique ID
     * @param reminderId    the unique ID of the reminder
     * @return {@link ResultData}
     */
    public static ResultData<IReminder> getReminder(String reminderId) {
        return TimeIQBGService.mTimeIQApi.getRemindersManager().getReminder(reminderId);
    }


    /**
     * Edit an existing reminder by delete the origin and set a new reminder instead
     * @param ctx The Context to get the return string resources from
     * @param newReminder   The new reminder data
     * @param oldReminderId The original reminder ID
     * @return A Status string
     */
    public static String editReminder(Context ctx, IReminder newReminder, String oldReminderId) {
        String message;
        if (newReminder != null) {
            IRemindersManager remindersManager = TimeIQBGService.mTimeIQApi.getRemindersManager();
            ResultData<IReminder> reminderResultData = remindersManager.getReminder(oldReminderId);
            if (reminderResultData.isSuccess()) {
                IReminder oldReminder = reminderResultData.getData();
                if (remindersManager.removeReminder(oldReminderId).isSuccess()) {
                    Result result = remindersManager.addReminder(newReminder);
                    if (result.isSuccess()) {
                        message = ctx.getString(R.string.reminder_was_edited); // reminder edited successfully
                    } else {
                        result = remindersManager.addReminder(oldReminder);
                        message = result.isSuccess() ?
                                ctx.getString(R.string.reminder_was_not_edited) + result.getMessage() : // reminder was not edited
                                ctx.getString(R.string.reminder_was_not_edited_data_lost) + result.getMessage(); // reminder was not edited (but the old reminder was deleted) with the error:
                    }
                } else {
                    message = ctx.getString(R.string.reminder_was_not_edited);
                }
            } else {
                message = reminderResultData.getMessage();
            }
        } else {
            message = ctx.getString(R.string.could_not_create_reminder);
        }
        return message;
    }


    /**
     * Mark a triggered reminder as ended
     * @param reminder The target reminder
     * @param reminderEndReason The end reason of the reminder
     * @return A result that state whether the ending process was successful
     */
    public static Result endReminder(IReminder reminder, ReminderEndReason reminderEndReason) {
        return endReminder(reminder.getId(), reminderEndReason);
    }


    /**
     * Mark a triggered reminder as ended
     * @param reminderId The target reminder ID
     * @param reminderEndReason The end reason of the reminder
     * @return A result that state whether the ending process was successful
     */
    public static Result endReminder(String reminderId, ReminderEndReason reminderEndReason) {
        return TimeIQBGService.mTimeIQApi.getRemindersManager().endReminder(reminderId, reminderEndReason);
    }



    /**
     * Get snooze options for a specific reminder using its unique ID
     * the snooze options can be used to defer the reminder until a later time (or state)
     * @param reminderId    the unique ID of the reminder
     * @return the snooze options that can be used to defer the reminder
     */
    public static ResultData<List<SnoozeOption>> getSnoozeOptions(String reminderId) {
        return TimeIQBGService.mTimeIQApi.getRemindersManager().getSnoozeOptions(reminderId);
    }


    /**
     * Defer a specific reminder until a later time (or state) using snooze options <BR>
     * <B>Only a triggered ({@link ReminderStatus#Triggered}) reminder can be snoozed</B>
     * @param reminderId      the unique ID of the reminder
     * @param snoozeOption    the snooze options to be used to defer the reminder
     * @return a result that state whether the snoozing process was successful (test with {@link Result#isSuccess()}) or not (error will be supplied in {@link Result#getResultCode()} and {@link Result#getMessage()})
     */
    public static Result snoozeReminder(String reminderId, SnoozeOption snoozeOption) {
        return TimeIQBGService.mTimeIQApi.getRemindersManager().snoozeReminderWithSnoozeOption(reminderId, snoozeOption);
    }


    /**
     * Get the Phone number from a Phone Reminder
     * @param phoneReminder The target Phone Reminder
     * @return Phone number as String
     */
    public static String getPhoneNumber(BasePhoneReminder phoneReminder) {
        return phoneReminder.getContactInfo().getPreferredPhoneNumber().getFullPhoneNumber();
    }
}
