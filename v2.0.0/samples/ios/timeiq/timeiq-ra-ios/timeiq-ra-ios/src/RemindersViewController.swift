//
//  RemindersViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 01/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TimeIQ
import TimeAgoInWords
import EventKit

class RemindersViewController: UIViewController, UITableViewDelegate, UITableViewDataSource, PagedViewControllerDelegate, BaseAddReminderViewControllerDelegate {

    var dataSource = Array<AnyObject>();
    var activityIndicator = UIActivityIndicatorView(activityIndicatorStyle: UIActivityIndicatorViewStyle.Gray);
    var updateTimeAgoTimer: NSTimer?
    
    @IBOutlet weak var reminderTableView : UITableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "userLoggedIn", name: NOTIFICATION_LOGIN_SUCCESS, object: nil);
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "reloadData", name: NOTIFICATION_RELOAD_REMINDERS, object: nil);
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewDidAppear(animated: Bool) {
        super.viewDidAppear(animated)
        login();
        
        updateTimeAgoTimer = NSTimer.scheduledTimerWithTimeInterval(20.0, target: self, selector: Selector("updateTimeAgo"), userInfo: nil, repeats: true)
        updateTimeAgoTimer?.fire()
    }
    
    override func viewWillDisappear(animated: Bool) {
        super.viewWillDisappear(animated)
        
        updateTimeAgoTimer?.invalidate()
    }
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        let nav = segue.destinationViewController as! UINavigationController
        let pagedVC = nav.topViewController as! PagedViewController
        pagedVC.delegate = self
    }
    
    func login()
    {
        if (!TimeIQBGService.sharedInstance.isUserLoggedIn()) {
            showLoginVC()
        }
    }
    
    private func showLoginVC() {
        let storyboard = UIStoryboard(name: "Login", bundle: nil)
        let userLoginVC = storyboard.instantiateViewControllerWithIdentifier("loginNavigationController")
        self.presentViewController(userLoginVC, animated: true, completion: nil)
    }
    
    func enableLoading(flag: Bool)
    {
        if (flag)
        {
            self.navigationItem.titleView = activityIndicator;
            activityIndicator.startAnimating()
        }
        else
        {
            self.navigationItem.titleView = nil;
            self.navigationItem.title = NSLocalizedString("Time IQ", comment: "");
            self.activityIndicator.stopAnimating()
        }
    }
    
    func userLoggedIn()
    {
        reloadData()
    }
    
    func reloadData() {
        dataSource = TimeIQRemindersUtils.getAllActiveReminders();
        dataSource += TimeIQEventsUtils.getBeEventsForTheNextYear()
        reminderTableView.reloadData();
    }
    
    @IBAction func didClickLogout(sender: AnyObject) {
        //TODO: curretnly logout is implemented on Auth level only, need to impelment app-wise (delte db, files, etc.)
        
        TimeIQBGService.sharedInstance.logout()
        
        showLoginVC()
    }
    
    @IBAction func didClickAdd(sender: AnyObject) {
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("reminderTableViewCell", forIndexPath: indexPath) as! reminderTableViewCell
        
        let object = dataSource[indexPath.row];
        
        if (object is IReminder)
        {
            let reminder = object as! IReminder;
            cell.reminder = reminder;
        }
        else if (object.isKindOfClass(BeEvent))
        {
            let beEvent = object as! BeEvent;
            cell.beEvent = beEvent;
        }
        
        return cell;
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if (self.dataSource.count > 0)
        {
            self.reminderTableView.backgroundView = nil
            return self.dataSource.count
        }
        else
        {
            // Display a message when the table is empty
            self.reminderTableView.backgroundView = common.labelMessageForString(NSLocalizedString("nothing_to_display_reminder", comment: ""));
        }
        
        return 0;
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        let object = dataSource[indexPath.row];
        
        if (object is IReminder)
        {
            let reminder = object as! IReminder;
            
            let reminderVC = UIStoryboard.init(name: "Main", bundle: nil).instantiateViewControllerWithIdentifier("AddReminderViewController") as! AddReminderViewController;
            reminderVC.currentReminder = reminder
            reminderVC.delegate = self
            self.navigationController?.pushViewController(reminderVC, animated: true)
        }
        else if (object.isKindOfClass(BeEvent))
        {
            let beEvent = object as! BeEvent;
            
            let reminderVC = UIStoryboard.init(name: "Main", bundle: nil).instantiateViewControllerWithIdentifier("AddBeReminderViewController") as! AddBeReminderViewController;
            reminderVC.currentBeEvent = beEvent
            reminderVC.delegate = self
            self.navigationController?.pushViewController(reminderVC, animated: true)
        }
    }
    
    func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        return true
    }
    
//    func tableView(tableView: UITableView, editActionsForRowAtIndexPath indexPath: NSIndexPath) -> [UITableViewRowAction]? {
//        let deleteButton = UITableViewRowAction(style: .Default, title: "Delete", handler: { (action, indexPath) in
//            self.reminderTableView.dataSource?.tableView?(
//                self.reminderTableView,
//                commitEditingStyle: .Delete,
//                forRowAtIndexPath: indexPath
//            )
//            return
//        })
//        
//        deleteButton.backgroundColor = UIColor.blackColor()
//        
//        return [deleteButton]
//    }
    
    func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
        if (editingStyle == UITableViewCellEditingStyle.Delete)
        {
            let object = dataSource[indexPath.row];
            
            if (object is IReminder)
            {
                let reminder = object as! IReminder;

                if (TimeIQRemindersUtils.removeReminder(reminder.getId()))
                {
                    dataSource.removeAtIndex(indexPath.row);
                    tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: UITableViewRowAnimation.Automatic);
                }
            }
            else if (object.isKindOfClass(BeEvent))
            {
                let beEvent = object as! BeEvent;
                
                if (TimeIQEventsUtils.deleteEvent(beEvent.getId()))
                {
                    dataSource.removeAtIndex(indexPath.row);
                    tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: UITableViewRowAnimation.Automatic);
                }
            }
        }
    }
    
    func didAddOrUpdateReminder() {
        self.reloadData()
    }
    
    func didAddReminder() {
        self.reloadData()
    }
    
    func didUpdateReminder() {
        self.reloadData()
    }
    
    //MARK:
    
    func updateTimeAgo() {
        for cell in self.reminderTableView.visibleCells {
            guard let reminderCell = cell as? reminderTableViewCell else { continue }
            reminderCell.updateTimeAgoLabel()
        }
    }
}

