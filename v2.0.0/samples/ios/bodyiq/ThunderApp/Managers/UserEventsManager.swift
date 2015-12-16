//
//  UserEventsManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 15/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import IQCore

typealias UserEventSubscriptionSuccess      = () -> ()
typealias UserEventUnsubscriptionSuccess    = () -> ()
typealias UserEventSubscriptionError        = (NSError) -> ()

class UserEventsManager {
    
    // MARK: Members
    
    static let sharedInstance = UserEventsManager()
    
    private var userEventController: UserEventsControllerType?
    
    private(set) var isSuscribed = false
    
    
    // MARK: Init / Deinit
    
    private init() {
        
        if let controller = GlobalStorage.controller?.controller {
            
            userEventController = UserEventsController.controllerForWearable(controller)
        }
        else {
            fatalError("WearableController should be already initiated at this point")
        }
    }
    
    deinit {
        
        unsubscribe()
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    
    // MARK: Subscribe / Unsubscribe
    
    func subscribe(success: UserEventSubscriptionSuccess, failure: UserEventSubscriptionError, event: UserEventReceived) {
        
        if !isSuscribed {
            
            isSuscribed = true
            
            userEventController?.subscribe(success, failure: failure, receiver: event)
        }
    }
    
    func unsubscribe(success: UserEventUnsubscriptionSuccess? = nil) {
        
        if isSuscribed {
         
            isSuscribed = false
            
            userEventController?.unsubscribe()
                        
            success?()
        }
    }
    
    
    // MARK: Helper
    
    class func gestureType(event: WearableUserEvent) -> String {
        
        var eventType: String = "Unknown Gesture"
        
        if let gesture = event.gestureEvent {
            
            switch gesture.type {
                case .DoubleTap:
                    eventType = "Double Tap"
                case .TripleTap:
                    eventType = "Triple Tap"
                default:
                    eventType = "Unknown Gesture Event"
            }
            
        }
        else if let tap = event.buttonEvent {
            
            switch tap.type {
                case .SinglePress:
                    eventType = "Single Press"
                case .DoublePress:
                    eventType = "Double Press"
                case .LongPress:
                    eventType = "Long Press"
                case .TriplePress:
                    eventType = "Triple Press"
                default:
                    eventType = "Unknown Tap Event"
                }
        }
        
        return eventType
    }
}