//
//  TimeIQBGService.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 04/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import TSO
import MessageUI
import EventKit

let NOTIFICATION_LOGIN_SUCCESS = "NOTIFICATION_LOGIN_SUCCESS"
let NOTIFICATION_RELOAD_REMINDERS = "NOTIFICATION_RELOAD_REMINDERS"

class TimeIQBGService : NSObject, IMessageListener, MFMessageComposeViewControllerDelegate, CLLocationManagerDelegate {
    
    static let sharedInstance = TimeIQBGService()
    let authCredentialsProvider = SimpleAuthCredentialsProvider.sharedInstance;
    var timeIQApi: TimeIQApi?
    var isLoggedIn = false;
    var locationManager : CLLocationManager!
    
    private override init()
    {
        super.init()
        
        if authCredentialsProvider.isUserLoggedIn() {
            self.loadData()
        }
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "doneAction:", name: "doneActionNotification", object: nil);
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "callAction:", name: "callActionNotification", object: nil);
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "notifyAction:", name: "notifyActionNotification", object: nil);
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "snoozeAction:", name: "snoozeActionNotification", object: nil);
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "beSnoozeAction:", name: "beSnoozeActionNotification", object: nil);
    }
    
    func doneAction(notification: NSNotification) {
        NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
    }
    
    func callAction(notification: NSNotification) {
        NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
        let reminderId = notification.userInfo!["reminder_id"] as! String
        let callReminder = TimeIQRemindersUtils.getReminder(reminderId).getData() as! CallReminder
        let phoneNumberObj = callReminder.getContactInfo().getPhoneNumbers().objectAtIndex(0) as! PhoneNumber
        let phoneNumberDirty = phoneNumberObj.getCleanPhoneNumber() as String
        var phoneNumber = ""
        for currChar in phoneNumberDirty.characters {
            switch (currChar) {
            case "0","1","2","3","4","5","6","7","8","9","+","-" :
                phoneNumber = phoneNumber + String(currChar)
                break
            default:
                break
            }
        }
        UIApplication.sharedApplication().openURL(NSURL.init(string: String(format: "tel://%@", phoneNumber))!)
    }
    
    func smsAction(notification: NSNotification) {
        NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
        let reminderId = notification.userInfo!["reminder_id"] as! String
        let notificationReminder = TimeIQRemindersUtils.getReminder(reminderId).getData() as! NotificationReminder
        let phoneNumber = TimeIQRemindersUtils.getPhoneNumber(notificationReminder)
        let message = notificationReminder.getNotificationMessage()
        if (MFMessageComposeViewController.canSendText())
        {
            let messageComposeViewController = MFMessageComposeViewController()
            messageComposeViewController.recipients = [phoneNumber]
            messageComposeViewController.body = message
            messageComposeViewController.messageComposeDelegate = self
            UIApplication.sharedApplication().keyWindow?.rootViewController?.presentViewController(messageComposeViewController, animated: true, completion: nil)
        }
    }
    
    func snoozeAction(notification: NSNotification) {
        let reminderId = notification.userInfo!["reminder_id"] as! String
        showSnoozeOptions(reminderId);
    }
    
    func beSnoozeAction(notification: NSNotification) {
        let eventId = notification.userInfo!["event_id"] as! String
        
        self.showEventSnoozeOptions(eventId)
    }
    
    //MARK: - User Authentication related
    
    func login(username: String, password: String, onSuccess: (() -> Void)!, onFailure: ((CDKError!) -> Void)!)
    {
        authCredentialsProvider.login(username, password: password,
            onSuccess: {
                
                self.loadData()

                onSuccess()
            },
            onFailure: { (error: CDKError!) in
                onFailure(error)
        })
    }
    
    func logout() {
        authCredentialsProvider.logout()
    }
    
    func isUserLoggedIn() -> Bool {
        return authCredentialsProvider.isUserLoggedIn()
    }
    
    private func loadData() {
        
        locationManager = CLLocationManager()
        locationManager.delegate = self
        locationManager.requestAlwaysAuthorization()
        locationManager.distanceFilter = kCLDistanceFilterNone
        locationManager.desiredAccuracy = kCLLocationAccuracyNearestTenMeters
        locationManager.startUpdatingLocation()
        
//        if self.timeIQApi == nil {
//        
//            self.timeIQApi = TimeIQApi();
//            
//            self.timeIQApi!.init__WithIAuthCredentialsProvider(self.authCredentialsProvider, withNSString:"https://tsodev-tsosdk.td.wearables.infra-host.com");
//
//            self.timeIQApi?.getMessageHandler().register__WithIMessageListener(self);
//            
//            // REMOVE WHEN FIX INSIDE SDK!!!
//            let messageEngine = ClassFactory.getInstance().resolveInternalWithClass(IOSClass.classWithProtocol(IExternalMessageEngine)) as! ExternalEngine;
//            messageEngine.init__();
//        }
//
//        dispatch_after(0, dispatch_get_main_queue()) {
//            NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_LOGIN_SUCCESS, object: nil)
//        }
    }
    
    func createAccount(email: String, password:String, firstName:String, lastName:String, onSuccess: (() -> Void)!, onFailure: ((CDKError!) -> Void)!)
    {
        authCredentialsProvider.createAccount(email, password: password, firstName: firstName, lastName: lastName, onSuccess: { () -> Void in
            onSuccess();
            }) { (error: CDKError!) -> Void in
                onFailure(error);
        }
    }
    
    //MARK:
    
    func onRemindersTriggered(reminders: Array<IReminder>) {
        
        for reminder in reminders
        {
            let title = TextFormatterUtil.getReminderNotifyText(reminder);
            let subMessage = TextFormatterUtil.getReminderNotifySubText(reminder);
            
            let trigger = reminder.getTrigger();
            
            var totalMessage = title;
            totalMessage += " " + TextFormatterUtil.getTriggerText(trigger);
            
            switch UIApplication.sharedApplication().applicationState
            {
            case .Active:
                let alert = UIAlertController(title: title, message: subMessage, preferredStyle: UIAlertControllerStyle.Alert)
                
                switch (reminder.getReminderType()){
                case ReminderType.DO():
                    alert.addAction(UIAlertAction(title: NSLocalizedString("Done", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                        NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
                    }));
                    alert.addAction(UIAlertAction(title: NSLocalizedString("Snooze", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                        self.showSnoozeOptions(reminder.getId())
                    }));
                    break;
                case ReminderType.CALL():
                    alert.addAction(UIAlertAction(title: NSLocalizedString("Call", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                        NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
                        let callReminder = reminder as! CallReminder;
                        let phoneNumberObj = callReminder.getContactInfo().getPhoneNumbers().objectAtIndex(0) as! PhoneNumber
                        let phoneNumberDirty = phoneNumberObj.getCleanPhoneNumber() as String
                        var phoneNumber = ""
                        for currChar in phoneNumberDirty.characters {
                            switch (currChar) {
                            case "0","1","2","3","4","5","6","7","8","9","+","-" :
                                phoneNumber = phoneNumber + String(currChar)
                                break
                            default:
                                break
                            }
                        }
                        #if (arch(i386) || arch(x86_64)) && os(iOS)
                            print("Can't call on SIMULATOR");
                        #else
                            UIApplication.sharedApplication().openURL(NSURL.init(string: String(format: "tel://%@", phoneNumber))!)
                        #endif
                    }));
                    alert.addAction(UIAlertAction(title: NSLocalizedString("Dismiss", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                        NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
                    }));
                    alert.addAction(UIAlertAction(title: NSLocalizedString("Snooze", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                        NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
                        self.showSnoozeOptions(reminder.getId())
                    }));
                    break;
                case ReminderType.NOTIFY():
                    alert.addAction(UIAlertAction(title: NSLocalizedString("", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                        NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
                        let notificationReminder = reminder as! NotificationReminder;
                        let phoneNumber = TimeIQRemindersUtils.getPhoneNumber(notificationReminder)
                        
                        if (MFMessageComposeViewController.canSendText())
                        {
                            let messageComposeViewController = MFMessageComposeViewController()
                            messageComposeViewController.recipients = [phoneNumber]
                            messageComposeViewController.body = notificationReminder.getNotificationMessage()
                            messageComposeViewController.messageComposeDelegate = self
                            UIApplication.sharedApplication().keyWindow?.rootViewController?.presentViewController(messageComposeViewController, animated: true, completion: nil)
                        }
                    }));
                    alert.addAction(UIAlertAction(title: NSLocalizedString("Cancel", comment: ""), style: UIAlertActionStyle.Cancel, handler: nil));
                    break;
                default:
                    print("unsupported reminder type");
                }
                showUIAlertController(alert)
                
                break;
            case .Inactive, .Background:
                let localNotification = UILocalNotification()
                localNotification.fireDate = NSDate()
                localNotification.timeZone = NSTimeZone.defaultTimeZone()
                localNotification.soundName = "alarmsound.caf"
                switch (reminder.getReminderType()){
                case ReminderType.DO():
                    localNotification.category = "DO_CATEGORY"
                    break;
                case ReminderType.CALL():
                    localNotification.category = "CALL_CATEGORY"
                    localNotification.userInfo = ["reminder_id" : reminder.getId()]
                    break;
                case ReminderType.NOTIFY():
                    localNotification.category = "NOTIFY_CATEGORY"
                    localNotification.userInfo = ["reminder_id" : reminder.getId()]
                    break;
                default:
                    print("unsupported reminder type");
                    break;
                }
                localNotification.alertBody = totalMessage
                UIApplication.sharedApplication().scheduleLocalNotification(localNotification)
                break;
            }
        }
    }
    
    func showSnoozeOptions(reminderId: String) {
        
        let alert = UIAlertController(title: "Alert", message: "Message", preferredStyle: UIAlertControllerStyle.Alert)
        
        let snoozeOptions = TimeIQRemindersUtils.getSnoozeOptions(reminderId)
        let options = snoozeOptions.getData() as! ArrayList
        
        for (var index:Int = 0; index < options.count; index++)
        {
            let snoozeOption = options[index] as! SnoozeOption
            let optionStr = TextFormatterUtil.getSnoozeOptionDescription(snoozeOption)
            
            alert.addAction(UIAlertAction(title: optionStr, style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                let result = TimeIQRemindersUtils.snoozeReminder(reminderId, snoozeOption: snoozeOption)
                if (result.isSuccess()) {
                    print(NSLocalizedString("snooze_option_snoozed", comment: ""))
                } else {
                    print(result.getMessage())
                }
                NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil);
            }));
        }
        
        alert.addAction(UIAlertAction(title: NSLocalizedString("Cancel", comment: ""), style:.Cancel) { (action) -> Void in
            NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_RELOAD_REMINDERS, object: nil)
        })

        showUIAlertController(alert)
        
    }
    
    func showEventSnoozeOptions(eventId: String) {
        let alert = UIAlertController(title: "Snooze", message: "Please enter minutes to postpone", preferredStyle: UIAlertControllerStyle.Alert)
        alert.addTextFieldWithConfigurationHandler {
            (txtMinutes) -> Void in
        }
        alert.addAction(UIAlertAction(title: NSLocalizedString("Done", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
            
            let tField = alert.textFields![0] as UITextField
            let userInput:NSString = tField.text!
            
            let minutes = userInput.longLongValue * 1000 * 60
            TimeIQEventsUtils.snoozeEventWithMinutes(eventId, snoozeTime: minutes);
        }));
        showUIAlertController(alert)
    }
    
    func showUIAlertController(alert:UIAlertController)
    {
        dispatch_async(dispatch_get_main_queue(), {
            UIApplication.sharedApplication().keyWindow?.rootViewController!.presentViewController(alert, animated: true, completion: nil);
        })
    }
    
    func onEventTriggered(eventTriggeredResult: TSOEventTriggeredMsg) {
        let event = eventTriggeredResult.getEvent();
        if(event != nil) {
            var subMessage = "";
            print("onEventTriggered: eventTriggeredResult event is OK");
            let routeData = eventTriggeredResult.getRouteData();
            if (routeData != nil) {
                print("onEventTriggered: eventTriggeredResult routeData is OK");
                
//                let routeDuration = routeData.getRouteDuration();
//                let routeDurationString = TextFormatterUtil.getTimeTillAsString(routeDuration);
                
//                let motType = routeData.getMainMotType();
//                let motTypeString = TextFormatterUtil.getMotString(motType);
                
                let arrivalTime = event.getArrivalTime();
                let arrivalTimeString = TextFormatterUtil.getHourAsString(arrivalTime);
                var lateType:LateType = LateType.LATE
                if routeData.isTtlRoute()
                {
                    lateType = LateType.NOT_LATE
                }
                subMessage = TextFormatterUtil.getRouteString(routeData, lateType: lateType, etaOrTtlString: arrivalTimeString)!
            } else {
                print("onEventTriggered: eventTriggeredResult routeData is null");
            }
            
            switch UIApplication.sharedApplication().applicationState
            {
            case .Active:
                let alert = UIAlertController(title: event.getSubject(), message: subMessage, preferredStyle: UIAlertControllerStyle.Alert)
                alert.addAction(UIAlertAction(title: NSLocalizedString("Done", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                }));
                alert.addAction(UIAlertAction(title: NSLocalizedString("Dismiss", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
                alert.addAction(UIAlertAction(title: NSLocalizedString("Snooze", comment: ""), style: UIAlertActionStyle.Default, handler: { (action) -> Void in
                    self.showEventSnoozeOptions(event.getId())
                }));
                self.showUIAlertController(alert)
                break;
            case .Inactive, .Background:
                let localNotification = UILocalNotification()
                localNotification.fireDate = NSDate()
                localNotification.timeZone = NSTimeZone.defaultTimeZone()
                localNotification.soundName = "alarmsound.caf"
                localNotification.category = "BE_CATEGORY"
                localNotification.userInfo = ["event_id" : event.getId()]
                localNotification.alertBody = subMessage
                UIApplication.sharedApplication().scheduleLocalNotification(localNotification)
                break;
            }
        } else {
            print("onEventTriggered but event is null");
        }
    }
    
    func onEventStarted(evenTriggerResult: TSOEventTriggeredMsg) {
        
    }
    
    func onEventEnded(evenTriggerResult: TSOEventTriggeredMsg) {
        
    }
    
    func onReceiveWithIMessage(message: IMessage!) {
        let messageType = message.getType() as! MessageType;
        
        switch (messageType.ordinal())
        {
        case MessageType.ON_REMINDERS_TRIGGERED().ordinal():
            DeviceConnectionManager().sendNotification()
            let reminderResult = message.getData() as! RemindersResult;
            self.onRemindersTriggered(self.remindersHashToArray(reminderResult.getResult()));
            break;
        case MessageType.ON_EVENT_TRIGGERED().ordinal():
            DeviceConnectionManager().sendNotification()
            self.onEventTriggered(message.getData() as! TSOEventTriggeredMsg);
            break;
        case MessageType.ON_EVENT_START().ordinal():
            self.onEventStarted(message.getData() as! TSOEventTriggeredMsg);
            break;
        case MessageType.ON_EVENT_END().ordinal():
            self.onEventEnded(message.getData() as! TSOEventTriggeredMsg);
            break;
        default:
            break;
        }
    }
    
    func remindersHashToArray(reminders: HashSet) -> Array<IReminder> {
        var list = [IReminder]();
        
        var index: Int
        for index = 0; index < reminders.count; ++index
        {
            list.append(reminders.objectAtIndex(UInt(index)) as! IReminder);
        }
        
        return list;
    }
    
    func messageComposeViewController(controller: MFMessageComposeViewController, didFinishWithResult result: MessageComposeResult) {
        controller.dismissViewControllerAnimated(true, completion: nil)
    }
    
    //MARK: CLLocationManagerDelegate
    
    func locationManager(manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        
        manager.stopUpdatingLocation()
        locationManager = nil
        
        LocationApi.setCurrentLocation(locations.last!)
        
        if self.timeIQApi == nil {
            
            dispatch_async(dispatch_get_main_queue(), { () -> Void in
                
                self.timeIQApi = TimeIQApi();
                
                self.timeIQApi!.init__WithIAuthCredentialsProvider(self.authCredentialsProvider, withNSString:"https://tsodev-tsosdk.td.wearables.infra-host.com");
                
                self.timeIQApi?.getMessageHandler().register__WithIMessageListener(self);
                
                // REMOVE WHEN FIX INSIDE SDK!!!
                let messageEngine = ClassFactory.getInstance().resolveInternalWithClass(IOSClass.classWithProtocol(IExternalMessageEngine)) as! ExternalEngine;
                messageEngine.init__();
            })
        }
        
        dispatch_after(0, dispatch_get_main_queue()) {
            NSNotificationCenter.defaultCenter().postNotificationName(NOTIFICATION_LOGIN_SUCCESS, object: nil)
        }
    }
    
    func locationManager(manager: CLLocationManager, didFailWithError error: NSError) {
        print("error")
    }
    
}

