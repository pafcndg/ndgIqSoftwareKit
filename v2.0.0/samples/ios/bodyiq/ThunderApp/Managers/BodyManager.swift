//
//  BodyManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 22/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import BodyIQ
import IQCore

class BodyManager {
    
    
    // MARK: Members
    
    private(set) var isSubscribed = false
    
    private weak var controller: WearableControllerType!
    
    private var timeLaunched: NSDate?
    
    private let listenerID =  "BodyIQRefApp.BodyManager"
    
    
    // MARK: Init / Deinit
    
    init(controller: WearableControllerType) {
        
        self.controller = controller
    }
    
    deinit {
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    
    // MARK: Subscribe / Unsubscribe
    
    func subscribe(listener: BodyIQActivityListenerCallback) {
        
        if !isSubscribed {
            
            self.timeLaunched = NSDate()
            
            var err:NSError?
            
            BodyIQ.addActivityListenerForWearableController(controller,
                failure: { [weak self] error in
                    
                    err = error
                    
                    DLog(
                    "error: \(error).\n" +
                    "While observing the device id:\(self?.controller.wearableIdentity!.uuid)"
                    )
                },
                success: {
                    self.isSubscribed = true
                },
                listener: listenerID,
                callback: listener)
            
            // FIXME: Bug: Success not called although the subscription is successful 
            // Hack: No error, we assume successful subscription
            if err == nil {
                isSubscribed = true
            }
            
            /*
            BodyIQ.beginObservingWearableController(controller,
                failure: { error in DLog("\(error)") },
                success: { DLog("Begin observing...") }
            )
            */
            
        }
        else {
            
            DLog("Already subscribed to events")
        }
    }

    func unsubscribe() {
        
        if isSubscribed {
            
            //BodyIQ.endObservingWearableController(controller)
            
            BodyIQ.removeActivityListenerForWearableController(controller, listener: listenerID)
            
            isSubscribed = false
        }
        else {
            
            DLog("Already unsubscribed from events")
        }
    }

}