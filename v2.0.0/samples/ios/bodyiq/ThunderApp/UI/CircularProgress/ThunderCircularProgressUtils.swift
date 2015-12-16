//
//  ThunderCircularProgressUtils.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 09/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class ThunderCircularProgressUtils: NSObject {

    class func DegreesToRadians (value:CGFloat) -> CGFloat {
        return value * CGFloat(M_PI) / 180.0
    }
    
    class func RadiansToDegrees (value:CGFloat) -> CGFloat {
        return value * 180.0 / CGFloat(M_PI)
    }
    
    class func Clamp<T: Comparable>(value: T, minMax: (T, T)) -> T {
        let (min, max) = minMax
        if value < min {
            return min
        } else if value > max {
            return max
        } else {
            return value
        }
    }
    
    class func Mod(value: Int, range: Int, minMax: (Int, Int)) -> Int {
        let (min, max) = minMax
        assert(abs(range) <= abs(max - min), "range should be <= than the interval")
        if value >= min && value <= max {
            return value
        } else if value < min {
            return Mod(value + range, range: range, minMax: minMax)
        } else {
            return Mod(value - range, range: range, minMax: minMax)
        }
    }
}
