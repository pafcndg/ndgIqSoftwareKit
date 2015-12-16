//
//  String+Utils.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/2/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit

extension String {
    var localized: String {
        return self.localizedWithComment("")
    }
}

extension String {
    func localizedWithComment(comment:String) -> String {
        return NSLocalizedString(self, tableName: nil, bundle: NSBundle.mainBundle(), value: "", comment: comment)
    }
}

extension String {
    var html2AttStr:NSAttributedString {
        
        do {
            let string = try NSAttributedString(data: dataUsingEncoding(NSUTF8StringEncoding)!, options:[NSDocumentTypeDocumentAttribute:NSHTMLTextDocumentType, NSCharacterEncodingDocumentAttribute: NSUTF8StringEncoding], documentAttributes: nil)
            
            return string
        }
        catch {
            DLog("Error creating attributed string!")
        }
        
        return NSAttributedString(string: self)
    }
}

extension String {
    
    func isValidEmail() -> Bool {
        
        let emailRegEx = "[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}"
        
        let emailTest   = NSPredicate(format:"SELF MATCHES %@", emailRegEx)
        
        return emailTest.evaluateWithObject(self)
    }
}

extension String {
    
    mutating func clean() {
        self = ""
    }
}

extension String {
    
    static func randomStringWithLength (len : Int) -> NSString {
        
        let letters : NSString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        
        let randomString : NSMutableString = NSMutableString(capacity: len)
        
        for (var i=0; i < len; i++){
            let length = UInt32 (letters.length)
            let rand = arc4random_uniform(length)
            randomString.appendFormat("%C", letters.characterAtIndex(Int(rand)))
        }
        
        return randomString
    }
}