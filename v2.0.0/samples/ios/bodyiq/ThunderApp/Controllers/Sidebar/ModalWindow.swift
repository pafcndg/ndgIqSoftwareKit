//
//  ActivityViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/6/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

class ModalWindow : UIViewController {
    
    @IBAction func close(sender: AnyObject) {
        dismissViewControllerAnimated(true, completion: nil)
    }
    
}
