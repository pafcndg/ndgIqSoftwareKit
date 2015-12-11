//
//  common.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 23/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

class common {
    
    
    class func labelMessageForString(message: String) -> UILabel {
        let messageLabel = UILabel(frame: CGRect(x: 0, y: 0, width: 100, height: 20))
        
        messageLabel.text = message
        messageLabel.textColor = UIColor(red: 109.0/255.0, green: 89.0/255.0, blue: 73.0/255.0, alpha: 1.0)
        messageLabel.numberOfLines = 0;
        messageLabel.textAlignment = .Center;
        messageLabel.font = UIFont.boldSystemFontOfSize(17)
        messageLabel.sizeToFit()
        
        return messageLabel
    }
}