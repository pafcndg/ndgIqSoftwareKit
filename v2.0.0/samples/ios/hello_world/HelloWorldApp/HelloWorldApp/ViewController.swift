//
//  ViewController.swift
//  HelloWorldApp
//
//  Created by DraganX L Jovev on 12/10/15 (WW50).
//  Copyright © 2015 iOS Developer. All rights reserved.
//

import UIKit
import IQCore
import BodyIQ

class ViewController: UIViewController {

    var scanner: WearableScannerType
    
    required init?(coder aDecoder: NSCoder) {
        scanner = WearableScanner.scannerForProtocol(.BLE)
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        /* 
        Start coding here :)
        For instance, we can use scanner here to search for wearables...

        scanner.startScan({ (tokenResult) -> () in
                // Found wearable
                print("Found \(tokenResult.token)")
            }, error: { (error) -> () in
                // Error
                print("Error detected \(error)")
            }) { () -> () in
                // Finished
                print("Scan finished")
        }
        */
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

