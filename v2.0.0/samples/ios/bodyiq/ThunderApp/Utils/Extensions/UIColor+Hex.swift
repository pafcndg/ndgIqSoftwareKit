//
//  UIColor+Hex.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 21/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit

extension UIColor {
    
    convenience init(red: UInt8, green: UInt8, blue: UInt8) throws {
        
        if red < 0 || red > 255 {
            throw ColorComponentError.InvalidRedComponent
        }
        
        if green < 0 || green > 255 {
            throw ColorComponentError.InvalidGreenComponent
        }
        
        if blue < 0 || blue > 255 {
            throw ColorComponentError.InvalidBlueComponent
        }
        
        self.init(red: CGFloat(red) / 255.0, green: CGFloat(green) / 255.0, blue: CGFloat(blue) / 255.0, alpha: 1.0)
    }
    
    convenience init(hex:Int) {
        
        self.init(hex: hex, alpha:1.0)
    }
    
    convenience init(hex:Int, alpha:  Double) {
        
        self.init(
            red: CGFloat((hex >> 16) & 0xff) / 255.0,
            green: CGFloat((hex >> 8) & 0xff) / 255.0,
            blue: CGFloat(hex & 0xff) / 255.0,
            alpha: CGFloat(alpha))
    }
}