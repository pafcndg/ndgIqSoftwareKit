//
//  VibrationManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/20/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import IQCore

public typealias durationPatterns = [NotificationDurationPattern]
public typealias colorPatterns    = [UIColor]


/// Class to manage and send to the device Vibration and Led events
class NotificationManager {
    
    
    // MARK: Members
    private var durations:durationPatterns
    private var colors:colorPatterns?
    private var notificationController:NotificationControllerType?
    private weak var controller: WearableControllerType!
    
    
    // MARK: Init / Deinit
    
    /**
     Default NotificationManager initializer
     
     - parameter durations: Array of NotificationDurationPattern
     - parameter colors:    Array of UIColor
     
     - returns: NotificationManager instance
     */
    init(controller:WearableControllerType, durations: durationPatterns, colors: colorPatterns?) {
        
        self.controller = controller
        self.durations  = durations
        self.colors     = colors
        
        if colors != nil {
            
            assert( colors!.count == durations.count, "Define a duration for each color pattern")
            
        }
        
    }
    
    deinit {
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    
    // MARK: Vibration pattern
    
    /**
     Configure a vibration pattern
     
     - parameter type:            The type of vibration pattern
     - parameter amplitude:       The amplitude value for the vibration pattern
     - parameter repetitionCount: The number of repetitions for this vibration pattern
     - parameter durations:       An array of Duration Patterns
    
     - throws: Vibration pattern invalid
     
     - returns: A vibration pattern specific to WearableNotifications
     */
    func vibraPattern(type: NotificationVibrationType, amplitude: Int, repetitionCount: Int) throws -> NotificationVibrationPattern?{
        
        let pattern             = NotificationVibrationPattern(type: type)
        pattern.amplitude       = amplitude
        pattern.repititionCount = repetitionCount
        
        
        assert(durations.count > 0, "Must define at least one duration pattern")
        
        for duration in durations {
            
            let d = NotificationDurationPattern(
                on: duration.durationOn,
                off: duration.durationOff
            )
            pattern.durationPatterns.append(d)
        }
        
        guard pattern.isValid else {
            throw NotificationVibrationPatternError.NotificationVibrationPatternInvalid
        }
        
        return pattern
    }
    
    
    // MARK: Led pattern
    
    /**
    Configure a led pattern
    
    - parameter type:            The led patterns type.
    - parameter intensity:       The intensity/brightness of the leds for the pattern.
    - parameter repetitionCount: The number of repetitions for the entire pattern.
    
    - throws: Led pattern invalid
    
    - returns: A led pattern specific to WearableNotifications
    */
    func ledPattern(type: NotificationLEDPatternType, intensity: UInt32, repetitionCount: UInt32) throws -> NotificationLedPattern? {
        
        let led             = NotificationLedPattern(type: type)
        led.intensity       = intensity
        led.repetitionCount = repetitionCount
        
        
        assert(durations.count > 0, "Must define at least one duration pattern")
        
        for duration in durations {
            
            let d = NotificationDurationPattern(
                on: duration.durationOn,
                off: duration.durationOff
            )

            led.durationPatterns.append(d)
        }
        
        
        assert(colors != nil && colors?.count > 0, "Must define at least one color pattern")
        
        for c in colors! {
            
            led.colors.append(c)
        }
        
        guard led.isValid else {
            throw NotificationLedPatternError.NotificationLedPatternInvalid
        }
        
        return led
    }
    
    
    // MARK: Send Notification
    
    /**
    Send a NotificationVibrationPattern and / or a NotificationLedPattern
    
    - throws: Notification invalid
    
    - parameter vibration: NotificationVibrationPattern
    - parameter led:       NotificationLedPattern
    */
    func sendNotification(vibration: NotificationVibrationPattern?, led: NotificationLedPattern?, completion: completionBlock?) throws{
        
        var notification: WearableNotification
        
        guard vibration != nil || led != nil else {
            assertionFailure("Define at least vibration pattern or a led pattern")
            return
        }
        
        
        if vibration != nil && led != nil {
            
            notification = WearableNotification(ledPattern: led, vibrationPattern: vibration, delay: 0)
        }
        else if vibration != nil {
            
            notification = WearableNotification(vibrationPattern: vibration!, delay: 0)
        }
        else {
            
            notification = WearableNotification(ledPattern: led!, delay: 0)
        }
        
        // Send notification
        guard notification.isValid else {
            throw NotificationError.NotificationInvalid
        }
        
        notificationController = NotificationController.controllerForWearable(controller)
        
        guard notificationController != nil else {
            completion?()
            return
        }
        
        if( notificationController!.sendNotification(notification, success: { () -> Void in
            
            // FIXME: IQCore bug: These closure are never called
            DLog("Sent Notification: \(notification)")
            }, failure: { (error) -> Void in
                print(error)
            })) {
            completion?()
        }
    }
    
}