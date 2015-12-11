//
//  AddBeReminderViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 10/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TSO
import SMDatePicker

class AddBeReminderViewController: BaseAddReminderViewController, PlacesViewControllerDelegate, SMDatePickerDelegate, UITextFieldDelegate {
    
    var dataSource = ArrayList()
    var beTime : long64?
    let datePicker: SMDatePicker = SMDatePicker()
    var destination : TSOPlace?
    var errorMessage: String?
    var currentBeEvent: BeEvent?
    @IBOutlet var conflictEventsTableView: UITableView?
    @IBOutlet var addToCalendarSwitch: UISwitch?
    @IBOutlet var whenTextField: UITextField?
    @IBOutlet var whereTextField: UITextField?
    
    override init() {
        super.init()
    }
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Customize background colors
        self.datePicker.toolbarBackgroundColor = UIColor(red: 25.0/255.0, green: 181.0/255.0, blue: 151.0/255.0, alpha: 1.0)
        self.datePicker.pickerBackgroundColor = UIColor.whiteColor()
        self.datePicker.minimumDate = NSDate()

        self.populateExistData()
        self.conflictEventsTableView?.reloadData()
    }
    
    func showConflicts() {
        self.dataSource = TimeIQEventsUtils.getEventsBetweenTimes(self.beTime!, endTimeFrame: self.beTime!);
        self.conflictEventsTableView?.reloadData();
    }
    
    func populateExistData() {
        if (currentBeEvent != nil)
        {
            self.destination = self.currentBeEvent?.getLocation()
            self.whereTextField?.text = self.destination!.getAddress()
            self.beTime = self.currentBeEvent!.getArrivalTime()
            datePicker.pickerDate = NSDate(long: self.currentBeEvent!.getArrivalTime())
            datePicker.minimumDate = NSDate()
            self.whenTextField?.text = TextFormatterUtil.getReminderDateString(self.currentBeEvent!.getArrivalTime())
        }
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("eventsTableViewCell", forIndexPath: indexPath)
        let event = self.dataSource[indexPath.row] as! IEvent
        
        cell.textLabel?.text = String(format: NSLocalizedString("be_at", comment: ""), event.getSubject())
        cell.detailTextLabel?.text = String(format: "%@ -> %@", TextFormatterUtil.getHourAsString(event.getArrivalTime()),TextFormatterUtil.getHourAsString(event.getDuration()))
        
        return cell;
    }
    
    func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 12.0
    }
    
    func tableView(tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        let headerLabel = UILabel(frame: CGRect(x: 0, y: 0, width: self.conflictEventsTableView!.frame.size.width, height: 12.0))
        headerLabel.textColor = UIColor.blackColor();
        headerLabel.text = NSLocalizedString("be_in_conflict_with", comment: "")
        headerLabel.font = UIFont(name: "Helvetica Neue", size: 13.0)
        
        return headerLabel
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return self.dataSource.count
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
    }
    
    func showAlertView(title:String, message:String)
    {
        let alert = UIAlertController(title: title, message: message, preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: NSLocalizedString("Dismiss", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
        self.presentViewController(alert, animated: true, completion: nil);
    }
    
    @IBAction func didClickAction(sender: UIButton) {
        
        if (!self.isFormValid())
        {
            return;
        }
        
        let newBeEvent = BeEvent_BeEventBuilder(TSOPlace: destination, withLong: self.beTime!)
        
        if (self.addToCalendarSwitch!.on)
        {
            newBeEvent.addToCalendarWithBoolean(addToCalendarSwitch!.on)
        }
        
        let message = TimeIQEventsUtils.updateEvent(newBeEvent.build())
        
        let alert = UIAlertController(title: nil, message: message, preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: NSLocalizedString("OK", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
        self.presentViewController(alert, animated: true) { () -> Void in
            self.delegate.didUpdateReminder()
            self.dismissViewControllerAnimated(true, completion: nil)
        }
    }
    
    override func didClickDone() {
        
        if (!self.isFormValid())
        {
            return;
        }
        
        let writeCalendarResultData = TimeIQCalendarUtils.getWriteCalendar();
        if (writeCalendarResultData == nil) {
            self.showAlertView(NSLocalizedString("error_title", comment: ""), message: NSLocalizedString("be_event_no_writable_calendar", comment: ""))
        }
        
        let beEvent = BeEvent_BeEventBuilder(TSOPlace: destination, withLong: self.beTime!)
        
        if (self.addToCalendarSwitch!.on)
        {
            beEvent.addToCalendarWithBoolean(addToCalendarSwitch!.on)
        }
        
        let result = TimeIQEventsUtils.addBeEvent(beEvent.build())
        if (result.success) {
            self.delegate.didAddReminder();
            self.dismissViewControllerAnimated(true, completion: nil);
        }
        else {
            self.showAlertView(NSLocalizedString("error_title", comment: ""), message: result.message)
        }
    }
    
    func isFormValid() -> Bool
    {
        if (self.beTime == nil)
        {
            self.errorMessage = whenTextField?.placeholder
        }
            
        else if (self.destination == nil)
        {
            self.errorMessage = whereTextField?.placeholder
        }
        
        if (self.errorMessage != nil)
        {
            let alert = UIAlertController(title: NSLocalizedString("missing_data", comment: ""), message: self.errorMessage, preferredStyle: UIAlertControllerStyle.Alert)
            alert.addAction(UIAlertAction(title: NSLocalizedString("OK", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
            self.presentViewController(alert, animated: true, completion: nil);
            self.errorMessage = nil;
            
            return false;
        }
        return true;
    }
    
    func textFieldShouldBeginEditing(textField: UITextField) -> Bool {
        
        if (textField == self.whereTextField)
        {
            let placeVC = UIStoryboard.init(name: "Main", bundle: nil).instantiateViewControllerWithIdentifier("PlacesViewController") as! PlacesViewController;
            placeVC.modal = true;
            placeVC.delegate = self
            self.presentViewController(UINavigationController.init(rootViewController: placeVC), animated: true, completion: nil)
        }
        else if (textField == self.whenTextField)
        {
            self.datePicker.delegate = self
            self.datePicker.showPickerInView(view, animated: true)
        }
        
        return false;
    }
    
    func scrollViewDidScroll(scrollView: UIScrollView) {
        self.datePicker.hidePicker(true);
    }
    
    func didSelectPlace(place: TSOPlace) {
        self.whereTextField?.text = place.getAddress()
        self.destination = place
    }
    
    func datePicker(picker: SMDatePicker, didPickDate date: NSDate) {
        whenTextField?.text = TextFormatterUtil.getReminderDateString(date.getTime())
        self.beTime = date.getTime();
        self.showConflicts()
    }
}