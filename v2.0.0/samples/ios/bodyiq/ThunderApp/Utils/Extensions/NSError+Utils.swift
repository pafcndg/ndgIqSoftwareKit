//
//  NSError+Utils.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 02/12/15.
//  Copyright © 2015 Intel All rights reserved.
//

import Foundation

extension NSError {
    
    convenience init(localizedDescription: String?, localizedFailureReason: String?, domain: String) {
        
        let userInfo: [NSObject : AnyObject] =
        [
            NSLocalizedDescriptionKey : localizedDescription ?? "",
            NSLocalizedFailureReasonErrorKey : localizedFailureReason ?? ""
        ]
        
        self.init(domain: domain, code: -1, userInfo: userInfo)
    }
    
    convenience init(localizedDescription: String?, localizedFailureReason: String?, code: Int, domain: String) {
        
        let userInfo: [NSObject : AnyObject] =
        [
            NSLocalizedDescriptionKey : localizedDescription ?? "",
            NSLocalizedFailureReasonErrorKey : localizedFailureReason ?? ""
        ]
        
        self.init(domain: domain, code: code, userInfo: userInfo)
    }
}