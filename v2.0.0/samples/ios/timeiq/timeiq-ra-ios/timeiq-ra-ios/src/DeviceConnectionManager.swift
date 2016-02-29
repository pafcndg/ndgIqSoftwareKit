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
    
    static let sharedInstance = DeviceConnectionManager()
    
    private var controller: WearableControllerType?
    private var notificationController: NotificationControllerType?
    private var bleScanner: WearableScannerType!
    private var selectedToken: WearableToken!
    private var registrationTokenType: RegistrationTokenType!
    private var updatesArray: [(connected: Bool)->()] = []
    private var userEventsArray: [(event: WearableUserEvent) -> ()] = []
    private var userEventController: UserEventsControllerType?
    private var subscribedToUserEvents = false
    
    override init() {
        if (bleScanner == nil) {
            bleScanner = WearableScanner.scannerForProtocol(.BLE)
        }
    }
    /********** SCAN **********/
    func scan(addToken: (newToken: WearableToken) -> (), err: (e: NSError) -> ()?, finish: () -> ()?) -> [WearableToken]{
        if isScanning() {
            stopScan()
        }
        var foundTokens = [WearableToken]()//[WearableToken]()
        
        bleScanner.startScan({ (tokenRes: WearableScannerTokenResult) -> () in
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
        bleScanner.stopScan()
    }
    func isScanning() -> Bool {
        return bleScanner.isScanning
    }
    /******** PAIRING ********/
    func pair(token: WearableToken) {
        selectedToken = token
        
        controller = WearableController.controllerForToken(selectedToken)
        
        //Adding connection listener
        registrationTokenType = controller?.addConnectionStatusObserver({ (wearableConnectStatus: WearableConnectStatus) -> () in
            var connected = false
            if self.controller?.connectionStatus == IQCore.WearableConnectStatus.Connected {
                connected = true
            }
            for curr_rec in self.updatesArray {
                curr_rec(connected: connected)
            }
        })
    }
    func registerForConnectionUpdates(codeToRun: (connected: Bool) -> ()) {
        updatesArray.append(codeToRun)
    }
    /******* CONNECTION *******/
    func connectToController(err: (e: NSError) -> ()?, success: () -> ()?) {
        controller?.connect({ (wearableToken, error) -> () in
            if error == nil {
                self.subscribeForUserEvents()
                success()
            } else {
                err(e: error!)
            }
        })
    }
    func disconnect() {
        controller?.disconnect()
        registrationTokenType.unsubscribe()
        unSubscribeFromUserEvents()
    }
    /******* NOTIFICATIONS *******/
    func sendNotification() {
        
        if (controller != nil && isConnected()) {
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
                if (notificationController == nil) {
                    notificationController = NotificationController.controllerForWearable(controller!)
                }
                notificationController!.sendNotification(notification, success: { () -> Void in
                    Log.verbose?.message("Sent Notification: \(notification)")
                    }, failure: { (error) -> Void in
                        print(error)
                })
            }
        }
    }
    func isConnected() -> Bool {
        if controller?.connectionStatus == IQCore.WearableConnectStatus.Connected {
            return true
        }
        return false
    }
    func getToken() -> WearableToken {
        return selectedToken
    }
    private func subscribeForUserEvents() {
        userEventController = UserEventsController.controllerForWearable(controller!)
        
        userEventController?.subscribe({ [weak self] () -> Void in
            self!.subscribedToUserEvents = true
            }, failure: { [weak self] (error) -> Void in
                self!.subscribedToUserEvents = false
            }) { [weak self] (event) -> () in
                self?.pushNewUserEvent(event)
        }
    }
    private func unSubscribeFromUserEvents() {
        subscribedToUserEvents = false
        userEventController?.unsubscribe()
    }
    func registerForUserEvents(pushEvent: (event: WearableUserEvent) -> ()) {
        userEventsArray.append(pushEvent)
    }
    private func pushNewUserEvent(event: WearableUserEvent) {
        for curr_rec in userEventsArray {
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
