//
//  loginViewController.swift
//  SensorRefApp2
//
//  Created by Gal Shirin on 10/13/15.
//  Copyright © 2015 fiat.bender. All rights reserved.
//

import UIKit
import CDKCore

let AUTO_LOGIN = "AUTO_LOGIN";

class loginViewController: UITableViewController, signUpViewControllerDelegate, UITextFieldDelegate {
    
    @IBOutlet weak var emailTextField: UITextField!
    @IBOutlet weak var passwordTextField: UITextField!
    @IBOutlet weak var signInButton: UIButton!
    @IBOutlet weak var autoLoginSwitch: UISwitch!
    @IBOutlet weak var signUpButton: UIButton!
    @IBOutlet weak var activityIndicator: UIActivityIndicatorView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        autoLoginSwitch.setOn(loginViewController.isAutoLoginEnabled(), animated: false)
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func didClickSignIn(sender: AnyObject) {
        
        guard let emailText = emailTextField.text, passText = passwordTextField.text
            where !emailText.isEmpty && !passText.isEmpty else
        {
            showAlertView(NSLocalizedString("login_error_title", comment: ""), message: NSLocalizedString("username_or_password_is_empty", comment:""))
            return
        }
        
        let timeIQService = TimeIQBGService.sharedInstance;
        self.activityIndicator.startAnimating()
        enableUserInteraction(false)
        
        timeIQService.login(emailText, password: passText,
            onSuccess: {
            self.activityIndicator.stopAnimating()
            self.enableUserInteraction(true)
            self.dismissViewControllerAnimated(true, completion: nil);
            
            if (self.autoLoginSwitch.on)
            {
                NSUserDefaults.standardUserDefaults().setBool(self.autoLoginSwitch.on, forKey: AUTO_LOGIN);
            }
            }) { (error: CDKError!) -> Void in
                self.showAlertView(NSLocalizedString("login_error_title", comment: ""), message:error.localizedDescription)
                self.activityIndicator.stopAnimating()
                self.passwordTextField.text = ""
                self.enableUserInteraction(true)
        }
    }
    
//    @IBAction func didClickSignUp(sender: AnyObject) {
//        let storyboard = UIStoryboard(name: "Login", bundle: nil)
//        let vc = storyboard.instantiateViewControllerWithIdentifier("signUpViewController") as! signUpViewController
//        vc.delegate = self
//        self.presentViewController(vc, animated: true, completion: nil)
//    }
    
    @IBAction func valueChanged(sender: UISwitch) {
        
        NSUserDefaults.standardUserDefaults().setBool(sender.on, forKey: AUTO_LOGIN)
        //NSUserDefaults.standardUserDefaults().boolForKey(AUTO_LOGIN);
    }
    
    func showAlertView(title:String, message:String)
    {
        let alert = UIAlertController(title: title, message: message, preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: NSLocalizedString("Dismiss", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
        self.presentViewController(alert, animated: true, completion: nil);
    }
    
    func enableUserInteraction(flag: Bool) {
        emailTextField.enabled = flag
        passwordTextField.enabled = flag
        signInButton.enabled = flag
        signInButton.hidden = !flag
        autoLoginSwitch.enabled = flag
        signUpButton.enabled = flag
    }
    
    override func scrollViewDidScroll(scrollView: UIScrollView) {
        self.view.endEditing(true)
    }
    
    func didSignUpSuccessfully(email: String) {
        self.emailTextField.text = email
    }
    
    class func isAutoLoginEnabled() -> Bool {
        return NSUserDefaults.standardUserDefaults().boolForKey(AUTO_LOGIN)
    }
    
    //MARK: - UITextFieldDelegate
    
    func textFieldShouldReturn(textField: UITextField) -> Bool {
        if textField == emailTextField {
            passwordTextField.becomeFirstResponder()
        } else if textField == passwordTextField {
            self.didClickSignIn(self)
        }
        
        return true
    }
    
}
