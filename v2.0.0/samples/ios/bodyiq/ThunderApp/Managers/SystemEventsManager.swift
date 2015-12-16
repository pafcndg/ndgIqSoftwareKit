//
//  SystemEventsManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 15/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import IQCore

typealias SystemEventSubscriptionSuccess      = () -> ()
typealias SystemEventUnsubscriptionSuccess    = () -> ()
typealias SystemEventSubscriptionError        = (NSError) -> ()

class SystemEventsManager {
    
    // MARK: Members
    
    static let sharedInstance = SystemEventsManager()
    
    private var systemEventsController: SystemEventsControllerType?
    
    private(set) var isSuscribed = false
    
    
    // MARK: Init / Deinit
    
    private init() {
        
        if let controller = GlobalStorage.controller?.controller {
            
            systemEventsController = SystemEventsController.controllerForWearable(controller)
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
    
    func subscribe(success: SystemEventSubscriptionSuccess, failure: SystemEventSubscriptionError, receiver: SystemEventReceived) {
        
        if !isSuscribed {
            
            isSuscribed = true
            
           systemEventsController?.subscribe(success, failure: failure, receiver: receiver)
        }
    }
    
    func unsubscribe(success: SystemEventUnsubscriptionSuccess? = nil) {
        
        if isSuscribed {
         
            isSuscribed = false
            
            systemEventsController?.unsubscribe()
                        
            success?()
        }
    }
    
    
    // MARK: Helper
    
    class func systemType(event: WearableSystemEvent) -> String {
        
        var eventType: String = "Unknown"
        
        if event.bootEvent != nil {
            eventType = "Boot event"
        }
        else if event.shutdownEvent != nil {
            eventType = "Shutdown event"
        }
        else if event.crashEvent != nil {
            eventType = "Crash event"
        }
        else if event.lowBatteryEvent != nil {
            eventType = "Low battery event"
        }
        
        return eventType
    }
}