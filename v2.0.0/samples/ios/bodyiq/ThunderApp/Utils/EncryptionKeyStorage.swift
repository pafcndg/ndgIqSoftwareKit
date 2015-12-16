//
//  EncryptionKeyStorage.swift
//  IQCore
//
//  Created by BrianX P Thomas on 12/8/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

private var _sharedStorage: EncryptionKeyStorage?

class EncryptionKeyStorage {
    
    var key: NSData
    
    static func generateKey() -> NSData {
        let encryptionKey = NSMutableData(length: 64)!
        SecRandomCopyBytes(kSecRandomDefault, encryptionKey.length, UnsafeMutablePointer<UInt8>(encryptionKey.mutableBytes))
        return encryptionKey
    }
    
    init() {
        let defaults = NSUserDefaults()
        if let encryptionKey = defaults.objectForKey("encryptor") as? NSData {
            key = encryptionKey
        } else {
            let encryptionKey = EncryptionKeyStorage.generateKey()
            defaults.setObject(encryptionKey, forKey: "encryptor")
            key = encryptionKey
        }
    }
    
    func setKey(encryptionKey: NSData) -> NSData {
        let defaults = NSUserDefaults()
        defaults.setObject(encryptionKey, forKey: "encryptor")
        self.key = encryptionKey
        return encryptionKey
    }
    
    static func sharedStorage() -> EncryptionKeyStorage {
        if let storage = _sharedStorage {
            return storage
        } else {
            return EncryptionKeyStorage()
        }
    }
    
}
