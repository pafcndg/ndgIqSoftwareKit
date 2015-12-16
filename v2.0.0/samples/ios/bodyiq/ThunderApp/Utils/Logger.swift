//
//  Logger.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/9/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

/**
 Simple logging function that prints the method name & class name of string.
 
 - parameter message:      Message to log.
 - parameter functionName: Method name to print. By default the caller method.
 - parameter fileName:     File name to print. By default the file where the log function is called.
 - requires:               Add the "-D DEBUG" flag to the "Swift Compiler - Custom Flags" to Debug mode, under Build Settings.
 - note:                   Will only print on DEBUG configuration.
 */
func DLog(message:String, functionName:String = __FUNCTION__, fileName:NSString = __FILE__)
{
    #if DEBUG
        Log(message, functionName:functionName, fileName:fileName)
    #endif
}

/**
 Simple logging function that prints the method name & class name of string.
 
 - parameter message:      Message to log.
 - parameter functionName: Method name to print. By default the caller method.
 - parameter fileName:     File name to print. By default the file where the log function is called.
 - note:                   Will print in any configuration
 */
func ALog(message:String, functionName:String = __FUNCTION__, fileName:NSString = __FILE__)
{
        Log(message, functionName:functionName, fileName:fileName)
}


// MARK: Log

private func Log(message:String, functionName:String = __FUNCTION__, fileName:NSString = __FILE__)
{
    print("\(Constants.AppName): \(message)\n Method_Name = \(functionName)\n File_Name = \(fileName.lastPathComponent)\n")
}