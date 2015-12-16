//
//  WeightViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/3/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class WeightViewController: UIViewController, UITextFieldDelegate {

    
    // MARK: Members
    let limitLength = 1
    
    
    // MARK: UI
    
    @IBOutlet weak var weigth1: UITextField!
    @IBOutlet weak var weigth2: UITextField!
    @IBOutlet weak var weigth3: UITextField!
    
    
    // MARK: Buttons
    
    @IBAction func nextButtonTap() {
        
        if validFields() {
            
            do {
                let user = try LocalDataStoreManager.getUser()
                
                user.weight_kg = Double(composeWeight())
                
                LocalDataStoreManager.setUser(user)
                
                performSegueWithIdentifier(Constants.SetWeightToSetHeight, sender: nil)
                
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
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        setupNavBar()
        
        setupTextFields()
        
        setupSwipeEffects()
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
        self.navigationItem.customize("current_weight".localized, target: self, action: Selector("back:"))
    }
    
    private func setupTextFields() {
        
        weigth1.font = UIFont(name: "HelveticaNeue", size: 70)
        weigth1.delegate = self
        weigth2.font = UIFont(name: "HelveticaNeue", size: 70)
        weigth2.delegate = self
        weigth3.font = UIFont(name: "HelveticaNeue", size: 70)
        weigth3.delegate = self
        
        let tapGesture = UITapGestureRecognizer(target: self, action: "tap:")
        view.addGestureRecognizer(tapGesture)
    }
    
    private func setupSwipeEffects() {
        
        let swipeRight          = UISwipeGestureRecognizer(target: self, action: "respondToSwipeGesture:")
        swipeRight.direction    = UISwipeGestureRecognizerDirection.Right
        self.view.addGestureRecognizer(swipeRight)
        
        let swipeDown          = UISwipeGestureRecognizer(target: self, action: "respondToSwipeGesture:")
        swipeDown.direction     = UISwipeGestureRecognizerDirection.Down
        self.view.addGestureRecognizer(swipeDown)
    }
    
    
    // MARK: Validate
    
    func validFields() -> Bool {
        
        guard 30 ... 299 ~= composeWeight() else {
            
            self.showAlert("BodyIQRefApp".localized, message: "invalid_weight".localized, ok: "ok".localized, cancel: "cancel".localized,
                cancelAction: nil, okAction: nil, completion: nil)
            
            return false
        }
        
        return true
    }
    
    func composeWeight() -> Int {
        
        return Int("\(weigth1.text ?? "0")\(weigth2.text ?? "0")\(weigth3.text ?? "0")") ?? 0
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
        
        weigth1.resignFirstResponder()
        weigth2.resignFirstResponder()
        weigth3.resignFirstResponder()
    }
    
    
    // MARK: Segue
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        
        if let identifier = segue.identifier where identifier == Constants.SetWeightToSetHeight,
            let controller = segue.destinationViewController as? HeightViewController {
                
                DLog("Going to \(NSStringFromClass(controller.classForCoder))")
        }
    }
}
