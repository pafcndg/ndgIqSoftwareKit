//
//  AddReminderViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 03/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TimeIQ
import AddressBook
import AddressBookUI
import SMDatePicker

class AddReminderViewController: BaseAddReminderViewController, UITableViewDelegate, UITableViewDataSource, UITextFieldDelegate, UITextViewDelegate, ABPeoplePickerNavigationControllerDelegate, SMDatePickerDelegate, PlacesViewControllerDelegate {
    
    var targetArrivePlace : TSOPlace?
    var targetLeavePlace : TSOPlace?
    var reminderTime : Int64?
    var targetContactInfo : ContactInfo?
    var dataSource : Array<String>?
    var currentSelectedTrigger:Int!
    let datePicker: SMDatePicker = SMDatePicker()
    var errorMessage: String?
    var currentReminder: IReminder? {
        willSet(newCurrentReminder) {
            switch(newCurrentReminder!.getReminderType())
            {
            case ReminderType.DO():
                self.reminderType = REMINDER_TYPE_DO
                break;
            case ReminderType.CALL():
                self.reminderType = REMINDER_TYPE_CALL
                break;
            case ReminderType.NOTIFY():
                self.reminderType = REMINDER_TYPE_NOTIFY
                break;
            default:
                break;
            }
        }
    }
    
    @IBOutlet weak var triggersTableView : UITableView!
    @IBOutlet weak var reminderTextField: UITextField!
    @IBOutlet weak var smsTextField: UITextView!
    
    override init() {
        super.init()
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setPlaceholder()
        
        // Customize background colors
        self.datePicker.toolbarBackgroundColor = UIColor(red: 25.0/255.0, green: 181.0/255.0, blue: 151.0/255.0, alpha: 1.0)
        self.datePicker.pickerBackgroundColor = UIColor.whiteColor()
        self.datePicker.minimumDate = NSDate()
        self.smsTextField.textColor = UIColor.lightGrayColor()
        
        self.populateExistData()
        self.reloadData()
    }
    
    func populateExistData() {
        if (currentReminder != nil)
        {
            switch(currentReminder!.getReminderType())
            {
            case ReminderType.DO():
                let doReminder = currentReminder as! DoReminder
                self.reminderTextField.text = doReminder.getDoAction()
                self.reminderType = REMINDER_TYPE_DO
                break;
            case ReminderType.CALL():
                let callReminder = currentReminder as! CallReminder
                self.reminderTextField.text = callReminder.getContactInfo().getName() + " - " + TimeIQRemindersUtils.getPhoneNumber(callReminder);
                self.reminderType = REMINDER_TYPE_CALL
                self.targetContactInfo = callReminder.getContactInfo()
                
                break;
            case ReminderType.NOTIFY():
                let notifyReminder = currentReminder as! NotificationReminder
                self.reminderTextField.text = notifyReminder.getContactInfo().getName() + " - " + TimeIQRemindersUtils.getPhoneNumber(notifyReminder);
                self.smsTextField.textColor = UIColor.blackColor()
                self.smsTextField.text = notifyReminder.getNotificationMessage()
                self.reminderType = REMINDER_TYPE_NOTIFY
                self.targetContactInfo = notifyReminder.getContactInfo()
                break;
            default:
                break;
            }
            
            switch (currentReminder!.getTrigger().getTriggerType())
            {
            case TriggerType.MOT():
                currentSelectedTrigger = TRIGGER_TYPE_NEXT_DRIVE
                break;
            case TriggerType.CHARGE():
                currentSelectedTrigger = TRIGGER_TYPE_BATTERY
                break;
            case TriggerType.TIME():
                currentSelectedTrigger = TRIGGER_TYPE_TIME
                let timeTrigger = currentReminder!.getTrigger() as! TimeTrigger
                datePicker.pickerDate = NSDate(long: timeTrigger.getTriggerTime())
                datePicker.minimumDate = NSDate()
                break;
            case TriggerType.PLACE():
                let placeTrigger = currentReminder?.getTrigger() as! PlaceTrigger
                switch (placeTrigger.getPlaceTriggerType())
                {
                case PlaceTriggerType.ARRIVE():
                    currentSelectedTrigger = TRIGGER_TYPE_ARRIVE
                    targetArrivePlace = TimeIQPlacesUtil.getPlace(placeTrigger.getPlaceId())
                    break;
                case PlaceTriggerType.LEAVE():
                    currentSelectedTrigger = TRIGGER_TYPE_LEAVE
                    targetLeavePlace = TimeIQPlacesUtil.getPlace(placeTrigger.getPlaceId())
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }
    
    func reloadData() {
        self.dataSource = prepareDataSource()
        self.triggersTableView.reloadData();
        if currentSelectedTrigger != nil  {
            self.triggersTableView.selectRowAtIndexPath(NSIndexPath(forRow: currentSelectedTrigger, inSection: 0), animated: true, scrollPosition: .None)
        }
    }
    
    func prepareDataSource() -> Array<String>
    {
        var list = [String]()
        
        if (targetArrivePlace == nil)
        {
            list.append(NSLocalizedString("TRIGGER_TYPE_ARRIVE", comment: ""));
        }
        else
        {
            list.append(String(format:NSLocalizedString("TRIGGER_TYPE_ARRIVE_TO", comment: ""), targetArrivePlace!.getName()));
        }
        
        if (targetLeavePlace == nil)
        {
            list.append(NSLocalizedString("TRIGGER_TYPE_LEAVE", comment: ""));
        }
        else
        {
            list.append(String(format:NSLocalizedString("TRIGGER_TYPE_LEAVE_FROM", comment: ""), targetLeavePlace!.getName()));
        }
        
        if (self.reminderType != REMINDER_TYPE_NOTIFY)
        {
            list.append(NSLocalizedString("TRIGGER_TYPE_NEXT_DRIVE", comment: ""));
            list.append(String(format:NSLocalizedString("TRIGGER_TYPE_BATTERY", comment: ""), "15%"));
            
            if (reminderTime != nil)
            {
                list.append(TextFormatterUtil.getReminderDateString(reminderTime!));
            }
            else
            {
                list.append(NSLocalizedString("TRIGGER_TYPE_TIME", comment: ""));
            }
        }
        
        return list;
    }
    
    func setPlaceholder()
    {
        switch(reminderType)
        {
        case REMINDER_TYPE_DO:
            self.reminderTextField.placeholder = NSLocalizedString("placeholder_do", comment: "");
            self.triggersTableView.tableFooterView?.hidden = true;
            break;
        case REMINDER_TYPE_CALL:
            self.reminderTextField.placeholder = NSLocalizedString("placeholder_call", comment: "");
            self.triggersTableView.tableFooterView?.hidden = true;
            break;
        case REMINDER_TYPE_NOTIFY:
            self.reminderTextField.placeholder = NSLocalizedString("placeholder_notify", comment: "");
            break;
        default:
            break;
        }
    }
    
    func getReminder() -> IReminder? {
        var reminder : IReminder?
        let trigger = getTrigger();
        
        if (!self.isFormValid())
        {
            return nil;
        }
        
        switch (reminderType)
        {
        case REMINDER_TYPE_DO:
            reminder = DoReminder_DoReminderBuilder(ITrigger: trigger, withNSString: reminderTextField.text).build();
            break;
        case REMINDER_TYPE_CALL:
            reminder = CallReminder_CallReminderBuilder(ITrigger: trigger, withContactInfo: targetContactInfo).build();
            break;
        case REMINDER_TYPE_NOTIFY:
            reminder = NotificationReminder_NotificationReminderBuilder(ITrigger: trigger, withContactInfo: targetContactInfo, withNSString: smsTextField.text).build();
            break;
        default:
            break;
        }
        
        return reminder!
    }
    
    @IBAction func didClickAction(sender: UIButton) {
        
        let reminder = getReminder()
        
        if (!self.isFormValid())
        {
            return;
        }
        
        let message = TimeIQRemindersUtils.editReminder(reminder, oldReminderId: self.currentReminder!.getId());
        
        let alert = UIAlertController(title: nil, message: message, preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: NSLocalizedString("OK", comment: ""), style: UIAlertActionStyle.Default,handler: { (action) -> Void in
            self.delegate.didUpdateReminder()
            self.navigationController?.popViewControllerAnimated(true)
        }))
        self.presentViewController(alert, animated: true, completion: nil);
    }
    
    override func didClickDone() {
        
        let reminder = getReminder()
        
        if (!self.isFormValid())
        {
            return;
        }
        
        let result = TimeIQBGService.sharedInstance.timeIQApi?.getRemindersManager().addReminderWithIReminder(reminder);
        if ((result?.isSuccess()) != nil)
        {
            self.delegate.didAddReminder();
        }
        
        self.dismissViewControllerAnimated(true, completion: nil);
    }
    
    func isFormValid() -> Bool
    {
        if (self.reminderTextField.text!.isEmpty)
        {
            self.errorMessage = self.reminderTextField.placeholder;
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
    
    func getTrigger() -> ITrigger?
    {
        var trigger: ITrigger!
        
        if (currentSelectedTrigger != nil)
        {
            switch (currentSelectedTrigger)
            {
            case TRIGGER_TYPE_ARRIVE:
                if (targetArrivePlace != nil)
                {
                    trigger = PlaceTrigger_PlaceTriggerBuilder(placeTriggerType: PlaceTriggerType.ARRIVE(), withPlaceID: targetArrivePlace?.getPlaceId()).build();
                }
                else
                {
                    self.errorMessage = NSLocalizedString("reminder_error_missing_arrive_place", comment: "");
                }
                break;
            case TRIGGER_TYPE_LEAVE:
                if (targetLeavePlace != nil)
                {
                    trigger = PlaceTrigger_PlaceTriggerBuilder(placeTriggerType: PlaceTriggerType.LEAVE(), withPlaceID: targetLeavePlace?.getPlaceId()).build();
                }
                else
                {
                    self.errorMessage = NSLocalizedString("reminder_error_missing_leave_place", comment: "");
                }
                break;
            case TRIGGER_TYPE_NEXT_DRIVE:
                trigger = MotTrigger_MotTriggerBuilder(motType: MotType.CAR(), withMotTransition: MotTransition.START()).build();
                break;
            case TRIGGER_TYPE_BATTERY:
                trigger = ChargeTrigger_ChargeTriggerBuilder(chargeTriggerType: ChargeTriggerType.BATTERY_OK()).setPercentWithInt(15).build();
                break;
            case TRIGGER_TYPE_TIME:
                if (self.reminderTime != nil)
                {
                    trigger = TimeTrigger_TimeTriggerBuilder(long : reminderTime!).build();
                }
                else
                {
                    self.errorMessage = NSLocalizedString("reminder_error_missing_reminder_time", comment: "");
                }
                break;
            default:
                break;
            }
        }
        else
        {
            self.errorMessage = NSLocalizedString("reminder_error_missing_data", comment: "");
        }
        
        return trigger;
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("triggerTableViewCell", forIndexPath: indexPath) as! triggerTableViewCell
        
        let triggerString = dataSource![indexPath.row];
        
        cell.titleLabel?.text = NSLocalizedString(triggerString, comment: "");
        
        return cell;
    }
    
    func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 12.0
    }
    
    func tableView(tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        let headerLabel = UILabel(frame: CGRect(x: 0, y: 0, width: self.triggersTableView.frame.size.width, height: 12.0))
        headerLabel.textColor = UIColor.blackColor();
        headerLabel.text = NSLocalizedString("trigger_header_in_section", comment: "")
        headerLabel.font = UIFont(name: "Helvetica Neue", size: 13.0)
        
        return headerLabel
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return dataSource!.count;
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        self.currentSelectedTrigger = self.triggersTableView.indexPathForSelectedRow!.row
        self.view.endEditing(true)
        self.datePicker.hidePicker(true);
        
        switch (currentSelectedTrigger)
        {
        case TRIGGER_TYPE_ARRIVE, TRIGGER_TYPE_LEAVE:
            let placeVC = UIStoryboard.init(name: "Main", bundle: nil).instantiateViewControllerWithIdentifier("PlacesViewController") as! PlacesViewController;
            placeVC.modal = true;
            placeVC.delegate = self
            self.presentViewController(UINavigationController.init(rootViewController: placeVC), animated: true, completion: nil)
            break;
        case TRIGGER_TYPE_TIME:
            self.datePicker.delegate = self
            self.datePicker.showPickerInView(view, animated: true)
            break;
        default:
            break;
        }
    }
    
    func scrollViewDidScroll(scrollView: UIScrollView) {
        self.view.endEditing(true)
        self.datePicker.hidePicker(true);
    }
    
    func textFieldShouldBeginEditing(textField: UITextField) -> Bool {
        var editing = true;
        
        switch (self.reminderType)
        {
        case REMINDER_TYPE_CALL,REMINDER_TYPE_NOTIFY:
            ABAddressBookRequestAccessWithCompletion(nil) {
                (granted:Bool, err:CFError!) in
                dispatch_async(dispatch_get_main_queue()) {
                    if granted {
                        let peoplePickerVC = ABPeoplePickerNavigationController.init();
                        peoplePickerVC.peoplePickerDelegate = self;
                        //                        peoplePickerVC.displayedProperties = [NSNumber(int: kABPersonFirstNameProperty)]
                        self.presentViewController(peoplePickerVC, animated: true, completion: nil);
                    }
                }
            }
            editing = false;
            break;
        default:
            break;
        }
        return editing;
    }
    
    func peoplePickerNavigationController(peoplePicker: ABPeoplePickerNavigationController, didSelectPerson person: ABRecord)
    {
        let firstNameCFString : CFString = ABRecordCopyCompositeName(person).takeRetainedValue()
        let firstName = firstNameCFString as String
        
        let unmanagedPhones = ABRecordCopyValue(person, kABPersonPhoneProperty)
        
        let phones: ABMultiValueRef = Unmanaged.fromOpaque(unmanagedPhones.toOpaque()).takeUnretainedValue()
        
        
        let phoneUnmaganed = ABMultiValueCopyValueAtIndex(phones, 0)
        
        if (phoneUnmaganed != nil)
        {
            let phoneNumber : String = phoneUnmaganed.takeUnretainedValue() as! String
            
            //        for(var numberIndex : CFIndex = 0; numberIndex < ABMultiValueGetCount(phones); numberIndex++)
            //        {
            //            let phoneUnmaganed = ABMultiValueCopyValueAtIndex(phones, numberIndex)
            //            let phoneNumber : String = phoneUnmaganed.takeUnretainedValue() as! String
            //            swiftString = phoneNumber
            //        }
            
            self.targetContactInfo = ContactInfo(NSString: firstName, withNSString: firstName, withDate: nil, withNSString: phoneNumber)
            self.targetContactInfo?.setPreferredPhoneNumberWithNSString(phoneNumber)
            
            self.reminderTextField.text = firstName + " - " + phoneNumber;
            self.buildTextMessage()
        }
    }
    
    func datePicker(picker: SMDatePicker, didPickDate date: NSDate) {
        print(date);
        self.reminderTime = date.getTime();
    }
    
    func buildTextMessage() {
        var message = "Hi ";
        
        if (self.targetContactInfo != nil)
        {
            message += self.targetContactInfo!.getName() + ", I just ";
        }
        
        if (self.targetArrivePlace != nil)
        {
            message += "arrive " + self.targetArrivePlace!.getName()
        }
        
        if (self.targetLeavePlace != nil)
        {
            message += "leave " + self.targetLeavePlace!.getName()
        }
        
        self.smsTextField.text = message
    }
    
    func didSelectPlace(place: TSOPlace) {
        if (self.currentSelectedTrigger == TRIGGER_TYPE_ARRIVE)
        {
            self.targetLeavePlace = nil
            self.targetArrivePlace = place
        }
        else
        {
            self.targetArrivePlace = nil
            self.targetLeavePlace = place
        }
        
        self.buildTextMessage()
        self.reloadData()
    }
}
