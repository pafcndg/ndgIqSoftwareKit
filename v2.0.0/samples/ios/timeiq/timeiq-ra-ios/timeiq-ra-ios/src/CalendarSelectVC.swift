//
//  CalendarSelectVC.swift
//  SensorRefApp2
//
//  Created by Nir Bitton on 8/26/15.
//  Copyright (c) 2015 fiat.bender. All rights reserved.
//

import UIKit
import EventKit
import TimeIQ

class CalendarSelectVC: UITableViewController, WriteCalendarDelegate {
    
    var allCalendars:NSArray!
    var readSelectedCalendars:NSMutableArray!
    var writeSelectedCalendar:EKCalendar!
    
    var prevSelectedReadCalendars:ArrayList!
    var prevSelectedWriteCalendars:CalendarDetails!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        if TimeIQCalendarUtils.getAvailableCalendars() != nil
        {
            prevSelectedReadCalendars = TimeIQCalendarUtils.getReadCalendars()
        }
        if TimeIQCalendarUtils.getWriteCalendar() != nil
        {
            prevSelectedWriteCalendars = TimeIQCalendarUtils.getWriteCalendar()
        }
        
        let eventStore = EKEventStore()
        allCalendars = eventStore.calendarsForEntityType(EKEntityType.Event)
        readSelectedCalendars = NSMutableArray()
        
        if prevSelectedWriteCalendars != nil
        {
            for cal in allCalendars
            {
                if cal.calendarIdentifier == prevSelectedWriteCalendars.getCalendarId()
                {
                    writeSelectedCalendar = cal as! EKCalendar
                    break
                }
            }
        }
        else
        {
            writeSelectedCalendar = allCalendars[0] as! EKCalendar
        }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    // MARK: - Table view data source
    
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 2
    }
    
    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if section == 0
        {
            return allCalendars.count
        }
        else
        {
            return 1
        }
    }
    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("calendar", forIndexPath: indexPath)
        
        if (indexPath.section == 0)
        {
            cell.textLabel?.text = allCalendars[indexPath.row].title
            cell.selectionStyle = .None
            
            
            let uiSwitch = UISwitch(frame: CGRectZero)
            cell.addSubview(uiSwitch)
            cell.accessoryView = uiSwitch
            uiSwitch.tag = indexPath.row
            
            uiSwitch.addTarget(self, action: Selector("switchChanged:"), forControlEvents: UIControlEvents.ValueChanged)
            
            for prevCalendar in prevSelectedReadCalendars
            {
                if (allCalendars[indexPath.row].calendarIdentifier == prevCalendar.getCalendarId())
                {
                    uiSwitch.setOn(true, animated: true)
                }
            }
        }
        else
        {
            cell.textLabel?.text = writeSelectedCalendar.title
            cell.accessoryType = UITableViewCellAccessoryType.DisclosureIndicator
            cell.userInteractionEnabled = true
            
        }
        
        
        return cell
    }
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        if indexPath.section == 1
        {
            self.performSegueWithIdentifier("selectWriteCal", sender: self)
        }
    }
    
    override func tableView(tableView: UITableView, titleForHeaderInSection section: Int) -> String?
    {
        if section == 0
        {
            return "Select read calendars"
        }
        else
        {
            return "Select write calendar"
        }
    }
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?)
    {
        if segue.identifier == "selectWriteCal"
        {
            if let vcDest:WriteCalendarSelectVC = segue.destinationViewController as? WriteCalendarSelectVC
            {
                vcDest.delegate = self
            }
            
        }
        
    }
    
    func writeCalendarSelected(calendarSelected:EKCalendar)
    {
        writeSelectedCalendar = calendarSelected
        self.tableView .reloadData()
    }
    
    @IBAction func saveAction(sender: AnyObject)
    {
        if readSelectedCalendars.count > 0
        {
            let array = ArrayList()
            for var index:Int = 0; index < readSelectedCalendars.count; ++index
            {
                let cal:EKCalendar = readSelectedCalendars[index] as! EKCalendar
                let calendarId = cal.calendarIdentifier
                //                let calendarName = cal.title
                //                var accountName = cal.source.title
                //                var accountType = cal.type
                //                var calendarDetails = CalendarDetails(NSString: calendarId, withNSString: calendarName, withNSString: calendarName, withNSString: calendarName)
                array.add(calendarId)
            }
           TimeIQCalendarUtils.setReadCalendars(array)
        }
        
        if writeSelectedCalendar != nil
        {
            let calendarId = writeSelectedCalendar.calendarIdentifier
            TimeIQCalendarUtils.setWriteCalendar(calendarId)
        }
        
        self.navigationController?.popViewControllerAnimated(true)
    }
    
    func alertView(message:String)
    {
        let alert = UIAlertController(title: title, message: message, preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: NSLocalizedString("Dismiss", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
        self.presentViewController(alert, animated: true, completion: nil);
        
        let delay = 5.0 * Double(NSEC_PER_SEC)
        let time = dispatch_time(DISPATCH_TIME_NOW, Int64(delay))
        dispatch_after(time, dispatch_get_main_queue(), {
            alert.dismissViewControllerAnimated(true, completion: nil)
        })
    }
    
    func switchChanged(switchState: UISwitch)
    {
        let uiSwitch:UISwitch = switchState
        let calendar:EKCalendar = allCalendars[uiSwitch.tag] as! EKCalendar
        
        if uiSwitch.on
        {
            readSelectedCalendars.add(calendar)
        }
        else
        {
            readSelectedCalendars.removeObject(calendar)
        }
        
    }
    
}
