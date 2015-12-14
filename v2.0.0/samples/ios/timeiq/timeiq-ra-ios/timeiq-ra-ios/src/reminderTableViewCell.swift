//
//  reminderTableViewCell.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 08/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TimeIQ

class reminderTableViewCell: UITableViewCell {
    
    @IBOutlet var reminderImageView: UIImageView?
    @IBOutlet var reminderTitleLabel: UILabel?
    @IBOutlet var reminderSubTitleLabel: UILabel?
    @IBOutlet var reminderTimeAgoLabel: UILabel?
    @IBOutlet var reminderArrivalTimeLabel: UILabel?

    var reminder: IReminder? {
        didSet {
            guard let reminder = reminder else { return }
            
            self.reminderTitleLabel?.text = TextFormatterUtil.getReminderText(reminder)
            self.reminderSubTitleLabel?.text = TextFormatterUtil.getTriggerText(reminder.getTrigger())
            self.reminderArrivalTimeLabel?.text = ""
            self.reminderImageView?.image = UIImage(named: self.imageForReminder(reminder))
            
            self.updateTimeAgoLabel()
        }
    }
    
    var beEvent: BeEvent? {
        didSet {
            guard let beEvent = beEvent else { return }
            
            self.reminderTitleLabel?.text = String(format: NSLocalizedString("be_at", comment: ""), beEvent.getLocation().getName())
            self.reminderSubTitleLabel?.text = TextFormatterUtil.getReminderDateString(beEvent.getArrivalTime())
            self.reminderArrivalTimeLabel?.text = TimeInString.relativeDateTimeString(NSDate(long: (beEvent.getArrivalTime())), now: NSDate())
            self.reminderImageView?.image = UIImage(named: "list_be");
            
            self.updateTimeAgoLabel()
        }
    }
    
    func updateTimeAgoLabel() {
        
        func agoTextFromLongDate(longDate : Int64) -> String {
            return NSDate(long: longDate).timeAgoInWords() + " ago" //TODO: support localization
        }
        
        func longDateToShow() -> Int64? {
            if reminder != nil {
                return reminder?.getAddedTime()
            } else if beEvent != nil {
                return beEvent?.getCreationTime()
            }
            else {
                return nil
            }
        }
        
        let agoLongDate = longDateToShow()
        self.reminderTimeAgoLabel?.text = agoLongDate != nil ? agoTextFromLongDate(agoLongDate!) : ""
    }
    
    func imageForReminder(reminder: IReminder) -> String
    {
        var reminderImageName: String?
        
        switch (reminder.getReminderType()){
            
        case ReminderType.DO():
            reminderImageName = "list_do";
            break;
        case ReminderType.CALL():
            reminderImageName = "list_call"
            break;
        case ReminderType.NOTIFY():
            reminderImageName = "list_notif";
            break;
        default:
            print("unsupported reminder type");
            reminderImageName = "";
        }
        return reminderImageName!;
    }
    
    //MARK:
    
    override func prepareForReuse() {
        reminder = nil
        beEvent = nil
    }
}