//
//  NSDate+Utils.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 28/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit

extension NSDate {
    class func dateByAddingDays(days: Int) -> NSDate {
            return self.days(days)
    }
    
    class func dateBySubstractingDays(days: Int) -> NSDate {
        return NSDate.days(-days)
    }
}

extension NSDate {
    
    class func dateSinceMidnight() -> NSDate? {
        
        let date                = NSDate.dateBySubstractingDays(1)
        let calendar            = NSCalendar.currentCalendar()
        let components          = calendar.components([.Year, .Month, .Day, .Hour, .Minute, .Second], fromDate: date)
        
        // Specify date components
        let dateComponents      = NSDateComponents()
        dateComponents.year     = components.year
        dateComponents.month    = components.month
        dateComponents.day      = components.day
        dateComponents.timeZone = NSTimeZone(name: "UTC")
        dateComponents.hour     = 0
        dateComponents.minute   = 0
        dateComponents.second   = 0
        
        // Create date from components
        let userCalendar    = NSCalendar.currentCalendar() 
        let dateTime        = userCalendar.dateFromComponents(dateComponents)
        
        return dateTime
    }
    
    class func nowUTC() -> NSDate? {
        
        let date                = NSDate()
        let calendar            = NSCalendar.currentCalendar()
        let components          = calendar.components([.Year, .Month, .Day, .Hour, .Minute, .Second], fromDate: date)
        
        // Specify date components
        let dateComponents      = NSDateComponents()
        dateComponents.year     = components.year
        dateComponents.month    = components.month
        dateComponents.day      = components.day
        dateComponents.timeZone = NSTimeZone(name: "UTC")
        dateComponents.hour     = components.hour
        dateComponents.minute   = components.minute
        dateComponents.second   = components.second
        
        // Create date from components
        let userCalendar    = NSCalendar.currentCalendar()
        let dateTime        = userCalendar.dateFromComponents(dateComponents)
        
        return dateTime
    }
}

extension NSDate {
    class func days(days:Int) -> NSDate {
        return NSCalendar.currentCalendar().dateByAddingUnit( [.Day], value: days, toDate: NSDate(), options: [] )!
    }
}

extension NSDate {
    
    class func currentDate(dateFormat: String) -> String {
        
        let dateFormatter           = NSDateFormatter()
        dateFormatter.dateFormat    = dateFormat
        dateFormatter.timeZone      = NSTimeZone.defaultTimeZone()
        
        let currentDate = NSDate()
        
        return dateFormatter.stringFromDate(currentDate);
    }
}