//
//  LocalDataStoreManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 28/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

private enum DataAccessObject: String {
    case User = "UserObjectKey"
}

// TODO: Support more than one user
class LocalDataStoreManager {
    
    class func setUser(user: ThunderUser) {
    
        let savedData = NSKeyedArchiver.archivedDataWithRootObject(user)
        let defaults = NSUserDefaults.standardUserDefaults()
        defaults.setObject(savedData, forKey: DataAccessObject.User.rawValue)
    }
    
    class func getUser() throws -> ThunderUser {
        
        let defaults = NSUserDefaults.standardUserDefaults()
        
        if let savedData = defaults.objectForKey(DataAccessObject.User.rawValue) as? NSData {
            return NSKeyedUnarchiver.unarchiveObjectWithData(savedData) as! ThunderUser
        }
        
        throw LocalDataStoreError.UnreachableObject
    }
}