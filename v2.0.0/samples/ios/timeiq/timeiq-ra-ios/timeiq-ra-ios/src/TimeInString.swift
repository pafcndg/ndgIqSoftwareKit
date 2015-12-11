//
//  TimeInString.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 17/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

let SECOND_IN_MILLIS: Double = 1000;
let MINUTE_IN_MILLIS: Double = SECOND_IN_MILLIS * 60;
let HOUR_IN_MILLIS: Double = MINUTE_IN_MILLIS * 60;
let DAY_IN_MILLIS: Double = HOUR_IN_MILLIS * 24;
let WEEK_IN_MILLIS: Double = DAY_IN_MILLIS * 7;
let minResolution: Double = MINUTE_IN_MILLIS;

class TimeInString {
    
    class func formatDate(date: NSDate, format: String) -> String
    {
        let formatter = NSDateFormatter.init()
        
        if (!format.isEmpty)
        {
            formatter.dateFormat = format
        }
        else
        {
            formatter.dateFormat = "dd-MM-yyyy"
        }
        
        return formatter.stringFromDate(date)
    }
    
    class func relativeDateTimeString(time: NSDate, now: NSDate) -> String {
        let dNowMillis = now.timeIntervalSince1970 * SECOND_IN_MILLIS;
        let dTimeMillis = time.timeIntervalSince1970 * SECOND_IN_MILLIS;
        let diff = dNowMillis - dTimeMillis;
        
        let past = (diff >= 0);
        let duration = fabs(Double(diff));
        var count: Double
        var resId: String?
        
        if (duration < MINUTE_IN_MILLIS && minResolution < MINUTE_IN_MILLIS) {
            count = duration / SECOND_IN_MILLIS;
            if (past) {
                resId = "date.num_seconds_ago";
            } else {
                resId = "date.in_num_seconds";
            }
        } else if (duration < HOUR_IN_MILLIS && minResolution < HOUR_IN_MILLIS) {
            count = duration / MINUTE_IN_MILLIS;
            if (past) {
                resId = "date.num_minutes_ago";
            } else {
                resId = "date.in_num_minutes";
            }
        } else if (duration < DAY_IN_MILLIS && minResolution < DAY_IN_MILLIS) {
            count = duration / HOUR_IN_MILLIS;
            if (past) {
                resId = "date.num_hours_ago";
            } else {
                resId = "date.in_num_hours";
            }
        } else if (duration < WEEK_IN_MILLIS && minResolution < WEEK_IN_MILLIS) {
            count = duration / DAY_IN_MILLIS;
            if (past) {
                resId = "date.num_days_ago";
            } else {
                resId = "date.in_num_days";
            }
        } else {
            return formatDate(time, format: "dd MMM yyyy"); //date format can be customized
        }
        
        if (count == 1)
        {
            resId = resId! + "-one";
        }
        else
        {
            resId = resId! + "-other";
        }
        
        return String(format: NSLocalizedString(resId!, comment: ""), count);
    }
    
}