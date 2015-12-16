//
//  ActivityViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/6/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

class TabVCTemplate : UIViewController {
    
    
    // MARK: Members
    
    /// Placeholder for the tab's index
    var selectedTab = 0
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        // Show navigation bar
        self.navigationController?.setNavigationBarHidden(false, animated: true)
    }
    
    override func viewWillAppear(animated: Bool) {
        
        // Sent from LeftMenu
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "openPushWindow", name: Constants.OpenPushWindow, object: nil)
        
        // Set navigation bar color
        self.navigationController?.navigationBar.barTintColor = UIColor(hex: Constants.StatusBarColor)
        
        // Set navigation bar title
        self.navigationItem.title = "daily_tracker".localized
    }
    
    override func viewWillDisappear(animated: Bool) {
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }
    
    
    // MARK: deinit
    
    deinit {
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }
    
    
    // MARK: Touch Events
    
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        
        // Close the menu when touching the tab controller
        NSNotificationCenter.defaultCenter().postNotificationName(Constants.CloseMenu, object: nil)
        view.endEditing(true)
    }
    
    /**
     Push a new window from the side menu
     */
    func openPushWindow(){
        
        //if tabBarController?.selectedIndex == selectedTab {
            performSegueWithIdentifier(Constants.PushWindowFromSideBar, sender: nil)
        //}
    }
    
}
