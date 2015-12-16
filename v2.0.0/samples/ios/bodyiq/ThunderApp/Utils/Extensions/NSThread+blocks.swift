//
//  NSThread+blocks.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/9/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

public typealias Block = @convention(block) () -> Void

// MARK: - Extension to add some methods to the NSThread class to run a block on any thread you have a reference to.
extension NSThread {
    
    /**
     Perform block on current thread
     
     - parameter block: block to be executed
     - parameter wait:
     */
    func performBlock(block: Block, waitUntilDone wait: Bool) {
        
        NSThread.performSelector("runBlock:", onThread: self, withObject: block as? AnyObject, waitUntilDone: wait)
    }

    /**
     Perform block on main thread
     
     - parameter block: block to be executed
     */
    class func performBlockOnMainThread(block: Block) {
        
        NSThread.mainThread().performBlock(block)
    
    }
    
    /**
     Perform block in background thread
     
     - parameter block: block to be executed
     */
    class func performBlockInBackground(block: Block) {
        
        NSThread.performSelectorInBackground("runBlock:", withObject: block as? AnyObject)
    
    }
    
    /**
     Execute block
     
     - parameter block: block to be executed
     */
    class func runBlock(block: Block) {
                
        block()
    
    }
    
    /**
     Perform block on current thread
     
     - parameter block: block to be executed
     */
    func performBlock(block: Block) {
        
        if NSThread.currentThread().isEqual(self) {
            block()
        }
        else {
            self.performBlock(block, waitUntilDone: false)
        }
    }
    
    /**
     Perform block
     
     - parameter block: block to be executed
     - parameter delay: 
     */
    func performBlock(block: Block, afterDelay delay: NSTimeInterval) {
        
        self.performSelector("performBlock:", withObject: block as? AnyObject, afterDelay: delay)
    }
}