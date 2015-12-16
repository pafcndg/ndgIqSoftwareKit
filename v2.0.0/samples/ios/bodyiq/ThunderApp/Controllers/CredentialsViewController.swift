//
//  CredentialsViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/2/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit

class CredentialsViewController: UIViewController {

    
    // MARK: UI
    
    @IBOutlet weak var emailTextField: ThunderTextField!
    @IBOutlet weak var passwordTextField: ThunderTextField!
    
    
    // MARK: Buttons
    
    @IBAction func submitButtonTap() {
        
        // DEBUG
        //performSegueWithIdentifier(Constants.CredentialsToSync, sender: nil)
        
        if validFields() {
            do {
                let user = try LocalDataStoreManager.getUser()
                
                guard user.email == emailTextField.text && user.password == passwordTextField.text else {
                    self.showAlert("BodyIQRefApp".localized, message: "invalid_user".localized, ok: "ok".localized, cancel: "cancel".localized,
                        cancelAction: nil, okAction: nil, completion: nil)
                    
                    return
                }
                
                //let userManager = UserManager()
                //let u   = userManager.getUserForUUID(user.uuid)
                //let u2  = userManager.getUserForIdentifier(user.identifier)
                
                // FIXME: User won't persist amongst runs
                //assert(u != nil && u2 != nil, "User not properly stored")
                
                DLog("IN!!!")
                
                cleanupForm()
                
                performSegueWithIdentifier(Constants.CredentialsToSync, sender: nil)
                
            }
            catch LocalDataStoreError.UnreachableObject {
                
                self.showAlert("BodyIQRefApp".localized, message: "no_user".localized, ok: "ok".localized, cancel: "cancel".localized,
                    cancelAction: nil, okAction: nil, completion: nil)
            }
            catch let error as NSError {
                DLog("Unexpected error: \(error.description)")
            }
        }
    }
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        self.statusBarBackgroundColor(Constants.StatusBarColor)
        
        DLog("\(NSStringFromClass(self.classForCoder)) loaded")
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
        
        // TEST
        /*
        emailTextField.text = "pablo@aumentia.com"
        passwordTextField.text = "asdf"
        */
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        setupNavBar()
        
        setupTextFields()
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
        
        view.removeAllGestureRecognizers()
    }
    
    
    // MARK: Cleanup
    
    private func cleanupForm() {
        
        emailTextField.text?.clean()
        passwordTextField.text?.clean()
    }
    
    
    // MARK: Validate
    
    func validFields() -> Bool {
        
        guard emailTextField.text?.characters.count > 0 else {
            
            self.showAlert("BodyIQRefApp".localized, message: "fill_email".localized, ok: "ok".localized, cancel: "cancel".localized,
                cancelAction: nil, okAction: nil, completion: nil)
            
            return false
        }
        
        guard passwordTextField.text?.characters.count > 0 else {
            
            self.showAlert("BodyIQRefApp".localized, message: "fill_password".localized, ok: "ok".localized, cancel: "cancel".localized,
                cancelAction: nil, okAction: nil, completion: nil)
            
            return false
        }
        
        guard emailTextField.text!.isValidEmail() else {
            
            self.showAlert("BodyIQRefApp".localized, message: "wrong_email".localized, ok: "ok".localized, cancel: "cancel".localized,
                cancelAction: nil, okAction: nil, completion: nil)
            
            return false
        }
        
        return true
    }
    
    
    // MARK: Keyboard
    
    func tap(gesture: UITapGestureRecognizer) {
        
        emailTextField.resignFirstResponder()
        passwordTextField.resignFirstResponder()
    }
    
    
    // MARK: Back
    
    @objc private func back(sender: AnyObject) {
        self.navigationController?.popViewControllerAnimated(true)
    }
    
    
    // MARK: Setup
    
    private func setupNavBar() {
        
        self.navigationController?.setNavigationBarHidden(false, animated: true)
        self.navigationItem.customize("login".localized, target: self, action: Selector("back:"))
    }
    
    private func setupTextFields() {
        
        emailTextField.placeholder      = "email_address".localized
        passwordTextField.placeholder   = "password".localized
        
        
        // Move emailTextField up when keyboard appears
        emailTextField.addEventsListener( { [weak self] in
            
            if let email = self?.emailTextField {
                
                self?.textFieldDidBeginEditing(email)
            }
            
            }, textDidEndEditing: { [weak self] in
                
                if let email = self?.emailTextField {
                    
                    self?.textFieldDidEndEditing(email)
                }
                
            }, textDisplayAnimationComplete: { (Bool) -> () in
                
            }, textEntryAnimationComplete: { (Bool) -> () in
        })
        
        // Move passwordTextField up when keyboard appears
        passwordTextField.addEventsListener( { [weak self] in
            
            if let password = self?.passwordTextField {
                
                self?.textFieldDidBeginEditing(password)
            }
            
            }, textDidEndEditing: { [weak self] in
                
                if let password = self?.passwordTextField {
                    
                    self?.textFieldDidEndEditing(password)
                }
                
            }, textDisplayAnimationComplete: { (Bool) -> () in
                
            }, textEntryAnimationComplete: { (Bool) -> () in
        })
        
        
        let tapGesture = UITapGestureRecognizer(target: self, action: "tap:")
        view.addGestureRecognizer(tapGesture)
    }
    
    
    // MARK: Segue
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        
        if let identifier = segue.identifier where identifier == Constants.CredentialsToSync,
            let controller = segue.destinationViewController as? SyncDeviceViewController {
                
                DLog("Going to \(NSStringFromClass(controller.classForCoder))")
        }
    }
}