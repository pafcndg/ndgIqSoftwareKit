//
//  FirmwareManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 25/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import IQCore

class FirmwareUpdateViewController: UITableViewController {
    
    
    // MARK: UI
    
    @IBOutlet weak var installedLabel: UILabel!
    @IBOutlet weak var installedVersionLabel: UILabel!
    @IBOutlet weak var newFirmwareLabel: UILabel!
    @IBOutlet weak var newFirmwareVersionLabel: UILabel!
    @IBOutlet weak var downloadingProgressView: UIProgressView!
    @IBOutlet weak var downloadingPercentCompleteLabel: UILabel!
    @IBOutlet weak var installingLabel: UILabel!
    @IBOutlet weak var installingProgressView: UIProgressView!
    @IBOutlet weak var installingPercentCompleteLabel: UILabel!
    @IBOutlet weak var checkButton: UIButton!
    @IBOutlet weak var downloadButton: UIButton!
    @IBOutlet weak var installButton: UIButton!
    @IBOutlet weak var downloadingLabel: UILabel!
    
    
    
    // MARK: UI
    
    private lazy var firmware:FirmwareManager = FirmwareManager.sharedInstance
    
    var deviceDescription: WearableIdentity?
    var deviceFirmwareRevision: String?


    // MARK: View Life Cycle

    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        setupInitialUI()
    }

    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
        
        // Start observing
        NSNotificationCenter.defaultCenter().addObserver(self, selector: Selector("setConnectionState:"), name: Constants.ConnectionStateNotification, object: nil)
        
        setupNavBar()
        
        connect()
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
        self.navigationItem.customize("device_details".localized, target: self, action: Selector("back:"))
    }

    
    private func setupInitialUI () {
        
        addLoading(view, message: "getting_info".localized)
        
        newFirmwareLabel.hidden                 = true
        newFirmwareVersionLabel.hidden          = true

        downloadingProgressView.progress        = 0.0
        installingProgressView.progress         = 0.0
        downloadingPercentCompleteLabel.text    = "0%"
        installingPercentCompleteLabel.text     = "0%"

        downloadingLabel.hidden                 = true
        downloadingProgressView.hidden          = true
        downloadingPercentCompleteLabel.hidden  = true

        installingLabel.hidden                  = true
        installingProgressView.hidden           = true
        installingPercentCompleteLabel.hidden   = true

        checkButton.enabled                     = false
        downloadButton.enabled                  = false
        installButton.enabled                   = false

        updateDeviceDescription(GlobalStorage.controller?.controller)
    }

    private func updateDeviceDescription(controller: WearableControllerType?) {
        
        if let controller = controller {
            
            controller.updateWearableIdentity { [unowned self] (deviceDescription, status) in
                
                self.deviceDescription = deviceDescription
                
                if let firmwareRevision = self.deviceDescription?.firmwareRevision {
                    
                    // Save current firmware rev
                    self.deviceFirmwareRevision        = firmwareRevision
                    
                    // UI updates
                    dispatch_async(dispatch_get_main_queue()) {
                        
                        self.installedVersionLabel.text    = firmwareRevision
                        self.checkButton.enabled           = true
                        
                        self.removeLoading(self.view, completion: nil)
                    }
                }
            }
        }
    }

    
    // MARK: - Table view data source

    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 2
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return 1
    }

    
    // MARK: Button Actions
    
    @IBAction func checkAction(sender: AnyObject) {
        
        DLog("Check button tapped")
        
        addLoading(view, message: "latest_firmware".localized)

        installButton.enabled                   = false
        downloadButton.enabled                  = false
        installingLabel.hidden                  = true
        downloadingLabel.hidden                 = true
        installingProgressView.hidden           = true
        downloadingProgressView.hidden          = true
        installingPercentCompleteLabel.hidden   = true
        downloadingPercentCompleteLabel.hidden  = true
        newFirmwareLabel.hidden                 = true
        newFirmwareVersionLabel.hidden          = true

        firmware.checkAvailableFirmware( { [unowned self] fw in
            
            if let currentVersion = self.deviceFirmwareRevision {
                
                // UI updates
                dispatch_async(dispatch_get_main_queue()) {
                    
                    if FirmwareManager.isNewFirmware(currentVersion, newVersion: fw.version) {
                        
                        self.newFirmwareVersionLabel.text   = fw.version
                    }
                    else {
                        self.newFirmwareVersionLabel.text   = "up_to_date".localized
                    }
                    
                    self.downloadButton.enabled         = true
                    self.newFirmwareLabel.hidden        = false
                    self.newFirmwareVersionLabel.hidden = false
                    
                    self.removeLoading(self.view, completion:nil)
                }//main thread
            }

            }, error: { e in
                
                self.presentDefaultAlert("\(e)", subText: .None, dismissText: "ok".localized)
        })
    }
    
    @IBAction func downloadAction(sender: AnyObject) {
        
        firmware.downloadFirmware( { [weak self] success in
            
            // UI updates
            dispatch_async(dispatch_get_main_queue()) {
                
                self?.downloadingLabel.hidden                   = false
                self?.downloadingProgressView.hidden            = false
                self?.downloadingPercentCompleteLabel.hidden    = false
                
                DLog("Download started")
            }
            
            }, failure: { failure in
            
                DLog("Failed to download:  \(failure.getCloudError())")
                
            }, progress: {[unowned self] progress in
            
                // UI updates
                dispatch_async(dispatch_get_main_queue()) {
                    
                    let percentComplete = (Int64)(progress * 100.0)
                    
                    self.downloadingProgressView.progress      = progress
                    self.downloadingPercentCompleteLabel.text  = "\(percentComplete)%"
                    
                    DLog("Percent complete: \(percentComplete)%")
                }
            }, completed: {[unowned self] completed in
            
                dispatch_async(dispatch_get_main_queue()) {
                    
                    self.downloadButton.enabled = false
                    self.installButton.enabled  = true
                    
                    DLog("Completed download: \(completed.getFileName()))")
                }
            })
    }

    @IBAction func installAction(sender: AnyObject) {
        
        guard let path = firmware.latestCloudFirmwareInfo?.path else {
            assertionFailure("Path to firmware is nil")
            return
        }
        
        installButton.enabled                   = false
        installingLabel.hidden                  = false
        installingProgressView.hidden           = false
        installingPercentCompleteLabel.hidden   = false
        
        DLog("User tapped Install")

        firmware.installFirmware( {started in
            
            DLog("Started firmware update")
            
            }, progress: {[unowned self] progress in
            
                dispatch_async(dispatch_get_main_queue()) {
                    
                    var percentComplete = (Int64)(progress * 100.0)
                    if progress > 1 {
                        percentComplete = (Int64)(progress * 100.0) - 1
                    }
                    
                    self.installingProgressView.progress        = progress
                    self.installingPercentCompleteLabel.text    = "\(percentComplete)%"
                }
                
            }, completed: { [unowned self] version in
            
                dispatch_async(dispatch_get_main_queue()) {
                    
                    self.installingProgressView.progress        = 1.0
                    self.installingPercentCompleteLabel.text    = "\(100)%"
                    self.newFirmwareLabel.hidden                = true
                    self.newFirmwareVersionLabel.hidden         = true
                    
                    self.updateDeviceDescription(GlobalStorage.controller?.controller)

                    DLog("Installation completed. Now at version: \(version)")
                    
                    do {
                        let fileManager = NSFileManager.defaultManager()
                        try fileManager.removeItemAtPath(path)
                        
                    } catch let error as NSError {
                        Log.warning?.message("Could not remove firmware file after installation: \(path): error: \(error.description)")
                    }
                    self.presentDefaultAlert("Installation Completed", subText: nil, dismissText: "OK")
                }
                
            }, failed: {failed in })
    }
    
    
    // MARK: Add / Remove loading
    
    private func addLoading(view: UIView, message: String) {
        
        do {
            try Loading.sharedInstance.addLoading(view, mode: .Indeterminate, label: message)
        }
        catch LoadingError.LoadingInstanceAlreadyAdded {
            DLog("Already scanning")
        }
        catch {
            ALog("Something went wrong")
        }
        
    }
    
    private func removeLoading(view: UIView, completion: LoadingCompletionBlock?) {
        
        // Remove loading
        do {
            try Loading.sharedInstance.removeLoading(view, completion:completion)
        }
        catch LoadingError.LoadingInstanceAlreadyAdded {
            DLog("Already stopped scanner")
        }
        catch {
            ALog("Something went wrong")
        }
    }
}