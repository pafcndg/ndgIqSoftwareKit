//
//  ControllerManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/15/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import IQCore

typealias WearableIdentityCompletion    = (deviceDescription: WearableIdentity?) -> ()
typealias WearableConnectionState       = (state: WearableConnectStatus) -> ()
typealias WearableTokenObserver         = (WearableConnectStatus) -> ()


class ControllerManager {
    
    // MARK: Members
    
    private(set) var controller: WearableControllerType?
    private(set) var isSubscribedToBatteryUpdates:Bool       = false
    
    
    // MARK: Init / Deinit
    
    // Default class initializer
    init(token: WearableControllerType, observer: WearableTokenObserver) {

        controller = token
        
        controller!.addConnectionStatusObserver(observer)
    }
    
    deinit {
     
        controller = nil
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    
    // MARK: Connect / Disconnect
    
    func connectToController(wearableConnectionState: WearableConnectionState?) {
        
        controller?.connect({ [weak self] (wearableToken, error) -> () in
            
            // FIXME: Closure called twice
            if error == nil {
                
                
            }
            else {
                
                DLog("\(error)")
            }
            
            wearableConnectionState?(state: self?.controller?.connectionStatus ?? .Disconnected)
        })
    }
    
    func disconnectFromController(wearableConnectionState: WearableConnectionState?) {
        
        controller?.disconnect()
        
        wearableConnectionState?(state: controller?.connectionStatus ?? .Disconnected)
        
    }
    
    func deviceDescription(completion: WearableIdentityCompletion?) {
        
        completion?(deviceDescription: controller?.wearableIdentity)
    }
    
    
    // MARK: Connection status
    
    func deviceConnectionStatus() -> WearableConnectStatus {
        return controller?.connectionStatus ?? .Disconnected
    }
    
    
    // MARK: Battery status
    
    func subscribeToBatteryUpdates() {
        
        controller?.subscribeToBatteryLevelUpdates({ [weak self] (batteryLevel, error) -> () in
            
            if error == nil {
                
                self?.isSubscribedToBatteryUpdates = true
                
                DLog( "Battery Status: \(batteryLevel)" )
                
            }
            else {
                
                DLog("\(error)")
            }
        })
    }
    
    func unsubscribeToBatteryUpdates() {
        
        controller?.unsubscribeToBatteryLevelUpdates()

        isSubscribedToBatteryUpdates = false
    }
}
