//
//  BaseAddReminderViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 11/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TimeIQ

protocol BaseAddReminderViewControllerDelegate {
    func didAddReminder()
    func didUpdateReminder()
}

class BaseAddReminderViewController: UIViewController {
    
    var reminderType = REMINDER_TYPE_DO
    var pageIndex : Int = 0
    var delegate:BaseAddReminderViewControllerDelegate! = nil;
    
    func didClickDone() {}
    
    init() {
        super.init(nibName:nil, bundle:nil)
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
}
