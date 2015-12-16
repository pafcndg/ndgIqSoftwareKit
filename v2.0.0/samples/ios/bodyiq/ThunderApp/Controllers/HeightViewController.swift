//
//  WeightViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/3/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class HeightViewController: UIViewController, UITextFieldDelegate {

    
    // MARK: Members
    let limitLength = 1
    
    
    // MARK: Buttons
    
    @IBAction func submitButtonTap() {
        
        if validFields() {
            
            do {
                let user = try LocalDataStoreManager.getUser()
                
                user.height_cm = Double(composeHeight())
                
                
                // Save Body/Core user
                let userManager = UserManager()
                let uuid        = userManager.createUser(user)
                
                user.uuid       = uuid
                
                
                LocalDataStoreManager.setUser(user)
                
                if let viewControllers = self.navigationController?.viewControllers where viewControllers.count > 0 {
                    
                    self.navigationController!.popToViewController(viewControllers[viewControllers.count - 4], animated: false);
                }
                
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
    
    
    // MARK: UI
    
    @IBOutlet weak var height1: UITextField!
    @IBOutlet weak var height2: UITextField!
    @IBOutlet weak var height3: UITextField!
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
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
        
        setupTextFields()
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
        
        view.removeAllGestureRecognizers()
    }
    
    
    // MARK: Back
    
    @objc private func back(sender: AnyObject) {
        self.navigationController?.popViewControllerAnimated(true)
    }
    
    
    // MARK: Setup
    
    private func setupNavBar() {
        
        self.navigationController?.setNavigationBarHidden(false, animated: true)
        self.navigationItem.customize("current_height".localized, target: self, action: Selector("back:"))
    }
    
    private func setupTextFields() {
        
        height1.font = UIFont(name: "HelveticaNeue", size: 70)
        height1.delegate = self
        height2.font = UIFont(name: "HelveticaNeue", size: 70)
        height2.delegate = self
        height3.font = UIFont(name: "HelveticaNeue", size: 70)
        height3.delegate = self
        
        let tapGesture = UITapGestureRecognizer(target: self, action: "tap:")
        view.addGestureRecognizer(tapGesture)
    }
    
    private func setupSwipeEffects() {
        
        let swipeRight          = UISwipeGestureRecognizer(target: self, action: "respondToSwipeGesture:")
        swipeRight.direction    = UISwipeGestureRecognizerDirection.Right
        self.view.addGestureRecognizer(swipeRight)
        
        let swipeDown           = UISwipeGestureRecognizer(target: self, action: "respondToSwipeGesture:")
        swipeDown.direction     = UISwipeGestureRecognizerDirection.Down
        self.view.addGestureRecognizer(swipeDown)
    }
    
    
    // MARK: Validate
    
    func validFields() -> Bool {
        
        guard 30 ... 250 ~= composeHeight() else {
            
            self.showAlert("BodyIQRefApp".localized, message: "invalid_height".localized, ok: "ok".localized, cancel: "cancel".localized,
                cancelAction: nil, okAction: nil, completion: nil)
            
            return false
        }
        
        return true
    }
    
    func composeHeight() -> Int {
        
        return Int("\(height1.text ?? "0")\(height2.text ?? "0")\(height3.text ?? "0")") ?? 0
    }
    
    
    // MARK: Swipe responder
    
    func respondToSwipeGesture(gesture: UIGestureRecognizer) {
        
        if let swipeGesture = gesture as? UISwipeGestureRecognizer {
            
            switch swipeGesture.direction {
            case UISwipeGestureRecognizerDirection.Right:
                print("Swiped right")
            case UISwipeGestureRecognizerDirection.Down:
                print("Swiped down")
            case UISwipeGestureRecognizerDirection.Left:
                print("Swiped left")
                performSegueWithIdentifier(Constants.CreateAccountToSetWeight, sender: nil)
            case UISwipeGestureRecognizerDirection.Up:
                print("Swiped up")
            default:
                break
            }
        }
    }
    
    
    // MARK: Keyboard
    
    func textField(textField: UITextField, shouldChangeCharactersInRange range: NSRange, replacementString string: String) -> Bool {
        
        guard let text = textField.text else { return true }
        
        let newLength = text.utf16.count + string.utf16.count - range.length
        
        return newLength <= limitLength
    }
    
    func tap(gesture: UITapGestureRecognizer) {
        
        height1.resignFirstResponder()
        height2.resignFirstResponder()
        height3.resignFirstResponder()
    }
}
