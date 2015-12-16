//
//  ThunderDropdownListTableViewCell.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/2/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class ThunderDropdownListTableViewCell: UITableViewCell {
    
    override init(style: UITableViewCellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        self.backgroundColor = UIColor.clearColor()
        
        // Add bottom border to cells
        self.addBorder(edges: [.Bottom], colour: UIColor(hex: Constants.NonFocusedColor), thickness: 0.5)
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        self.backgroundColor = UIColor.clearColor()
    }
    
}