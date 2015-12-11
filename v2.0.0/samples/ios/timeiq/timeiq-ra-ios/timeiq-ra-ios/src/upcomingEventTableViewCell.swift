//
//  upcomingEventTableViewCell.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 23/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TSO

class upcomingEventTableViewCell: UITableViewCell {

    @IBOutlet var eventImageView: UIImageView?
    @IBOutlet var eventTitleLabel: UILabel?
    @IBOutlet var eventSubTitleLabel: UILabel?
    @IBOutlet var fromLabel: UILabel?
    @IBOutlet var toLabel: UILabel?
    @IBOutlet var ttlLabel: UILabel?
    var timer: NSTimer?
    
    var startTTLTimer = false
    var event: IEvent! {
        willSet(newEvent) {
            
            self.eventTitleLabel?.text = newEvent?.getSubject()
            self.eventSubTitleLabel?.text = newEvent?.getLocation().getAddress()
            self.eventImageView?.image = UIImage(named: self.imageForEvent(newEvent!));
            
            let from = newEvent!.getArrivalTime();
            let to = from + newEvent!.getDuration();
            self.fromLabel?.text = TextFormatterUtil.getHourAsString(from)
            self.toLabel?.text = TextFormatterUtil.getHourAsString(to)
            self.ttlLabel?.text = NSLocalizedString("ttl_calculating", comment: "")
            
            if (timer == nil)
            {
                timer = NSTimer.scheduledTimerWithTimeInterval(10.0, target: self, selector: "getTTL", userInfo: nil, repeats: true)
            }
        }
    }
    
    func getTTL() {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0)) {
            TimeIQEventsUtils.getTtlOrEtaString(self.event) { (ttlString: String) -> () in
                dispatch_async(dispatch_get_main_queue()) {
                    self.ttlLabel?.text = ttlString
                }
            }
        }
    }
    
    override func awakeFromNib() {
        super.awakeFromNib();
    }
    
    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated);
    }
    
    func imageForEvent(event: IEvent) -> String
    {
        var reminderImageName: String?
        
        switch (event.getEventType()){
            
        case TSOEventType.CALENDAR():
            reminderImageName = "list_meeting";
            break;
        case TSOEventType.BE():
            reminderImageName = "list_event"
            break;
        default:
            print("unsupported reminder type");
            reminderImageName = "";
        }
        return reminderImageName!;
        
    }
}
