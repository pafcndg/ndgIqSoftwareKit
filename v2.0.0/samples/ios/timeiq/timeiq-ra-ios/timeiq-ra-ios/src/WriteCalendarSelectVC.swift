//
//  WriteCalendarSelectVC.swift
//  SensorRefApp2
//
//  Created by Nir Bitton on 8/27/15.
//  Copyright (c) 2015 fiat.bender. All rights reserved.
//

import UIKit
import EventKit

protocol WriteCalendarDelegate {
    func writeCalendarSelected(calendarSelected:EKCalendar)
}

class WriteCalendarSelectVC: UITableViewController {
    
    var allWritableCalendars:NSMutableArray!
    var delegate:WriteCalendarDelegate!

    override func viewDidLoad() {
        super.viewDidLoad()

        allWritableCalendars = NSMutableArray()
        let eventStore = EKEventStore()
        let allCalendars:NSArray = eventStore.calendarsForEntityType(EKEntityType.Event)
        for calendar in allCalendars
        {
            if (calendar.allowsContentModifications == true)
            {
                allWritableCalendars.add(calendar)
            }
        }
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // MARK: - Table view data source

    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return allWritableCalendars.count
    }

    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("calendar", forIndexPath: indexPath) 
        
        cell.textLabel?.text = allWritableCalendars[indexPath.row].title

        return cell
    }
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        let calendar:EKCalendar = allWritableCalendars[indexPath.row] as! EKCalendar
        delegate.writeCalendarSelected(calendar)
        self.navigationController?.popViewControllerAnimated(true)
    }

}
