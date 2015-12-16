//
//  DeviceViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 10/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class DeviceViewController: UIViewController {

    // MARK: Members
    lazy var userEvents     = UserEventsManager.sharedInstance
    lazy var systemEvents   = SystemEventsManager.sharedInstance
    
    // MARK: UI
    @IBOutlet weak var connectDisconnectLb: UILabel!
    
    
    // MARK: Buttons
    
    @IBAction func DeviceDetailsButtonTap() {
        
        performSegueWithIdentifier(Constants.DeviceToInfo, sender: nil)
    }
    
    @IBAction func NotificationsButtonTap() {
        
        print("Go to notifications")
        
        // Send notification
        let _ = SendVibrationLedNotification(controller: GlobalStorage.controller, completion: { () -> () in
            
            print("Notification Sent")
        })
    }
    
    @IBAction func FirmwareUpdatesButtonTap() {
        
        print("Go to firmware updates")
        
        performSegueWithIdentifier(Constants.DeviceToFirmware, sender: nil)
    }
    
    @IBAction func UserEventsButtonTap() {
        
        userEvents.isSuscribed ? userEvents.unsubscribe( { [weak self] success in
        
                dispatch_async(dispatch_get_main_queue()) {
                    self?.presentDefaultAlert("Unsubscribed from user events", subText: nil, dismissText: "ok".localized)
                }
            }) : userEvents.subscribe( { [weak self] success in
            
                dispatch_async(dispatch_get_main_queue()) {
                    self?.presentDefaultAlert("Subscribed to user events", subText: nil, dismissText: "ok".localized)
                }
            }, failure: { [weak self] failure in
                
                dispatch_async(dispatch_get_main_queue()) {
                    self?.presentDefaultAlert("Error subscribing to user events: \(failure.localizedDescription)", subText: nil, dismissText: "ok".localized)
                }
            }, event: { [weak self] event in
        
                dispatch_async(dispatch_get_main_queue()) {
                    self?.presentDefaultAlert(UserEventsManager.gestureType(event), subText: nil, dismissText: "ok".localized)
                }
        })
    }
    
    @IBAction func DeviceEventsButtonTap() {
        
        systemEvents.isSuscribed ? systemEvents.unsubscribe( { [weak self] success in
            
            dispatch_async(dispatch_get_main_queue()) {
                self?.presentDefaultAlert("Unsubscribed from system events", subText: nil, dismissText: "ok".localized)
            }
            }) : systemEvents.subscribe( { [weak self] success in
                
                dispatch_async(dispatch_get_main_queue()) {
                    self?.presentDefaultAlert("Subscribed to system events", subText: nil, dismissText: "ok".localized)
                }
                }, failure: { [weak self] failure in
                    
                    dispatch_async(dispatch_get_main_queue()) {
                        self?.presentDefaultAlert("Error subscribing to system events: \(failure.localizedDescription)", subText: nil, dismissText: "ok".localized)
                    }
                }, receiver: { [weak self] event in
                    
                    dispatch_async(dispatch_get_main_queue()) {
                        self?.presentDefaultAlert(SystemEventsManager.systemType(event), subText: nil, dismissText: "ok".localized)
                    }
                })
    }
    
    @IBAction func DisconnectButtonTap() {
        
        if GlobalStorage.isDeviceConnected {
            
            dispatch_async(dispatch_get_main_queue()) {
                
                self.connectDisconnectLb.text = "conn".localized
                
                GlobalStorage.controller?.disconnectFromController(nil)
                
                NSNotificationCenter.defaultCenter().postNotificationName(Constants.ConnectionStateNotification, object: self, userInfo: [Constants.IsConnectionState:false])
            }
        }
        else {
            
            dispatch_async(dispatch_get_main_queue()) {
                
                self.connectDisconnectLb.text = "dis".localized
                
                if let controller = GlobalStorage.controller?.controller {
                    
                    controller.connect({(wearableToken, error) -> () in
                        
                        if let err = error {
                            
                             DLog("\(err)")
                        }
                        else {
                            NSNotificationCenter.defaultCenter().postNotificationName(Constants.ConnectionStateNotification, object: self, userInfo: [Constants.IsConnectionState:true])
                        }
                    })
                }
            }
        }
    }
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        DLog("\(NSStringFromClass(self.classForCoder)) loaded")
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        // Start observing
        NSNotificationCenter.defaultCenter().addObserver(self, selector: Selector("setConnectionState:"), name: Constants.ConnectionStateNotification, object: nil)
        
        setConnectionStatusIndicator()
        
        setupNavBar()
        
        self.connectDisconnectLb.text = GlobalStorage.isDeviceConnected ? "dis".localized : "conn".localized
    }
    
    override func viewWillDisappear(animated: Bool) {
    
        super.viewWillDisappear(animated)
        
        userEvents.unsubscribe()
        systemEvents.unsubscribe()
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
        
        // Stop observing
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }
    
    
    // MARK: Back
    
    @objc private func back(sender: AnyObject) {
        self.navigationController?.popViewControllerAnimated(true)
    }
    
    
    // MARK: Setup
    
    private func setupNavBar() {
        
        self.navigationController?.setNavigationBarHidden(false, animated: true)
        self.navigationItem.customize(GlobalStorage.token?.displayName ?? "", target: self, action: Selector("back:"))
    }
}
