//
//  LoginViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/29/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit

class LoginViewController: UIViewController {
    
    
    // MARK: UI
    
    @IBOutlet weak var createAccountBtn: UIButton!
    @IBOutlet weak var loginBtn: UIButton!
    @IBOutlet weak var skipBtn: UIButton!
    
    
    // MARK: Buttons
    
    @IBAction func createAccountButtonTap() {
        
        DLog("Create account pressed")
        
        performSegueWithIdentifier(Constants.LoginToCreateAccountnSegue, sender: nil)
    }
    
    @IBAction func loginButtonTap() {
        
        DLog("Login pressed")
        
        performSegueWithIdentifier(Constants.LoginToCredentialsSegue, sender: nil)
    }
    
    @IBAction func skipButtonTap() {
        DLog("Skip pressed")
    }
    
    
    // MARK: Segue
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        
        if let identifier = segue.identifier where identifier == Constants.LoginToCreateAccountnSegue,
            let controller = segue.destinationViewController as? CreateAccountViewController {
                
                DLog("Going to \(NSStringFromClass(controller.classForCoder))")
        }
        else if let identifier = segue.identifier where identifier == Constants.LoginToCredentialsSegue,
            let controller = segue.destinationViewController as? CreateAccountViewController {
                
                DLog("Going to \(NSStringFromClass(controller.classForCoder))")
        }
    }
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        DLog("\(NSStringFromClass(self.classForCoder)) loaded")        
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func customViewWillAppear(animated: Bool) {
        
        super.customViewWillAppear(animated)
        
        self.statusBarBackgroundColor(Constants.StatusBarColor)
        
        setupButtonLabels()
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
    }
    
    
    // MARK: Setup
    
    private func setupButtonLabels() {
        
        createAccountBtn.setTitle("create_account".localized, forState: .Normal)
        loginBtn.setTitle("login".localized, forState: .Normal)
        skipBtn.setTitle("skip".localized, forState: .Normal)
    }
}