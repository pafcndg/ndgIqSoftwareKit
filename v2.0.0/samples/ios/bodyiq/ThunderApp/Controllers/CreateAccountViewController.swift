//
//  CreateAccountViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/30/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit
import BodyIQ

class CreateAccountViewController: UIViewController {

    
    // MARK: UI
    
    @IBOutlet weak var nameTextField: ThunderTextField!
    @IBOutlet weak var emailTextField: ThunderTextField!
    @IBOutlet weak var passwordTextField: ThunderTextField!
    @IBOutlet weak var dropdownMenu: ThunderDropdownMenu!
    
    
    // MARK: Buttons
    
    @IBAction func nextButtonTap() {
        
        // DEGUB
        //performSegueWithIdentifier(Constants.CreateAccountToSetWeight, sender: nil)
        
        if validFields() {
            
            cleanupForm()
            
            performSegueWithIdentifier(Constants.CreateAccountToSetWeight, sender: nil)
            
            DLog("Move to configure user!")
        }
    }
    
    
    // MARK: Members
    
    let pickerData = ["Male","Female","Unknown"]
    var bioSex:BodyIQBiologicalSex?
    
    
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
        self.nameTextField.text     = "pablo"
        self.emailTextField.text    = "pablo@aumentia.com"
        self.passwordTextField.text = "asdf"
        */
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
        
        dropdownMenu?.closeList()
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        setupNavBar()
        
        setupTextFields()
        
        setupDropdownMenu()
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
        
        view.removeAllGestureRecognizers()
    }
    
    
    // MARK: Segue
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        
        if let identifier = segue.identifier where identifier == Constants.CreateAccountToSetWeight,
            let controller = segue.destinationViewController as? WeightViewController {
                
                DLog("Going to \(NSStringFromClass(controller.classForCoder))")
        }
    }
    
    
    // MARK: Setup
    
    private func setupNavBar() {
        
        self.navigationController?.setNavigationBarHidden(false, animated: true)
        self.navigationItem.customize("account_creation".localized, target: self, action: Selector("back:"))
    }
    
    private func setupTextFields() {
        
        nameTextField.placeholder       = "name".localized
        emailTextField.placeholder      = "email_address".localized
        passwordTextField.placeholder   = "password".localized
        
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
    
    private func setupDropdownMenu() {
        
        dropdownMenu?.items = pickerData
        
        dropdownMenu?.title = "gender".localized
        
        dropdownMenu?.onItemSelect = { [weak self] (index, item) -> () in
            
            DLog("Item \(item) selected at index \(index)")
            
            if item as! String == "Male" {
                self?.bioSex = .Male
            }
            else if item as! String == "Female" {
                self?.bioSex = .Female
            }
            else {
                self?.bioSex = .Unknown
            }
        }
        
        dropdownMenu?.onMenuButtonTapped = { willOpen -> () in
            
            DLog("Drop menu button tapped")
        }
        
        dropdownMenu?.configuration.headerBackgroundColor = UIColor.clearColor()
        
        dropdownMenu?.configuration.titleColor = UIColor.whiteColor()
        dropdownMenu?.configuration.titleFont = UIFont(name: "HelveticaNeue", size: 17)!
        
        dropdownMenu?.configuration.cellHeight                               = 40
        dropdownMenu?.configuration.animationDuration                        = 0.3
        
        dropdownMenu?.configuration.cellFont                                 = UIFont(name: "HelveticaNeue", size: 15)!
        dropdownMenu?.configuration.cellTextColor                            = UIColor(hex: Constants.BackgroundColor)
        
        dropdownMenu?.configuration.dropdownListBackgroundColor              = UIColor.whiteColor()
        dropdownMenu?.configuration.dropdownListBorderColor                  = UIColor(hex: Constants.ButtonsActionBarLinkColor)
        dropdownMenu?.configuration.dropdownListBorderWidth                  = 2.0
        
        dropdownMenu?.configuration.dropdownListMaxHeight                    = 200
        dropdownMenu?.configuration.dropdownListSelectedItemIndex            = 0
        
        dropdownMenu?.configuration.dropdownListSelectedItemBackgroundColor  = UIColor(hex: Constants.HighlightActiveField)
        dropdownMenu?.configuration.dropdownListSelectedItemAccessoryType    = UITableViewCellAccessoryType.Checkmark
        
        dropdownMenu?.configuration.dropdownIconAssetName                    = "arrow"
        dropdownMenu?.configuration.dropdownIconWillRotate                   = true
        
        dropdownMenu?.addBorder(edges: [.Bottom], colour: UIColor(hex: Constants.NonFocusedColor), thickness: 0.65)
    }
    
    
    // MARK: Back
    
    @objc private func back(sender: AnyObject) {
        self.navigationController?.popViewControllerAnimated(true)
    }
    
    
    // MARK: User

    func validFields() -> Bool {
        
        guard nameTextField.text?.characters.count > 0 else {
            
            self.showAlert("BodyIQRefApp".localized, message: "fill_name".localized, ok: "ok".localized, cancel: "cancel".localized,
                           cancelAction: nil, okAction: nil, completion: nil)
            return false
        }
        
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
        
        let user = ThunderUser(identifier: "user-id",firstName: nameTextField.text, email: emailTextField.text, password: passwordTextField.text, biologicalSex: bioSex ?? .Male)
        
        LocalDataStoreManager.setUser(user)
        
        return true
    }
    
    
    // MARK: Cleanup
    
    private func cleanupForm() {
        
        nameTextField.text?.clean()
        emailTextField.text?.clean()
        passwordTextField.text?.clean()
        dropdownMenu?.title = pickerData.first!
    }
    

    // MARK: Keyboard
    
    private func textFieldDidBeginEditing(textField: UITextField) {
        animateViewMoving(true, moveValue: 100)
    }
    private func textFieldDidEndEditing(textField: UITextField) {
        animateViewMoving(false, moveValue: 100)
    }
    
    private func animateViewMoving (up:Bool, moveValue :CGFloat){
        
        let movementDuration:NSTimeInterval = 0.3
        let movement:CGFloat = ( up ? -moveValue : moveValue)
        UIView.beginAnimations( "animateView", context: nil)
        UIView.setAnimationBeginsFromCurrentState(true)
        UIView.setAnimationDuration(movementDuration )
        self.view.frame = CGRectOffset(self.view.frame, 0,  movement)
        UIView.commitAnimations()
    }
    
    func tap(gesture: UITapGestureRecognizer) {
        
        nameTextField.resignFirstResponder()
        emailTextField.resignFirstResponder()
        passwordTextField.resignFirstResponder()
    }
}
