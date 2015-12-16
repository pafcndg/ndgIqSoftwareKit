//
//  ThunderGlobal.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 13/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit
import IQCore

struct GlobalStorage {
    
    static var token: WearableToken?                = nil
    static var deviceDescription: WearableIdentity? = nil
    static var scanner:ScannerManager?              = nil
    static var controller:ControllerManager?        = nil
    static var user:UserManager?                    = nil
    static var ts:TimeSeriesManager?                = nil
    static var isDeviceConnected:Bool               = false
    
    func destroy() {
        
        GlobalStorage.token             = nil
        GlobalStorage.deviceDescription = nil
        GlobalStorage.scanner           = nil
        GlobalStorage.controller        = nil
        GlobalStorage.user              = nil
        GlobalStorage.ts                = nil
    }
}