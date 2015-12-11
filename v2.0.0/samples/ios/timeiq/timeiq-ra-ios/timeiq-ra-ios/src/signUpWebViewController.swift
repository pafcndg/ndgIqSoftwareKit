//
//  signUpWebViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 23/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

class signUpWebViewController: UIViewController, UIWebViewDelegate {
    
    @IBOutlet var activityIndicator: UIActivityIndicatorView?
    @IBOutlet var webView: UIWebView?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.webView?.loadRequest(NSURLRequest.init(URL: NSURL.init(string: "https://tsodev-login.td.wearables.infra-host.com/accounts/new?client_id=ndgThunderDome")!))
    }
    
    @IBAction func didClickDismiss() {
        self.dismissViewControllerAnimated(true, completion: nil)
    }
    
    func webView(webView: UIWebView, shouldStartLoadWithRequest request: NSURLRequest, navigationType: UIWebViewNavigationType) -> Bool {
        if (!self.activityIndicator!.isAnimating())
        {
            self.activityIndicator?.startAnimating()
        }
        
        return true
    }
    
    func webViewDidFinishLoad(webView: UIWebView) {
        self.activityIndicator?.stopAnimating()
    }
}
