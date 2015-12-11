//
//  signUpViewController.swift
//  SensorRefApp2
//
//  Created by Gal Shirin on 10/16/15.
//  Copyright © 2015 fiat.bender. All rights reserved.
//

import UIKit
import CDKCore

protocol signUpViewControllerDelegate {
    func didSignUpSuccessfully(email: String)
}

class signUpViewController: UITableViewController {

    var delegate: signUpViewControllerDelegate! = nil
    
    @IBOutlet weak var emailTextField: UITextField!
    @IBOutlet weak var passwordTextField: UITextField!
    @IBOutlet weak var signUpButton: UIButton!
    @IBOutlet weak var cancelButton: UIButton!
    @IBOutlet weak var activityIndicator: UIActivityIndicatorView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func didClickSignUp(sender: AnyObject) {
        activityIndicator.startAnimating()
        enableUserInteraction(false)
        let timeIQService = TimeIQBGService.sharedInstance;
        self.activityIndicator.startAnimating()
        timeIQService.createAccount(emailTextField.text!, password: emailTextField.text!, firstName: emailTextField.text!, lastName: emailTextField.text!, onSuccess: { () -> Void in
            self.activityIndicator.stopAnimating()
            self.enableUserInteraction(true)
            self.delegate.didSignUpSuccessfully(self.emailTextField.text!)
            self.dismissViewControllerAnimated(true, completion: nil)
            }) { (error: CDKError!) -> Void in
                self.activityIndicator.stopAnimating()
                self.enableUserInteraction(true)
                self.showAlertView("Account creation failed", strBody:error.localizedDescription)
        }
    }
    
    @IBAction func didClickCancel(sender: AnyObject) {
        self.dismissViewControllerAnimated(true, completion: nil)
    }
    
    func showAlertView(strTitle:String, strBody:String)
    {
        let alert = UIAlertController(title: strTitle, message: strBody, preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: NSLocalizedString("Dismiss", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
        self.presentViewController(alert, animated: true, completion: nil);
    }
    
    func enableUserInteraction(flag: Bool) {
        emailTextField.enabled = flag
        passwordTextField.enabled = flag
        signUpButton.enabled = flag
        signUpButton.hidden = !flag
        cancelButton.enabled = flag
    }
    
    override func scrollViewDidScroll(scrollView: UIScrollView) {
        self.view.endEditing(true)
    }
}
