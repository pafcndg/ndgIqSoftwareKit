//
//  MockNotifications.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 22/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import IQCore


typealias completionBlock = () -> ()

private let delayTime = dispatch_time(DISPATCH_TIME_NOW, Int64(5 * Double(NSEC_PER_SEC)))

// FIXME: Detected several bugs(IQCore/DSW):
//        1. Repetition count is irrelevant when we pass just one pattern
//        2. When passing more than one pattern it runs the notification repetitionCount + 1
public final class SendVibrationNotification {
    
    init (controller: ControllerManager?, completion: completionBlock?) {
            
        // Configure pattern duration
        var durations = durationPatterns()
        
        let duration1 = NotificationDurationPattern(on: 0.8, off: 0.3)
        
        if duration1.isValid {
            
            durations.append(duration1)
            
        }
        
        let duration2 = NotificationDurationPattern(on: 0.4, off: 0.7)
        
        if duration2.isValid {
            
            durations.append(duration2)
            
        }
        
        assert( controller != nil, "Controller not initiated")
        
        let notification = NotificationManager(controller: controller!.controller!, durations: durations, colors: nil)
        
        do {
            // Configure vibration pattern
            let vibrationPattern = try notification.vibraPattern(.Square, amplitude: 128, repetitionCount: 2)
            
            // Send notification
            try notification.sendNotification(vibrationPattern, led: nil, completion: completion)
        }
        catch NotificationVibrationPatternError.NotificationVibrationPatternInvalid {
            DLog("Invalid vibration pattern")
        }
        catch NotificationError.NotificationInvalid {
            DLog("Invalid notification")
        }
        catch {
            ALog("Something went wrong")
        }
    }
}

// FIXME: Detected several bugs(IQCore/DSW):
//        1. Repetition count is irrelevant when we pass just one pattern
//        2. It does not work with 2 or more color patterns
final class SendLedNotification {
    
    init (controller: ControllerManager?, completion: completionBlock?) {
        
        // Configure pattern duration
        var durations = durationPatterns()
        
        let duration1 = NotificationDurationPattern(on: 0.8, off: 0.3)
        
        if duration1.isValid {
            
            durations.append(duration1)
            
        }
        
        let duration2 = NotificationDurationPattern(on: 0.8, off: 0.3)
        
        if duration2.isValid {
            
            durations.append(duration2)
            
        }
        
        
        // Configure pattern colors
        var colors = colorPatterns()
        
        let color1 = UIColor(hex: 0x0000ff)
        
        colors.append(color1)
        
        let color2 = UIColor(hex: 0xff0000)
        
        colors.append(color2)
        
        
        assert( controller != nil, "Controller not initiated")
        
        let notification = NotificationManager(controller: controller!.controller!, durations: durations, colors: colors)
        
        
        do {
            // Configure led pattern
            let ledPattern = try notification.ledPattern(.LEDBlink, intensity: 150, repetitionCount: 2)
            
            // Send notification
            try notification.sendNotification(nil, led: ledPattern, completion: completion)
        }
        catch NotificationLedPatternError.NotificationLedPatternInvalid {
            DLog("Invalid vibration pattern")
        }
        catch NotificationError.NotificationInvalid {
            DLog("Invalid notification")
        }
        catch {
            ALog("Something went wrong")
        }
    }
}

final class SendVibrationLedNotification {
    
    init (controller: ControllerManager?, completion: completionBlock?) {
        
        // Configure pattern duration
        var durations = durationPatterns()
        
        let duration1 = NotificationDurationPattern(on: 0.8, off: 0.3)
        
        if duration1.isValid {
            
            durations.append(duration1)
            
        }
        
        let duration2 = NotificationDurationPattern(on: 0.3, off: 0.8)
        
        if duration2.isValid {
            
            durations.append(duration2)
            
        }
        
        
        // Configure pattern colors
        var colors = colorPatterns()
        
        let color1 = UIColor(hex: 0x0000ff)
        
        colors.append(color1)
        
        let color2 = UIColor(hex: 0xff0000)
        
        colors.append(color2)
        
        
        assert( controller != nil, "Controller not initiated")
        
        let notification = NotificationManager(controller: controller!.controller!, durations: durations, colors: colors)
        
        
        do {
            // Configure led pattern
            let ledPattern = try notification.ledPattern(.LEDBlink, intensity: 150, repetitionCount: 2)
            
            // Configure vibration pattern
            let vibrationPattern = try notification.vibraPattern(.Square, amplitude: 128, repetitionCount: 2)
            
            // Send notification
            try notification.sendNotification(vibrationPattern, led: ledPattern, completion: completion)
        }
        catch NotificationLedPatternError.NotificationLedPatternInvalid {
            DLog("Invalid vibration pattern")
        }
        catch NotificationError.NotificationInvalid {
            DLog("Invalid notification")
        }
        catch {
            ALog("Something went wrong")
        }
    }
}