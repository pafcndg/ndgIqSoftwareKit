//
//  UIApplication+Utils.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 16/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

extension UIApplication {
    
    class func topViewController(base: UIViewController? = (UIApplication.sharedApplication().delegate as! AppDelegate).window?.rootViewController) -> UIViewController? {
        
        if let nav = base as? UINavigationController {
            return topViewController(nav.visibleViewController)
        }
        
        if let tab = base as? UITabBarController {
            if let selected = tab.selectedViewController {
                return topViewController(selected)
            }
        }
        
        if let presented = base?.presentedViewController {
            return topViewController(presented)
        }
        
        return base
    }
}