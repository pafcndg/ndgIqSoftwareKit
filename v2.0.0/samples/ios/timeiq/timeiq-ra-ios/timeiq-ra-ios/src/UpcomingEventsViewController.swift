//
//  UpcomingEventsViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 18/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TimeIQ

class UpcomingEventsViewController: UITableViewController {
    
    var dataSource: ArrayList?

    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.reloadData()
        self.refreshControl?.addTarget(self, action: "handleRefresh:", forControlEvents: UIControlEvents.ValueChanged)
    }
    
    func handleRefresh(refreshControl: UIRefreshControl) {
        self.reloadData()
        self.refreshControl?.endRefreshing()
    }
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        self.reloadData()
    }
    
    func reloadData() {
        self.dataSource = TimeIQEventsUtils.getUpcomingEvents()
        self.tableView.reloadData()
    }
    
    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if (self.dataSource?.count() > 0)
        {
            self.tableView.backgroundView = nil
            return self.dataSource!.count
        }
        else
        {
            // Display a message when the table is empty
            self.tableView.backgroundView = common.labelMessageForString(NSLocalizedString("nothing_to_display_events", comment: ""));
        }
        
        return 0;
    }
    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = self.tableView!.dequeueReusableCellWithIdentifier("upcomingEventTableViewCell", forIndexPath: indexPath) as! upcomingEventTableViewCell
        
        let event = dataSource![indexPath.row] as! IEvent
        cell.startTTLTimer = true
        cell.event = event
        
        return cell;
    }
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
    }
}
