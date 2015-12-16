//
//  ThunderDropdownListWrapperView.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/2/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class ThunderDropdownListWrapperView: UIView {
    
    internal var context: ThunderDropdownMenu?
    
    override func pointInside(point: CGPoint, withEvent event: UIEvent?) -> Bool {
        
        for subview in subviews {
            if !subview.hidden && subview.alpha > 0 && subview.userInteractionEnabled && subview.pointInside(convertPoint(point, toView: subview), withEvent: event) {
                return true
            }
        }
        return false
    }
    
}