//
//  SplashViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/28/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit

private let delayTime = dispatch_time(DISPATCH_TIME_NOW, Int64(2 * Double(NSEC_PER_SEC)))

class SplashViewController: UIViewController {

    
    // MARK: Members
    
    let progressIndicatorView = ProgressView(frame: CGRectZero)
    
    
    // MARK: Segue
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        
        if let identifier = segue.identifier where identifier == Constants.LoadingToLoginSegue,
           let controller = segue.destinationViewController as? LoginViewController {
            
            DLog("Going to \(NSStringFromClass(controller.classForCoder))")
        }
    }
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
    }
    
    override func customViewWillAppear(animated: Bool) {
        
        super.customViewWillAppear(animated)
        
        self.statusBarBackgroundColor(Constants.StatusBarColor)
        
        self.addLoading()
        
        dispatch_after(delayTime, dispatch_get_main_queue()) { [weak self] in
            self?.removeLoading()
        }
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
        
    }
    
    // MARK: Add / Remove Loading
    
    private func addLoading() {
        
        // Add loading to view
        self.view.addSubview(self.progressIndicatorView)
        
        // Setup
        let size:CGFloat = 100.0
        progressIndicatorView.frame = CGRectMake( (self.view.bounds.size.width - size) / 2, self.view.bounds.size.height / 2 + 10, size, size)
        progressIndicatorView.autoresizingMask = [.FlexibleWidth, .FlexibleHeight]
        
        // Start loading
        progressIndicatorView.start()
    }
    
    private func removeLoading() {
        
        progressIndicatorView.removeFromSuperview()
        progressIndicatorView.stop( { [weak self] in
            
            self?.performSegueWithIdentifier(Constants.LoadingToLoginSegue, sender: nil)
        })
    }
}
