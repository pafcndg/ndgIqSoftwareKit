//
//  Array+HashableFilterAppend.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/9/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

extension Array where Element: Hashable {
    
    mutating func filterAppend(newElement: Element) {
        for item in self {
            if item.hashValue == newElement.hashValue {
                return
            }
        }
        self.append(newElement)
    }
}

extension Array {
    
    var last: Element {
        return self[self.endIndex - 1]
    }
}