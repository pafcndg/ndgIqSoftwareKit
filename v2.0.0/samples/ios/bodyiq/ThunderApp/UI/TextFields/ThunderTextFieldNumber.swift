//
//  ThunderTextFieldNumber.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/3/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class ThunderTextFieldNumber: ThunderTextField {

    let padding     = UIEdgeInsets(top: 0, left: 0, bottom: 0, right: 0);
    
    override func textRectForBounds(bounds: CGRect) -> CGRect {
        return self.newBounds(bounds)
    }
    
    override func placeholderRectForBounds(bounds: CGRect) -> CGRect {
        return self.newBounds(bounds)
    }
    
    override func editingRectForBounds(bounds: CGRect) -> CGRect {
        return self.newBounds(bounds)
    }
    
    private func newBounds(bounds: CGRect) -> CGRect {
        
        var newBounds = bounds
        newBounds.origin.x += padding.left
        newBounds.origin.y += padding.top
        newBounds.size.height -= padding.top + padding.bottom
        newBounds.size.width -= padding.left + padding.right
        return newBounds
    }
}
