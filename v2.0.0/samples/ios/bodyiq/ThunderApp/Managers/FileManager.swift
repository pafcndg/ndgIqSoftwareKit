//
//  FileManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 28/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

typealias ExistPath = (isDir: ObjCBool, exists:Bool)

class FileManager {
    
    class func getPath(folder: String, directory: NSSearchPathDirectory) -> String {
        
        let docDirPath = NSSearchPathForDirectoriesInDomains(directory, .UserDomainMask, true).first!
        
        let docPath = docDirPath + "/" + folder
        
        return docPath
    }
    
    class func removeFile(path: String) {
        
        let fileManager = NSFileManager.defaultManager()
        
        if fileManager.fileExistsAtPath(path) {
            do {
                try fileManager.removeItemAtPath(path)
                
            }
            catch let error as NSError {
                DLog("Could not remove existing firmware file: \(path): error: \(error.description)")
            }
        }
    }
    
    class func existsAtPath(path: String) -> ExistPath {
        
        var isDir : ObjCBool = false
        var exists:Bool      = false
        
        let fileManager = NSFileManager.defaultManager()
        
        if fileManager.fileExistsAtPath(path, isDirectory:&isDir) {
            
            if isDir {
                print("file exists and is a directory")
            }
            else {
                print("file exists and is not a directory")
            }
            
            exists = true
        }
        else {
            print("file does not exist")
        }
        
        return (isDir: isDir, exists: exists)
    }
}