//
//  TimeIQRemindersUtils.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 01/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import TSO

class TimeIQRemindersUtils {
    
    class func getAllActiveReminders() -> Array<IReminder>
    {
        let reminders = TimeIQBGService.sharedInstance.timeIQApi?.getRemindersManager().getRemindersWithEnumSet([ReminderStatus.Active()]).getData() as! Array<IReminder>;
        
        //        var list = [IReminder]();
        //
        //        var index: Int
        //        for index = 0; index < reminders.count; ++index
        //        {
        //            list.append(reminders[index]) as! IReminder);
        //        }
        
        return reminders;
    }
    
    class func addReminder(reminder: IReminder) -> String {
        let result = TimeIQBGService.sharedInstance.timeIQApi?.getRemindersManager().addReminderWithIReminder(reminder);
        
        if (!result!.isSuccess())
        {
            return (result?.getMessage())!;
        }
        return (result?.getMessage())!;
    }
    
    class func removeReminder(reminderId: String) -> Bool {
        return (TimeIQBGService.sharedInstance.timeIQApi?.getRemindersManager().removeReminderWithNSString(reminderId).isSuccess())!;
    }
    
    class func getReminder(reminderId: String) -> ResultData {
        return (TimeIQBGService.sharedInstance.timeIQApi?.getRemindersManager().getReminderWithNSString(reminderId))!;
    }
    
    class func editReminder(newReminder: IReminder?, oldReminderId: String) -> String {
        var message = ""
        let remindersManager = TimeIQBGService.sharedInstance.timeIQApi!.getRemindersManager();
        if (newReminder != nil) {
            let reminderResultData = remindersManager.getReminderWithNSString(oldReminderId);
            if (reminderResultData.isSuccess()) {
                let oldReminder = reminderResultData.getData() as! IReminder;
                if (remindersManager.removeReminderWithNSString(oldReminderId).isSuccess()) {
                    var result = remindersManager.addReminderWithIReminder(newReminder);
                    if (result.isSuccess()) {
                        message = NSLocalizedString("reminder_was_edited", comment: ""); // reminder edited successfully
                    } else {
                        result = remindersManager.addReminderWithIReminder(oldReminder);
                        message = result.isSuccess() ?
                            NSLocalizedString("reminder_was_not_edited", comment: "") + result.getMessage() : // reminder was not edited
                            NSLocalizedString("reminder_was_not_edited_data_lost", comment: "") + result.getMessage(); // reminder was not edited (but the old reminder was deleted) with the error:
                    }
                } else {
                    message = NSLocalizedString("reminder_was_not_edited", comment: "");
                }
            } else {
                message = reminderResultData.getMessage();
            }
        } else {
            message = NSLocalizedString("could_not_create_reminder", comment: "");
        }
        return message;
    }
    
    /**
    * Mark a triggered reminder as ended
    * @param reminder The target reminder
    * @param reminderEndReason The end reason of the reminder
    * @return A result that state whether the ending process was successful
    */
    class func endReminder(reminder: IReminder, reminderEndReason: ReminderEndReason) -> Result {
        return endReminder(reminder.getId(), reminderEndReason: reminderEndReason);
    }
    
    
    /**
    * Mark a triggered reminder as ended
    * @param reminderId The target reminder ID
    * @param reminderEndReason The end reason of the reminder
    * @return A result that state whether the ending process was successful
    */
    class func endReminder(reminderId: String, reminderEndReason: ReminderEndReason) -> Result {
    return TimeIQBGService.sharedInstance.timeIQApi!.getRemindersManager().endReminderWithNSString(reminderId, withReminderEndReason: reminderEndReason);
    }
    
    
    
    /**
    * Get snooze options for a specific reminder using its unique ID
    * the snooze options can be used to defer the reminder until a later time (or state)
    * @param reminderId    the unique ID of the reminder
    * @return the snooze options that can be used to defer the reminder
    */
    class func getSnoozeOptions(reminderId: String) -> ResultData {
    return TimeIQBGService.sharedInstance.timeIQApi!.getRemindersManager().getSnoozeOptionsWithNSString(reminderId);
    }
    
    
    /**
    * Defer a specific reminder until a later time (or state) using snooze options <BR>
    * <B>Only a triggered ({@link ReminderStatus#Triggered}) reminder can be snoozed</B>
    * @param reminderId      the unique ID of the reminder
    * @param snoozeOption    the snooze options to be used to defer the reminder
    * @return a result that state whether the snoozing process was successful (test with {@link Result#isSuccess()}) or not (error will be supplied in {@link Result#getResultCode()} and {@link Result#getMessage()})
    */
    class func snoozeReminder(reminderId: String, snoozeOption: SnoozeOption) -> Result {
        return TimeIQBGService.sharedInstance.timeIQApi!.getRemindersManager().snoozeReminderWithSnoozeOptionWithNSString(reminderId, withSnoozeOption: snoozeOption);
    }
    
    
    /**
    * Get the Phone number from a Phone Reminder
    * @param phoneReminder The target Phone Reminder
    * @return Phone number as String
    */
    class func getPhoneNumber(phoneReminder: BasePhoneReminder) -> String {
        return phoneReminder.getContactInfo().getPreferredPhoneNumber().getFullPhoneNumber();
    }
}

