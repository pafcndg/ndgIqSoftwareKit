//
//  TextFormatterUtil.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 03/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import TimeIQ

class TextFormatterUtil {
    
    class func getReminderNotifyText(reminder: IReminder) -> String {
        var reminderText: String?
        
        switch (reminder.getReminderType()){
            
        case ReminderType.CALL():
            let callReminder = reminder as! CallReminder;
            reminderText = String(format:NSLocalizedString("reminder_call", comment: ""), callReminder.getContactInfo().getName());
            break;
        case ReminderType.NOTIFY():
            let notificationReminder = reminder as! NotificationReminder;
            reminderText = String(format:NSLocalizedString("reminder_notification", comment: ""), notificationReminder.getContactInfo().getName(), notificationReminder.getNotificationMessage());
            break;
        case ReminderType.DO():
            let doReminder = reminder as! DoReminder;
            reminderText = String(format:NSLocalizedString("reminder_do", comment: ""), doReminder.getDoAction());
            break;
        default:
            print("unsupported reminder type");
            reminderText = "";
        }
        return reminderText!;
    }
    
    class func getReminderNotifySubText(reminder: IReminder) -> String {
        var reminderSubText: String?
        
        let trigger = reminder.getTrigger();
        let triggerType = trigger.getTriggerType();
        switch (triggerType) {
        case TriggerType.CHARGE():
            // Only one type of charge trigger is currently available
            reminderSubText = NSLocalizedString("reminder_subtext_battery_above", comment:"");
            break;
        case TriggerType.MOT():
            // Only one type of MOT trigger is currently available
            reminderSubText = NSLocalizedString("reminder_subtext_driving", comment:"");
            break;
        case TriggerType.PLACE():
            let placeTrigger = trigger as! PlaceTrigger;
            let place = TimeIQPlacesUtil.getPlace(placeTrigger.getPlaceId());
            let placeTriggerType = placeTrigger.getPlaceTriggerType();
            switch (placeTriggerType) {
            case PlaceTriggerType.ARRIVE():
                reminderSubText = String(format: NSLocalizedString("reminder_subtext_arrived", comment: ""), place.getName());
                break;
            case PlaceTriggerType.LEAVE():
                reminderSubText = String(format: NSLocalizedString("reminder_subtext_left", comment: ""), place.getName());
                break;
            default:
                print("unsupported place trigger type: " + placeTriggerType.name());
                reminderSubText = "";
                break;
            }
            break;
        case TriggerType.TIME():
            let timeTrigger = trigger as! TimeTrigger;
            let triggerTime = timeTrigger.getTriggerTime();
            reminderSubText = String(format:NSLocalizedString("reminder_subtext_time", comment: ""), TextFormatterUtil.getHourAsString(triggerTime));
            break;
        default:
            print("unsupported trigger type: " + triggerType.name());
            reminderSubText = "";
        }
        return reminderSubText!;
    }
    
    class func getTriggerText(trigger: ITrigger) -> String {
        var triggerText: String?
        
        switch (trigger.getTriggerType()) {
            
        case TriggerType.CHARGE():
            triggerText = String(format:NSLocalizedString("TRIGGER_TYPE_BATTERY", comment: ""), "15%");
            break;
        case TriggerType.MOT():
            triggerText = NSLocalizedString("TRIGGER_TYPE_NEXT_DRIVE", comment: "");
            break;
        case TriggerType.PLACE():
            let placeTrigger = trigger as! PlaceTrigger;
            triggerText?.appendContentsOf(String(format:NSLocalizedString("place_trigger", comment: ""), placeTrigger.getPlaceTriggerType().name().lowercaseString, placeTrigger.getPlaceId().getSemanticKey().getIdentifier()));
            let placeId = placeTrigger.getPlaceId();
            let semanticKey = placeId.getSemanticKey();
            let identifier = semanticKey.getIdentifier();
            if(placeTrigger.getPlaceTriggerType() == PlaceTriggerType.ARRIVE())
            {
                triggerText = String(format:NSLocalizedString("TRIGGER_TYPE_ARRIVE_TO", comment: ""), identifier);
            }
            else
            {
                triggerText = String(format:NSLocalizedString("TRIGGER_TYPE_LEAVE_FROM", comment: ""), identifier);
            }
            break;
        case TriggerType.TIME():
            let timeTrigger = trigger as! TimeTrigger;
            triggerText = getReminderDateString(timeTrigger.getTriggerTime());
            break;
        default:
            print("unsupported trigger type: " + trigger.getTriggerType().name());
            triggerText = "";
            break;
        }
        return triggerText!;
    }
    
    class func getReminderText(reminder : IReminder) -> String {
        var reminderText = "";
        
        switch (reminder.getReminderType())
        {
        case ReminderType.CALL():
            let callReminder = reminder as! CallReminder;
            reminderText = String(format: NSLocalizedString("reminder_call", comment: ""), callReminder.getContactInfo().getName()) ;
            break;
        case ReminderType.DO():
            let doReminder = reminder as! DoReminder;
            reminderText = String(format: NSLocalizedString("reminder_do", comment: ""), doReminder.getDoAction()) ;
            break;
        case ReminderType.NOTIFY():
            let notifyReminder = reminder as! NotificationReminder;
            reminderText = String(format: NSLocalizedString("reminder_notify", comment: ""), notifyReminder.getContactInfo().getName()) ;
            break;
        default:
            break;
        }
        
        return reminderText;
    }
    
    class func getReminderDateString(time : long64) -> String
    {
        let currDate = NSDate(long: time);
        let reminderAtDateFormatter = NSDateFormatter();
        reminderAtDateFormatter.dateFormat = NSLocalizedString("HH:mm dd/MM/yy", comment: "");
        
        return reminderAtDateFormatter.stringFromDate(currDate);
    }
    
    class func getHourAsString(hourInMillis : long64) -> String
    {
        let hours = NSDate(long: hourInMillis);
        let hourDateFormatter = NSDateFormatter();
        hourDateFormatter.dateFormat = NSLocalizedString("h:mma", comment: "");
        
        return hourDateFormatter.stringFromDate(hours);
    }
    
    class func getTimeTillAsString(timeToInMillis : long64) -> String
    {
        let result:String
        let minutes : CLongLong
        
        let hours = TimeUnit.MILLISECONDS().toHoursWithLong(timeToInMillis);
        
        if (hours == 0) {
            minutes = TimeUnit.MILLISECONDS().toMinutesWithLong(timeToInMillis);
            result = String(format: NSLocalizedString("ttl_min", comment: ""), minutes);
        } else {
            minutes = TimeUnit.MILLISECONDS().toMinutesWithLong(timeToInMillis - TimeUnit.HOURS().toMillisWithLong(hours));
            result = String(format:NSLocalizedString("ttl_hour_and_min", comment: ""), hours, minutes);
        }
        
        return result;
    }
    
    class func getSnoozeOptionDescription(snoozeOption: SnoozeOption) -> String {
        var desc = ""
        switch (snoozeOption.getType()) {
        case SnoozeType.WHEN_CHARGING():
            desc = NSLocalizedString("snooze_option_when_charging", comment: "")
            break;
        case SnoozeType.FROM_CAR():
            desc = NSLocalizedString("snooze_option_from_the_car", comment: "")
            break;
        case SnoozeType.NEXT_DRIVE():
            desc = NSLocalizedString("snooze_option_next_drive", comment: "")
            break;
        case SnoozeType.FROM_PLACE():
            let placeSnoozeOption = snoozeOption as! PlaceSnoozeOption;
            desc = String(format: NSLocalizedString("snooze_option_from_place", comment: ""), placeSnoozeOption.getPlaceId()) //TODO: get place form places engine
            break;
        case SnoozeType.DEFINE_HOME():
            desc = NSLocalizedString("snooze_option_define_home", comment: "")
            break;
        case SnoozeType.DEFINE_WORK():
            desc = NSLocalizedString("snooze_option_define_work", comment: "")
            break;
        case SnoozeType.NEXT_TIME_AT_CURRENT_PLACE():
            let placeSnoozeOption = snoozeOption as! PlaceSnoozeOption;
            desc = String(format: NSLocalizedString("snooze_option_next_time_at_current_place", comment: ""), placeSnoozeOption.getPlaceId()) //TODO: get place form places engine
            break;
        case SnoozeType.LEAVE_CURRENT_PLACE():
            let placeSnoozeOption = snoozeOption as! PlaceSnoozeOption;
            desc = String(format: NSLocalizedString("snooze_option_leave_current_place", comment: ""), placeSnoozeOption.getPlaceId()) //TODO: get place form places engine
            break;
        case SnoozeType.IN_X_MIN():
            let timeDelaySnoozeOption = snoozeOption as! TimeDelaySnoozeOption;
            desc = String(format: NSLocalizedString("snooze_option_in_x_min", comment: ""), timeDelaySnoozeOption.getDelayMinutes())
            break;
        case SnoozeType.TIME_RANGE():
            let timeRangeSnoozeOption = snoozeOption as! TimeRangeSnoozeOption;
            switch (timeRangeSnoozeOption.getTimeRange()) {
            case SnoozeTimeRange.THIS_MORNING():
                desc = NSLocalizedString("snooze_option_time_range_this_morning", comment: "")
                break;
            case SnoozeTimeRange.TODAY():
                desc = NSLocalizedString("snooze_option_time_range_later_today", comment: "")
                break;
            case SnoozeTimeRange.THIS_EVENING():
                desc = NSLocalizedString("snooze_option_time_range_in_the_evening", comment: "")
                break;
            case SnoozeTimeRange.THIS_NIGHT():
                desc = NSLocalizedString("snooze_option_time_range_later_tonight", comment: "")
                break;
            case SnoozeTimeRange.TOMORROW_MORNING():
                desc = NSLocalizedString("snooze_option_time_range_tomorrow_morning", comment: "")
                break;
            default:
                print("Missing time range description for %@",timeRangeSnoozeOption.getTimeRange());
            }
            break;
        default:
            print("Missing description for %@",snoozeOption.getType());
        }
        
        return desc;
    }
    
    /**
     *
     * @param context Context - the Context to get resources from
     * @param motType Input MotType
     * @return Mot Type as string
     */
    class func getMotString(motType: MotType) -> String {
        var motString = "";
        switch (motType) {
        case MotType.CAR():
            motString = NSLocalizedString("ttl_mot_type_string_car", comment: "")
            break;
        case MotType.WALK():
            motString = NSLocalizedString("ttl_mot_type_string_walk", comment: "")
            break;
        case MotType.PUBLIC_TRANSPORT():  // Currently no supported
            break;
        case MotType.STATIONARY():        //  Cannot occur
            break;
        default:
            break;
        }
        
        return motString;
    }
    
    class func getRouteString(data: IRouteData, lateType: LateType, etaOrTtlString: String) -> String? {
        var routeString: String? = ""
        let motString = TextFormatterUtil.getMotString(data.getMainMotType());
        let routeDataType = data.getRouteDataType();
        let routeDuration = data.getRouteDuration();
        let routeDurationAsString = TextFormatterUtil.getTimeTillAsString(routeDuration);

        switch (routeDataType.ordinal())
        {
        case .RouteDataType_AT_DESTINATION:
            routeString = NSLocalizedString("route_at_destination", comment: "");
            break;
        case .RouteDataType_AT_DESTINATION_WHILE_DRIVING:
            routeString = NSLocalizedString("route_at_destination", comment: "");
            break;
        case .RouteDataType_NEAR_DESTINATION, .RouteDataType_DESTINATION_CLOSE_BY, .RouteDataType_DESTINATION_CLOSE_BY_WHILE_DRIVING:
            routeString = String(format: NSLocalizedString("route_near_destination", comment: ""), routeDurationAsString, motString);
            break;
        case .RouteDataType_WALK, .RouteDataType_DRIVE:
            switch (lateType){
            case .NOT_LATE:
                routeString = String(format: NSLocalizedString("route_ok_not_late", comment: ""), etaOrTtlString, routeDurationAsString, motString);
                break;
            case .LATE, .OVERDUE:
                routeString = String(format: NSLocalizedString("route_ok_late", comment: ""), etaOrTtlString, routeDurationAsString, motString);
                break;
            }
            break;
        case .RouteDataType_DRIVE_WHILE_DRIVING:
            switch (lateType){
            case .NOT_LATE:
                routeString = String(format: NSLocalizedString("string.route_ok_while_driving_not_late", comment: ""), routeDurationAsString, motString);
                break;
            case .LATE, .OVERDUE:
                routeString = String(format: NSLocalizedString("route_ok_while_driving_late", comment: ""), etaOrTtlString, routeDurationAsString, motString);
                break;
            }
            break;
        case .RouteDataType_TOO_FAR_FOR_WALKING:
            routeString = NSLocalizedString("route_too_far_for_walking", comment: "");
            break;
        case .RouteDataType_TOO_FAR_FOR_DRIVING:
            routeString = NSLocalizedString("route_too_far_for_driving", comment: "");
            break;
        }
        return routeString;
    }
}