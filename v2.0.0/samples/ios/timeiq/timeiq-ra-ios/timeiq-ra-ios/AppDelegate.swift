//
//  AppDelegate.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 01/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TimeIQ
import GoogleMaps
import EventKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {
    
    var window: UIWindow?
    
    func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
        // Override point for customization after application launch.
        #if !DEBUG
            GMSServices.provideAPIKey("AIzaSyDfYADuz7ZOTuZO7I1GKh-Q9Ay3Lkv987k");
        #endif

        customAppearance()
        setupNotificationSettings()
        getOrAskCalendarPermission()
            
        return true
    }
    func applicationDidReceiveMemoryWarning(application: UIApplication) {
        TSOLoggerPlaces.get().addToLogWithLOG_LEVEL(LOG_LEVEL.DEBUG_(), withNSString: "MemoryWarning", withNSString: "applicationDidReceiveMemoryWarning");
    }
    func applicationWillResignActive(application: UIApplication) {
        // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
        // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    }
    
    func applicationDidEnterBackground(application: UIApplication) {
        // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
        // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    }
    
    func applicationWillEnterForeground(application: UIApplication) {
        // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    }
    
    func applicationDidBecomeActive(application: UIApplication) {
        // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    }
    
    func applicationWillTerminate(application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
        
        if !loginViewController.isAutoLoginEnabled() {
            TimeIQBGService.sharedInstance.logout()
        }
    }
    
    func application(application: UIApplication, handleActionWithIdentifier identifier: String?, forLocalNotification notification: UILocalNotification, completionHandler: () -> Void) {
        
        if identifier == "DO_IDENTIFIER" {
            NSNotificationCenter.defaultCenter().postNotificationName("doneActionNotification", object: nil, userInfo: notification.userInfo)
        }
        else if identifier == "CALL_IDENTIFIER" {
            NSNotificationCenter.defaultCenter().postNotificationName("callActionNotification", object: nil, userInfo: notification.userInfo)
        }
        else if identifier == "NOTIFY_IDENTIFIER" {
            NSNotificationCenter.defaultCenter().postNotificationName("notifyActionNotification", object: nil, userInfo: notification.userInfo)
        }
        else if identifier == "SNOOZE_IDENTIFIER" {
            NSNotificationCenter.defaultCenter().postNotificationName("snoozeActionNotification", object: nil, userInfo: notification.userInfo)
        }
        else if identifier == "BE_SNOOZE_IDENTIFIER" {
            NSNotificationCenter.defaultCenter().postNotificationName("beSnoozeActionNotification", object: nil, userInfo: notification.userInfo)
        }
        
        completionHandler()
    }
    
    func setupNotificationSettings() {
        
        let notificationActionCancel :UIMutableUserNotificationAction = UIMutableUserNotificationAction()
        notificationActionCancel.identifier = "CANCEL_IDENTIFIER"
        notificationActionCancel.title = "Cancel"
        notificationActionCancel.destructive = false
        notificationActionCancel.authenticationRequired = false
        notificationActionCancel.activationMode = UIUserNotificationActivationMode.Background
        
        let notificationActionDo :UIMutableUserNotificationAction = UIMutableUserNotificationAction()
        notificationActionDo.identifier = "DO_IDENTIFIER"
        notificationActionDo.title = "Done"
        notificationActionDo.destructive = false
        notificationActionDo.authenticationRequired = false
        notificationActionDo.activationMode = UIUserNotificationActivationMode.Background
        
        let notificationActionCall :UIMutableUserNotificationAction = UIMutableUserNotificationAction()
        notificationActionCall.identifier = "CALL_IDENTIFIER"
        notificationActionCall.title = "Call"
        notificationActionCall.destructive = false
        notificationActionCall.authenticationRequired = false
        notificationActionCall.activationMode = UIUserNotificationActivationMode.Foreground

        let notificationActionNotify :UIMutableUserNotificationAction = UIMutableUserNotificationAction()
        notificationActionNotify.identifier = "NOTIFY_IDENTIFIER"
        notificationActionNotify.title = "Send SMS"
        notificationActionNotify.destructive = false
        notificationActionNotify.authenticationRequired = false
        notificationActionNotify.activationMode = UIUserNotificationActivationMode.Foreground

        let notificationActionSnooze :UIMutableUserNotificationAction = UIMutableUserNotificationAction()
        notificationActionSnooze.identifier = "SNOOZE_IDENTIFIER"
        notificationActionSnooze.title = "Snooze"
        notificationActionSnooze.destructive = true
        notificationActionSnooze.authenticationRequired = false
        notificationActionSnooze.activationMode = UIUserNotificationActivationMode.Foreground
        
        let notificationActionBeSnooze :UIMutableUserNotificationAction = UIMutableUserNotificationAction()
        notificationActionBeSnooze.identifier = "BE_SNOOZE_IDENTIFIER"
        notificationActionBeSnooze.title = "Snooze"
        notificationActionBeSnooze.destructive = true
        notificationActionBeSnooze.authenticationRequired = false
        notificationActionBeSnooze.activationMode = UIUserNotificationActivationMode.Foreground

        let notificationCategoryDo:UIMutableUserNotificationCategory = UIMutableUserNotificationCategory()
        notificationCategoryDo.identifier = "DO_CATEGORY"
        notificationCategoryDo .setActions([notificationActionDo,notificationActionCancel], forContext: UIUserNotificationActionContext.Default)
        notificationCategoryDo .setActions([notificationActionDo,notificationActionCancel], forContext: UIUserNotificationActionContext.Minimal)
        
        let notificationCategoryCall:UIMutableUserNotificationCategory = UIMutableUserNotificationCategory()
        notificationCategoryCall.identifier = "CALL_CATEGORY"
        notificationCategoryCall.setActions([notificationActionCall,notificationActionSnooze], forContext: UIUserNotificationActionContext.Default)
        notificationCategoryCall.setActions([notificationActionCall,notificationActionSnooze], forContext: UIUserNotificationActionContext.Minimal)

        let notificationCategoryNotify:UIMutableUserNotificationCategory = UIMutableUserNotificationCategory()
        notificationCategoryNotify.identifier = "NOTIFY_CATEGORY"
        notificationCategoryNotify.setActions([notificationActionNotify,notificationActionSnooze], forContext: UIUserNotificationActionContext.Default)
        notificationCategoryNotify.setActions([notificationActionNotify,notificationActionSnooze], forContext: UIUserNotificationActionContext.Minimal)
        
        let notificationCategoryBe:UIMutableUserNotificationCategory = UIMutableUserNotificationCategory()
        notificationCategoryBe.identifier = "BE_CATEGORY"
        notificationCategoryBe.setActions([notificationActionBeSnooze,notificationActionCancel], forContext: UIUserNotificationActionContext.Default)
        notificationCategoryBe.setActions([notificationActionBeSnooze,notificationActionCancel], forContext: UIUserNotificationActionContext.Minimal)

        UIApplication.sharedApplication().registerUserNotificationSettings(UIUserNotificationSettings(forTypes: [UIUserNotificationType.Sound, UIUserNotificationType.Alert,
            UIUserNotificationType.Badge], categories: [notificationCategoryDo, notificationCategoryCall, notificationCategoryNotify,notificationCategoryBe]))
    }
    
    func customAppearance() {
        var attributes = [NSFontAttributeName: UIFont(name: "Helvetica Neue", size: 10) as! AnyObject , NSForegroundColorAttributeName: UIColor.whiteColor()]
        UITabBarItem.appearance().setTitleTextAttributes(attributes, forState: .Normal)
        
        UIBarButtonItem.appearance().tintColor = UIColor.whiteColor()
        
        UINavigationBar.appearance().barTintColor = UIColor(red: 25.0/255.0, green: 181.0/255.0, blue: 151.0/255.0, alpha: 1.0)
        attributes = [NSFontAttributeName: UIFont.boldSystemFontOfSize(17), NSForegroundColorAttributeName: UIColor.whiteColor()]
        UINavigationBar.appearance().titleTextAttributes = attributes

        UITabBar.appearance().barTintColor = UIColor(red: 94.0/255.0, green: 106.0/255.0, blue: 122.0/255.0, alpha: 1.0)
        UITabBar.appearance().tintColor = UIColor(red: 95.0/255.0, green: 227.0/255.0, blue: 200.0/255.0, alpha: 1.0)
        
//        let pageControl = UIPageControl.appearance()
//        pageControl.pageIndicatorTintColor = UIColor.lightGrayColor()
//        pageControl.currentPageIndicatorTintColor = UIColor.blackColor()
//        pageControl.backgroundColor = UIColor.whiteColor()
    }
    
    func getOrAskCalendarPermission() -> Bool {
        var res: Bool = true
        
        let eventStore = EKEventStore()
        var status = EKEventStore.authorizationStatusForEntityType(EKEntityType.Event)
        
        if status == EKAuthorizationStatus.NotDetermined {
            //request auth
            eventStore.requestAccessToEntityType(EKEntityType.Event, completion: {
                (Bool, NSError) -> Void in
            })
            while status == EKAuthorizationStatus.NotDetermined {
                status = EKEventStore.authorizationStatusForEntityType(EKEntityType.Event)
            }
        }
        
        if status != EKAuthorizationStatus.Authorized {
            res = false
        }
        
        return res
    }
}
