//
//  UIViewController+StatusBar.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 28/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit

typealias CancelActionPerformed = (UIAlertAction) -> ()
typealias OkActionPerformed     = (UIAlertAction) -> ()
typealias AlertPerformed        = () -> ()


// MARK: Status Bar

extension UIViewController {
    
    public override class func initialize() {
        struct Static {
            static var token: dispatch_once_t = 0
        }
        
        // make sure this isn't a subclass
        if self !== UIViewController.self {
            return
        }
        
        dispatch_once(&Static.token) {
            
            let originalSelector    = Selector("viewWillAppear:")
            let swizzledSelector    = Selector("customViewWillAppear:")
            
            let originalMethod      = class_getInstanceMethod(self, originalSelector)
            let swizzledMethod      = class_getInstanceMethod(self, swizzledSelector)
            
            let didAddMethod        = class_addMethod(self, originalSelector, method_getImplementation(swizzledMethod), method_getTypeEncoding(swizzledMethod))
            
            if didAddMethod {
                class_replaceMethod(self, swizzledSelector, method_getImplementation(originalMethod), method_getTypeEncoding(originalMethod))
            }
            else {
                method_exchangeImplementations(originalMethod, swizzledMethod);
            }
        }
    }
    
    
    // MARK: - Method Swizzling
    
    func customViewWillAppear(animated: Bool) {
        
        self.customViewWillAppear(animated)
        
        // Hide navigation bar
        self.navigationController?.setNavigationBarHidden(true, animated: false)
        
        //statusBarBackgroundColor(0x0083EE)
    }
    
    
    // MARK: - Custom statusBar background color
    
    func statusBarBackgroundColor(color: Int) {
        
        let view = UIView(frame:
            CGRect(x: 0.0, y: 0.0, width: UIScreen.mainScreen().bounds.size.width, height: 20.0)
        )
        view.backgroundColor = UIColor.init(hex: color)
        
        self.view.addSubview(view)
    }
}


// MARK - Alert

extension UIViewController {
    
    func showAlert(title: String, message: String, ok: String, cancel: String,
                    cancelAction: CancelActionPerformed?,
                    okAction: OkActionPerformed?,
                    completion: AlertPerformed?) {
        
        let alertController = UIAlertController(title: title, message: message, preferredStyle: .Alert)
        
        if cancelAction != nil {
            let cancelAction = UIAlertAction(title: cancel, style: .Cancel) { (action) in
                if let a = cancelAction {
                    a(action)
                }
            }
            alertController.addAction(cancelAction)
        }
                        
        let OKAction = UIAlertAction(title: ok, style: .Default) { (action) in
            if let a = okAction {
                a(action)
            }
        }
        alertController.addAction(OKAction)
        
        self.presentViewController(alertController, animated: true) {
            if let a = completion {
                a()
            }
        }
    }
    
    func presentDefaultAlert(alertText: String?, subText: String?, dismissText: String) -> UIAlertController {
        
        let alert = UIAlertController(title: alertText, message: subText, preferredStyle: .Alert)
        let action = UIAlertAction(title: dismissText, style: UIAlertActionStyle.Default) { (action) -> Void in
            
        }
        alert.addAction(action)
        self.presentViewController(alert, animated: true, completion: .None)
        return alert
    }
}


// MARK - Text Field

extension UIViewController {
    
    func textFieldDidBeginEditing(textField: UITextField) {
        animateViewMoving(true, moveValue: 100)
    }
    
    func textFieldDidEndEditing(textField: UITextField) {
        animateViewMoving(false, moveValue: 100)
    }
    
    func animateViewMoving (up:Bool, moveValue :CGFloat){
        
        let movementDuration:NSTimeInterval = 0.3
        let movement:CGFloat = ( up ? -moveValue : moveValue)
        UIView.beginAnimations( "animateView", context: nil)
        UIView.setAnimationBeginsFromCurrentState(true)
        UIView.setAnimationDuration(movementDuration )
        self.view.frame = CGRectOffset(self.view.frame, 0,  movement)
        UIView.commitAnimations()
    }
}


// MARK - Connection state 

extension UIViewController {
    
    func setupNavBar(image: String) {
        
        self.navigationItem.rightBarButtonItem = nil
        
        let image       = UIImage(named: image)
        let frame       = CGRectMake(0, 0, 25, 25)
        let button      = UIButton()
        button.frame    = frame
        button.setImage(image, forState: .Normal)
        let rightMenuButton = UIBarButtonItem(customView: button)
        self.navigationItem.rightBarButtonItem = rightMenuButton
    }
    
    @objc func setConnectionState(notification: NSNotification) {
        
        if notification.name == Constants.ConnectionStateNotification {
            
            if let connState = notification.userInfo![Constants.IsConnectionState] as? Bool {
                
                if connState {
                    connect()
                }
                else {
                    disconnect()
                }
            }
        }
    }
    
    func connect() {
        dispatch_async(dispatch_get_main_queue()) {
            self.setupNavBar("green_indicator")
        }
    }
    
    func disconnect() {
        dispatch_async(dispatch_get_main_queue()) {
            self.setupNavBar("red_indicator")
        }
    }
}