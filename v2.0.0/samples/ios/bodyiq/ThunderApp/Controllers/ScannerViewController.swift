//
//  SettingsViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 3/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit
import IQCore
import BodyIQ

class ScannerCell:UITableViewCell {
    
    @IBOutlet weak var deviceInfo:UILabel!
    @IBOutlet weak var connectBtn:UIButton!
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
    
    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)
    }
}

class ScannerViewController: UITableViewController {
    
    
    // MARK: UI
    
    @IBOutlet weak var table: UITableView!
    
    
    // MARK: Members
    
    private var tokens = WearableTokenArray()
    private var scanner:ScannerManager?
    //private var controller:ControllerManager?
    private var loadingView:UIView?
    private let delayTime = dispatch_time(DISPATCH_TIME_NOW, Int64(10 * Double(NSEC_PER_SEC)))
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        self.statusBarBackgroundColor(Constants.StatusBarColor)
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
        
        addScanner()
        
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        setupNavBar()
        
        startScanner()
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
        
        // Clear array elements
        tokens.removeAll()
        
        // Clear table
        self.table.reloadData()
        
        // Stop scanner
        stopScanner()
        
        // Release scanner
        removeScanner()
    }
    
    
    // MARK: Add / Remove Scanner
    
    private func addScanner() {
        
        let loadingSize:CGFloat = 200.0
        
        loadingView = UIView(frame:CGRectMake( (table.frame.width - loadingSize) / 2,
                                               (table.frame.height - loadingSize) / 2,
                                               loadingSize, loadingSize) )
        
        table.addSubview(loadingView!)
        
        scanner = ScannerManager(view: loadingView!)
        
    }
    
    private func removeScanner() {
        
        loadingView?.removeFromSuperview()
        loadingView = nil
        
        scanner?.destroy()
        scanner = nil
        
    }
    
    
    // MARK: Start / Stop Scanner
    
    private func startScanner() {
        
        // Start scanning & display the found devices
        scanner?.startScan( { [weak self] (tokens) -> () in
            
            DLog("found: \(tokens.last?.token.displayName)")
            
            // TODO: keep only one array of tokens
            
            self?.tokens.filterAppend(tokens.last!)
            
            // Update table
            dispatch_async(dispatch_get_main_queue()) {
                
                self?.table.reloadData()
                
            }
        })
    }
    
    private func stopScanner() {
        
        // Stop scanning
        scanner?.stopScan()
        
    }
    
    
    // MARK: Connect / Disconnect
    
    private func connectToDevice(token: WearableToken) {
        
        guard GlobalStorage.controller == nil || GlobalStorage.controller?.deviceConnectionStatus() == .Disconnected else {
            DLog("Already connected")
            return
        }
        
        let wearableController      = WearableController.controllerForToken(token)
        
        GlobalStorage.controller    = ControllerManager(token: wearableController, observer:{ [weak self] (status) -> () in
            
            if let _ = self {
                if status == .Connected {
                    
                    DLog("Connected")
                    
                    GlobalStorage.isDeviceConnected = true
                    
                    NSNotificationCenter.defaultCenter().postNotificationName(Constants.ConnectionStateNotification, object: self, userInfo: [Constants.IsConnectionState:true])
                    
                    GlobalStorage.controller?.deviceDescription( { [weak self] (deviceDescription) -> () in
                        
                        // Save device description in global storage
                        GlobalStorage.deviceDescription =  deviceDescription
                        
                        
                        // We only want to push the segue if we are in the ScannerViewController
                        if let topController = UIApplication.topViewController() {
                            
                            if topController is ScannerViewController {
                                
                                dispatch_async(dispatch_get_main_queue()) {
                                    
                                    self?.performSegueWithIdentifier(Constants.ScannerToSidebar, sender: nil)
                                }
                            }
                        }
                    })
                }
                else {
                    
                    DLog("Disconnected")
                    
                    GlobalStorage.isDeviceConnected = false
                    
                    NSNotificationCenter.defaultCenter().postNotificationName(Constants.ConnectionStateNotification, object: self, userInfo: [Constants.IsConnectionState:false])
                    
                    // Promt user to reconnect
                    
                    dispatch_async(dispatch_get_main_queue()) { [weak self] in
                        
                        let alertController = UIAlertController(title: "\(Constants.AppName)", message: NSString(format: "connect".localized, (token.displayName != nil ? token.displayName : "unknown".localized)!) as String, preferredStyle: .Alert)
                        
                        let cancelAction = UIAlertAction(title: "cancel".localized, style: .Cancel) { (action) in
                            
                        }
                        alertController.addAction(cancelAction)
                        
                        let OKAction = UIAlertAction(title: "ok".localized, style: .Default) { (action) in
                            
                            self?.connectToDevice(token)
                        }
                        alertController.addAction(OKAction)
                        
                        self?.presentViewController(alertController, animated: true) {
                            
                        }
                    }
                }
            }
        })
        
        // FIXME: The is a bug in IQCore. You can not get the WearableControllerType and connect straigth. The device won't be ready.
        sleep(1)
        
        // Stop scanner
        stopScanner()
        
        GlobalStorage.controller?.connectToController( { (state) -> () in

        })
    }
    
    private func disconnectFromDevice() {
        
        guard GlobalStorage.controller != nil && GlobalStorage.controller?.deviceConnectionStatus() == .Connected else {
            DLog("Already disconnected")
            return
        }
        
        // Disconnect from device
        GlobalStorage.controller?.disconnectFromController(nil)
        //controller = nil
        
    }
    
    
    // MARK: Setup
    
    private func setupNavBar() {
        
        self.navigationController?.setNavigationBarHidden(false, animated: true)
        self.navigationItem.customize("sync_device".localized, target: self, action: Selector("back:"))
    }
    
    
    // MARK: Back
    
    @objc private func back(sender: AnyObject) {
        self.navigationController?.popViewControllerAnimated(true)
    }
    
    
    
    // MARK: UITableViewDataSource
    
    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        
        return self.tokens.count
        
    }
    
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        
        return 1
        
    }
    
    override func tableView(tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        
        return "devices".localized;
        
    }
    
    override func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 70
    }
    
    override func tableView(tableView: UITableView, willDisplayHeaderView view: UIView, forSection section: Int) {
        
        let header: UITableViewHeaderFooterView = view as! UITableViewHeaderFooterView
        
        // Background
        header.contentView.backgroundColor      = UIColor.whiteColor()
        
        // Text
        header.textLabel!.textColor             = UIColor.init(hex: 0x747474)
        header.textLabel!.font                  = UIFont(name: "HelveticaNeueLight ", size: 17)
    }
    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        
        let cell                = table.dequeueReusableCellWithIdentifier(Constants.CellId, forIndexPath: indexPath) as! ScannerCell
        let token               = tokens[indexPath.item]
        
        // Disable the cell's selection property
        cell.selectionStyle     = UITableViewCellSelectionStyle.None
                
        //cell.deviceInfo?.attributedText = "\(token.token.displayName ?? "unknown".localized) <br><b>id</b>: \(token.token.id)".html2AttStr
        cell.deviceInfo.text    = token.token.displayName ?? "unknown".localized
        
        return cell
    }
    
    
    // MARK: UITableViewDelegate
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        DLog("Selected row \(self.tokens[indexPath.item].token.displayName)")
        
        // Prepare controller
        let selToken:WearableToken     = self.tokens[indexPath.item].token
        
        // Save token in global storage
        GlobalStorage.token            = selToken
        
        connectToDevice(selToken)
    }
    
    override func tableView(tableView: UITableView, estimatedHeightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        
        return 80.0
    }
}