//
//  DeviceConnectionManager.swift
//  SensorRefApp2
//
//  Created by Gal Shirin on 19/11/2015.
//  Copyright © 2015 fiat.bender. All rights reserved.
//

import UIKit
import IQCore

class DeviceConnectionManager: NSObject {
    
    private static var controller: WearableControllerType?
    private static var notificationController: NotificationControllerType?
    private static var bleScanner: WearableScannerType!
    private static var selectedToken: WearableToken!
    private static var registrationTokenType: RegistrationTokenType!
    private static var updatesArray: [(connected: Bool)->()] = []
    private static var userEventsArray: [(event: WearableUserEvent) -> ()] = []
    private static var userEventController: UserEventsControllerType?
    private static var subscribedToUserEvents = false
    
    override init() {
        if (DeviceConnectionManager.bleScanner == nil) {
            DeviceConnectionManager.bleScanner = WearableScanner.scannerForProtocol(.BLE)
        }
    }
    /********** SCAN **********/
    func scan(addToken: (newToken: WearableToken) -> (), err: (e: NSError) -> ()?, finish: () -> ()?) -> [WearableToken]{
        if isScanning() {
            stopScan()
        }
        var foundTokens = [WearableToken]()//[WearableToken]()
        
        DeviceConnectionManager.bleScanner.startScan({ (tokenRes: WearableScannerTokenResult) -> () in
            dispatch_async(dispatch_get_main_queue()) { () -> Void in
                foundTokens.append(tokenRes.token)
                addToken(newToken: tokenRes.token)
            }
            }, error: { (error) -> () in
                self.stopScan()
                Log.warning?.message("\(error)")
                err(e: error)
            }) { () -> () in
                self.stopScan()
                Log.info?.message("finished")
                finish()
        }
        return foundTokens
    }
    func stopScan() {
        DeviceConnectionManager.bleScanner.stopScan()
    }
    func isScanning() -> Bool {
        return DeviceConnectionManager.bleScanner.isScanning
    }
    /******** PAIRING ********/
    func pair(token: WearableToken) {
        DeviceConnectionManager.selectedToken = token
        
        DeviceConnectionManager.controller = WearableController.controllerForToken(DeviceConnectionManager.selectedToken)
        
        //Adding connection listener
        DeviceConnectionManager.registrationTokenType = DeviceConnectionManager.controller?.addConnectionStatusObserver({ (wearableConnectStatus: WearableConnectStatus) -> () in
            var connected = false
            if DeviceConnectionManager.controller?.connectionStatus == IQCore.WearableConnectStatus.Connected {
                connected = true
            }
            for curr_rec in DeviceConnectionManager.updatesArray {
                curr_rec(connected: connected)
            }
        })
    }
    func registerForConnectionUpdates(codeToRun: (connected: Bool) -> ()) {
        DeviceConnectionManager.updatesArray.append(codeToRun)
    }
    /******* CONNECTION *******/
    func connectToController(err: (e: NSError) -> ()?, success: () -> ()?) {
        DeviceConnectionManager.controller?.connect({ (wearableToken, error) -> () in
            if error == nil {
                self.subscribeForUserEvents()
                success()
            } else {
                err(e: error!)
            }
        })
    }
    func disconnect() {
        DeviceConnectionManager.controller?.disconnect()
        DeviceConnectionManager.registrationTokenType.unsubscribe()
        unSubscribeFromUserEvents()
    }
    /******* NOTIFICATIONS *******/
    func sendNotification() {
        
        if (DeviceConnectionManager.controller != nil && isConnected()) {
            //Vibration pattern
            let vibra = NotificationVibrationPattern(type: .Square)
            vibra.amplitude = 127
            vibra.repititionCount = Int(1)
            
            //Led pattern
            let leds = NotificationLedPattern(type: .LEDBlink)
            leds.intensity = UInt32(123)
            leds.repetitionCount = UInt32(1)
            for (var i:Int = 0; i < 2; i++) {
                let durations = NotificationDurationPattern(
                    on: NSTimeInterval(0.5), // NSTimeInterval is typically in seconds
                    off: NSTimeInterval(0.5)
                )
                vibra.durationPatterns.append(durations)
                leds.durationPatterns.append(durations)
                leds.colors.append(UIColor.blueColor())
            }
            
            let notification: WearableNotification = WearableNotification(ledPattern: leds, vibrationPattern: vibra, delay: 0)
            
            if notification.isValid {
                // TODO: add some loopback / completion functionality
                if (DeviceConnectionManager.notificationController == nil) {
                    DeviceConnectionManager.notificationController = NotificationController.controllerForWearable(DeviceConnectionManager.controller!)
                }
                DeviceConnectionManager.notificationController!.sendNotification(notification, success: { () -> Void in
                    Log.verbose?.message("Sent Notification: \(notification)")
                    }, failure: { (error) -> Void in
                        print(error)
                })
            }
        }
    }
    func isConnected() -> Bool {
        if DeviceConnectionManager.controller?.connectionStatus == IQCore.WearableConnectStatus.Connected {
            return true
        }
        return false
    }
    func getToken() -> WearableToken {
        return DeviceConnectionManager.selectedToken
    }
    private func subscribeForUserEvents() {
        DeviceConnectionManager.userEventController = UserEventsController.controllerForWearable(DeviceConnectionManager.controller!)
        
        DeviceConnectionManager.userEventController?.subscribe({ [weak self] () -> Void in
            DeviceConnectionManager.subscribedToUserEvents = true
            }, failure: { [weak self] (error) -> Void in
                DeviceConnectionManager.subscribedToUserEvents = false
            }) { [weak self] (event) -> () in
                self?.pushNewUserEvent(event)
        }
    }
    private func unSubscribeFromUserEvents() {
        DeviceConnectionManager.subscribedToUserEvents = false
        DeviceConnectionManager.userEventController?.unsubscribe()
    }
    func registerForUserEvents(pushEvent: (event: WearableUserEvent) -> ()) {
        DeviceConnectionManager.userEventsArray.append(pushEvent)
    }
    private func pushNewUserEvent(event: WearableUserEvent) {
        for curr_rec in DeviceConnectionManager.userEventsArray {
            curr_rec(event: event)
        }
    }
    func showAlertView(strTitle:String, strBody:String)
    {
        let alert = UIAlertView()
        alert.title = strTitle
        alert.message = strBody
        alert.addButtonWithTitle("Dismiss")
        alert.show()
    }
}
