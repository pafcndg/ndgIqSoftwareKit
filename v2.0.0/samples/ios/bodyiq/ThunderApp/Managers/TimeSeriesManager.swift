//
//  TimeSeriesManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 25/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import BodyIQ
import IQCore

typealias timeSeriesArray = [TimeSeries]
typealias timeSeriesData  = (timeSeriesArray) -> ()

struct Activity {
    
    var distance:String
    var duration:String
    var avgSpeed:String
    var longestInactivity:String
    var steps:Int
    var calories:Double
}

class TimeSeriesManager {
    
    
    // MARK: Members
    
    var startDate: NSDate!
    var endDate: NSDate!
    

    // MARK: Init / Deinit
    
    init(startDate: NSDate, endDate: NSDate) {
        
        self.startDate = startDate
        self.endDate   = endDate
    }
    
    deinit {
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    
    // MARK: Time Series Query
    
    func timeSeriesQuery(data: timeSeriesData?) {
        
        BodyIQ.timeSeriesFromDate(startDate, endDate:  endDate) { error, timeSeriesArray in
            
            if error != nil {
                DLog("TS Error: \(error)")
            }
            else {
                if let d = timeSeriesArray {
                    data!(d)
                }
            }
        }
    }
    
    func extractTimeSeriesData(data: timeSeriesArray) -> Activity {
        
        // TODO: update this for several devices
        
        print("Data has \(data.count) elements")
        
        var _distance:Double                    = 0
        var _duration:Double                    = 0
        var _avgSpeed:Double                    = 0
        var _longestInactivity:NSTimeInterval   = 0
        var _steps:Int                          = 0
        var _calories:Double                    = 0
        var _oldDate:NSDate?                    = nil
        
        // Iterate through time series array
        for ts in data {
            
            print("************************** \n" +
                  "Info: \n" +
                  "deviceId: \(ts.info.deviceID) \n" +
                  "companionId: \(ts.info.companionID) \n" +
                  "observations: \(ts.observations.count) \n" +
                  "************************** \n")
            
            // Iterate through each time series observations array
            for observation in ts.observations {
                
                print("-------------------------- \n")
                print("Observation: \n")
                print("Lat: \(observation.location?.lat), Lon: \(observation.location?.lon) \n")
                print("Timestamp: \(observation.timestamp) \n")
                print("Source: \(observation.source) \n")
                print("SessionId: \(observation.value.sessionID) \n")
                
                
                if let value = observation.value as? BodyIQWalkingTimeSeriesValue {
                    
                    _distance += value.distance
                    _duration += value.duration
                    _steps    += value.stepCount
                    _calories += value.calories
                    
                    if _oldDate == nil {
                        _oldDate = observation.timestamp
                    }
                    else {
                        let newTs  = _oldDate!.timeIntervalSinceDate(observation.timestamp) * -1
                        
                        print("Inactivity: \(newTs)")
                        
                        _longestInactivity = newTs > _longestInactivity ? newTs : _longestInactivity
                    }
                    
                    print("Steps: \(value.stepCount) \n")
                    print("Calories: \(value.calories) \n")
                    print("Distance: \(value.distance) \n")
                    print("Speed: \(value.speed) \n")
                    print("Duration: \(value.duration) \n")
                }
                
                if let value = observation.value as? BodyIQHeartRateTimeSeriesValue {
                    
                    print("Beats per minute: \(value.beatsPerMinute) \n")
                }
                
                print("-------------------------- \n")
            }
            _avgSpeed = (_distance / _duration) * 0.001 * 3600
        }
        
        let (h,m,s) = TimeSeriesManager.secondsToHoursMinutesSeconds(Int(_duration))
        let (h2,m2,s2) = TimeSeriesManager.secondsToHoursMinutesSeconds(Int(_longestInactivity))

        
        return Activity(distance: "\(String(format: "%.1f m", _distance))", duration: "\(h) h, \(m) m, \(s) s", avgSpeed: "\(String(format: "%.1f km/h", _avgSpeed))", longestInactivity: "\(h2) h, \(m2) m, \(s2) s", steps: _steps, calories: _calories)
    }
    
    // MARK: Helper
    
    class func secondsToHoursMinutesSeconds (seconds : Int) -> (Int, Int, Int) {
        return (seconds / 3600, (seconds % 3600) / 60, (seconds % 3600) % 60)
    }
}