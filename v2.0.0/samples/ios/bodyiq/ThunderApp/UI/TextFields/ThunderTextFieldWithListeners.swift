//
//  ThunderTextFieldWithListeners.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 16/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import UIKit

class ThunderTextFieldWithListeners: UITextField {
    
 
    // MARK: Members
    
    var texdDidBeginEditing: DidBeginEditing?
    var textDidEndEditing: DidEndEditing?
    var textDisplayAnimationComplete: TextDisplayAnimationComplete?
    var textEntryAnimationComplete: TextEntryAnimationComplete?
    
    
    // MARK: Listener
    
    /**
    Events listener
    
    - parameter texdDidBeginEditing:          Called when begin editing
    - parameter textDidEndEditing:            Called when end editing
    - parameter textDisplayAnimationComplete: Called when text display animation is completed
    - parameter textEntryAnimationComplete:   Called when text entry animation is completed
    */
    func addEventsListener(texdDidBeginEditing: DidBeginEditing,
        textDidEndEditing: DidEndEditing,
        textDisplayAnimationComplete: TextDisplayAnimationComplete,
        textEntryAnimationComplete: TextEntryAnimationComplete) {
            
            self.texdDidBeginEditing = texdDidBeginEditing
            self.textDidEndEditing = textDidEndEditing
            self.textDisplayAnimationComplete = textDisplayAnimationComplete
            self.textEntryAnimationComplete = textEntryAnimationComplete
    }
    
}