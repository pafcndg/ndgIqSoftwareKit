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
        
        /*
        var loading     = MBProgressHUD()
        loading.mode    = MBProgressHUDMode.Determinate
        
        let currentVersion = firmware.deviceFirmware()
        
        let message1 = "Current installed firmware: \(currentVersion). \n\n Check if there's any update?"
        
        showAlert("Firmware", message: message1, ok: "Yes", cancel: "No", cancelAction: { (UIAlertAction) -> () in
            
            }, okAction: { [weak self] (UIAlertAction) -> () in
            
                self?.firmware.latestCloudFirmwareInfo({firmware in
                    
                    dispatch_async(dispatch_get_main_queue()) {
                    
                        let version = firmware.version.componentsSeparatedByString(".")
                        
                        let v = currentVersion.substringWithRange(Range(start: (currentVersion.endIndex.advancedBy(-4)), end: currentVersion.endIndex))
                        
                        if version.last == v {
                            self?.showAlert("Firmware", message: "You have the latest version.", ok: "Yes", cancel: "No", cancelAction: nil, okAction: nil, completion: nil)
                        }
                        else {
                            self?.showAlert("Firmware", message: "There's a new version (\(firmware.version)). Do you want to update your device?", ok: "Yes", cancel: "No", cancelAction: nil, okAction: { (UIAlertAction) -> () in
                                
                                self?.firmware.downloadFirmware( { success in
                                    
                                    DLog("Downloading ...")
                                    
                                   loading           = MBProgressHUD.showHUDAddedTo(self!.view, animated: true)
                                   loading.labelText = "Downloading started ..."
                                    
                                    }, failure: {failure in
                                        
                                        DLog("Failed to download:  \(failure.getCloudError())")
                                        
                                        loading.labelText = "Failed to download:  \(failure.getCloudError())"
                                        
                                    }, progress: {progress in
                                        
                                        DLog("Download Percent complete: \(progress)%")
                                        
                                        loading.progress = Float(progress)
                                        
                                    }, completed: {completed in
                                        
                                        DLog("Completed download: \(completed.getFileName()))")
                                        
                                        loading.hide(true)
                                })
                                
                            }, completion: nil)
                        }
                    }//dispatch_async
                })
            }, completion: nil)
            */
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
        
        print("Disconnect")
        
        GlobalStorage.controller?.disconnectFromController(nil)
    }
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        DLog("\(NSStringFromClass(self.classForCoder)) loaded")
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        userEvents.unsubscribe()
        
        super.viewWillDisappear(animated)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        // Start observing
        NSNotificationCenter.defaultCenter().addObserver(self, selector: Selector("setConnectionState:"), name: Constants.ConnectionStateNotification, object: nil)
        
        connect()
        
        setupNavBar()
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
        self.navigationItem.customize("Thunder 7", target: self, action: Selector("back:"))
    }
}
