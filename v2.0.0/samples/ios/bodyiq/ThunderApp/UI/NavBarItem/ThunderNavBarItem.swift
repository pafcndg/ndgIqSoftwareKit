//
//  ThunderNavBarItem1.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/30/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation
import ObjectiveC

private var backButtonKey: UInt8 = 0

extension UINavigationItem {

    var backButton: UIBarButtonItem! {
        get {
            return objc_getAssociatedObject(self, &backButtonKey) as? UIBarButtonItem
        }
        set(newValue) {
            objc_setAssociatedObject(self, &backButtonKey, newValue, objc_AssociationPolicy.OBJC_ASSOCIATION_RETAIN_NONATOMIC)
        }
    }
    
    func customize(itemTitle: String, target: AnyObject?, action: Selector) {
        
        self.title  = itemTitle
        
        backButton  = UIBarButtonItem(title: "Back", style: .Plain, target: target, action: action)
        
        self.leftBarButtonItem = backButton
        self.leftBarButtonItem?.tintColor = UIColor.whiteColor()
    }
}
