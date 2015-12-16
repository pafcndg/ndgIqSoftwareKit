//
//  ThunderBtnType1.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/30/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class ThunderBtnType3: UIButton {

    required init?(coder aDecoder: NSCoder) {
        
        super.init(coder: aDecoder)
        
        // Button
        self.layer.cornerRadius     = 25.0;
        self.layer.borderColor      = UIColor.init(hex: Constants.HighlightActiveTabs).CGColor
        self.layer.borderWidth      = 1.5
        self.backgroundColor        = UIColor.init(hex: Constants.HighlightActiveTabs)
        self.tintColor              = UIColor.whiteColor()
        
        // Label
        self.titleLabel?.font       = UIFont(name: "System", size: 30)
        self.titleLabel?.textColor  = UIColor(hex: 0xffffff)
        
        // Shadow
        self.layer.shadowOffset     = CGSizeMake(1, 1)
        self.layer.shadowRadius     = 1
        self.layer.shadowColor      = UIColor.blackColor().CGColor
        self.layer.shadowOpacity    = 1.0
    }
}
