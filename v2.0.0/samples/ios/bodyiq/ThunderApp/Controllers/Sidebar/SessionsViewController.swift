//
//  ActivityViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/6/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import BodyIQ

class SessionsCell:UITableViewCell {
    
    @IBOutlet weak var sessionTypeIcon:UIImageView!
    @IBOutlet weak var sessionTypeLabel:UILabel!
    @IBOutlet weak var sessionStart:UILabel!
    @IBOutlet weak var sessionDistance:UILabel!
    @IBOutlet weak var sessionDuration:UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
    
    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)
    }
}

struct ObservationRecord {
    let observation: TimeSeriesObservation
    let status: String
    let date:String
}

class SessionsViewController: TabVCTemplate, UITableViewDelegate, UITableViewDataSource {

    // MARK: Members
    private var observationRecords  = [ObservationRecord]()
    private let body                = BodyManager(controller: (GlobalStorage.controller?.controller)!)
    
    
    // MARK: UI
    @IBOutlet weak var table:UITableView!
    @IBOutlet weak var sessionBtn:UIButton!
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        selectedTab = 1
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        // Start observing
        NSNotificationCenter.defaultCenter().addObserver(self, selector: Selector("setConnectionState:"), name: Constants.ConnectionStateNotification, object: nil)
        
        connect()
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
        
        body.unsubscribe()
        
        // Stop observing
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }
    
    
    // MARK: Buttons
    
    @IBAction func toggleMenu(sender: AnyObject) {
        NSNotificationCenter.defaultCenter().postNotificationName(Constants.ToggleMenu, object: nil)
    }
    
    @IBAction func startListeningForActivitiesBtnTap() {
        
        if body.isSubscribed {
            
            body.unsubscribe()
            
            sessionBtn.setImage(UIImage(named: "start_session"), forState: UIControlState.Normal)
        }
        else {
            
            sessionBtn.setImage(UIImage(named: "stop_session"), forState: UIControlState.Normal)
                        
            
            body.subscribe( { observation, status, deviceID in
                
                let record = ObservationRecord(observation: observation, status: status, date: NSDate.currentDate("h:mm a"))
                
                self.pushNewActivity(record)
                
                print("************************************************************************")
                DLog("App recieved \(observation.eventType): \(status) wearableController event")
                
                if let value = observation.value as? BodyIQWalkingTimeSeriesValue {
                    
                    print("Steps: \(value.stepCount) \n")
                    print("Calories: \(value.calories) \n")
                    print("Distance: \(value.distance) \n")
                    print("Speed: \(value.speed) \n")
                    print("Duration: \(value.duration) \n")
                }
                print("************************************************************************")
            })
        }
    }
    

    // MARK: UITableViewDelegate methods
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        tableView.deselectRowAtIndexPath(indexPath, animated: true)
        
    }
    
    func tableView(tableView: UITableView, estimatedHeightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        
        return 100.0
    }
    
    
    // MARK: UITableViewDataSource methods
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        
        tableView.separatorColor = UIColor .clearColor()
        return observationRecords.count
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        
        let cell                = tableView.dequeueReusableCellWithIdentifier(Constants.SessionCellId) as! SessionsCell
        
        // Disable the cell's selection property
        cell.selectionStyle     = UITableViewCellSelectionStyle.None
        
        cell.backgroundColor    = UIColor.clearColor()
        
        // Fill cell
        if let record = observationRecordForIndexPath(indexPath) {
         
            if let value = record.observation.value as? BodyIQWalkingTimeSeriesValue {
                
                let (h,m,s) = TimeSeriesManager.secondsToHoursMinutesSeconds(Int(value.duration))
                
                cell.sessionDistance.text  = String(format: "%.1f m", value.distance)
                cell.sessionDuration.text  = "\(h) Hours, \(m) Minutes, \(s) Seconds"
                cell.sessionStart.text     = record.date
                
                let lowerCaseSt = record.observation.eventType.lowercaseString
                
                cell.sessionTypeLabel.text = lowerCaseSt.capitalizedString
                
                cell.sessionTypeIcon.image = UIImage(named: "\(lowerCaseSt)")
            }
        }
        
        return cell;
    }
    
    func tableView(tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        
        return NSDate.currentDate("EEEE, MMMM dd")
    }
    
    func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 70
    }
    
    func tableView(tableView: UITableView, willDisplayHeaderView view: UIView, forSection section: Int) {
        
        let header: UITableViewHeaderFooterView = view as! UITableViewHeaderFooterView
        
        // Background
        header.contentView.backgroundColor      = UIColor.clearColor()
        header.tintColor                        = UIColor.clearColor()
        
        // Text
        header.textLabel!.textColor             = UIColor.init(hex: Constants.SecondaryActionColor)
        header.textLabel!.textAlignment         = NSTextAlignment.Center
        header.textLabel!.font                  = UIFont(name: "HelveticaNeue ", size: 18)
    }
    
    
    // MARK: Helper
    
    func observationRecordForIndexPath(indexPath: NSIndexPath) -> ObservationRecord? {
        let row = indexPath.row
        return row < observationRecords.count ? observationRecords[ observationRecords.count - row - 1 ] : nil
    }
    
    func pushNewActivity(activityStatus: ObservationRecord) {
        
        // Started observations come along only with 0, so we don't show them
        guard activityStatus.status.lowercaseString != "started" else {
            return
        }
        observationRecords.append(activityStatus)
        table.insertRowsAtIndexPaths([NSIndexPath(forRow: 0, inSection: 0)], withRowAnimation: UITableViewRowAnimation.Automatic)
    }
}

