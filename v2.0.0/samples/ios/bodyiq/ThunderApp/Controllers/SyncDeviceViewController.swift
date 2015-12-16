//
//  SyncDeviceViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/3/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class SyncDeviceViewController: UIViewController {

    // MARK: Buttons
    
    @IBAction func syncButtonTap() {
        
        performSegueWithIdentifier(Constants.SyncToScanDevices, sender: nil)
    }
    
    @IBAction func syncLaterButtonTap() {
        
        
    }
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        self.statusBarBackgroundColor(Constants.StatusBarColor)
        
        DLog("\(NSStringFromClass(self.classForCoder)) loaded")
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        setupNavBar()
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)        
    }
    
    
    // MARK: Back
    
    @objc private func back(sender: AnyObject) {
        self.navigationController?.popViewControllerAnimated(true)
    }
    
    
    // MARK: Setup
    
    private func setupNavBar() {
        
        self.navigationController?.setNavigationBarHidden(false, animated: true)
        self.navigationItem.customize("sync_device".localized, target: self, action: Selector("back:"))
    }
}
