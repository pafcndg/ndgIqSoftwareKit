//
//  SideBarNavigationController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 08/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class SideBarNavigationController: UINavigationController {

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
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
    }
    
}
