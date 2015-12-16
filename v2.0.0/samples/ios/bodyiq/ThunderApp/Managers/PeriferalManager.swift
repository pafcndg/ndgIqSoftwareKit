//
//  BluetoothManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import CoreBluetooth

public typealias peripheralUpdates = (state: CBPeripheralManagerState) -> ()


/// Basic interface to track the bluetooth status
class PeriferalManager:NSObject, CBPeripheralManagerDelegate {
    
    
    // MARK: Members
    
    private var peripheralManager:CBPeripheralManager?
    private var stateUpdates:peripheralUpdates?
    
    
    // MARK: Init / Deinit
    
    override init() {
        
        super.init()
        
        let queue           = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)
        
        peripheralManager   = CBPeripheralManager(delegate: self, queue: queue)
        
    }
    
    deinit {
        
        peripheralManager?.delegate = nil
        peripheralManager           = nil
        stateUpdates                = nil
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
        
    }    
    
    
    // MARK: API
    
    /**
    Check if Bluetooth is currently powered on and available to use
    
    - returns: true if available, false otherwise
    */
    func isEnabled() -> Bool {
        
        return peripheralManager?.state == .PoweredOn
        
    }
    
    /**
     Subscribe to this function to receive bluetooth status updates
     
     - parameter state: CBPeripheralManagerState
     */
    func stateUpdate(state: peripheralUpdates) {
        
        stateUpdates = state
        
    }
    
    
    // MARK: Delegate Methods
    
    func peripheralManagerDidStartAdvertising(peripheral: CBPeripheralManager, error: NSError?) {
        
    }
    
    func peripheralManagerDidUpdateState(peripheral: CBPeripheralManager) {
        
        stateUpdates?(state: peripheral.state)
        
    }
}