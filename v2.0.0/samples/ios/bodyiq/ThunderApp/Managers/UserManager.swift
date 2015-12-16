//
//  UserManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 24/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import BodyIQ
import IQCore


public class ThunderUser: NSObject, NSCoding {
    
    var identifier: String = ""
    var uuid: String = ""
    var firstName: String?
    var lastName: String?
    var email: String?
    var password: String?
    var phoneNumber: String?
    var weight_kg: Double = 0.0
    var height_cm: Double = 0.0
    var biologicalSex: BodyIQBiologicalSex = .Male
    
    public init(identifier: String, firstName: String?, lastName: String?,
                email: String?, password: String?, phoneNumber: String?,
                weight_kg: Double, height_cm: Double, biologicalSex: BodyIQBiologicalSex) {
        
        self.firstName      = firstName
        self.identifier     = identifier
        self.lastName       = lastName
        self.password       = password
        self.email          = email
        self.phoneNumber    = phoneNumber
        self.weight_kg      = weight_kg
        self.height_cm      = height_cm
        self.biologicalSex  = biologicalSex
    }
    
    public init(identifier: String, firstName: String?, lastName: String?,
        email: String?, phoneNumber: String?,
        weight_kg: Double, height_cm: Double, biologicalSex: BodyIQBiologicalSex) {
            
            self.firstName      = firstName
            self.identifier     = identifier
            self.lastName       = lastName
            self.email          = email
            self.phoneNumber    = phoneNumber
            self.weight_kg      = weight_kg
            self.height_cm      = height_cm
            self.biologicalSex  = biologicalSex
    }
    
    public init(identifier: String, firstName: String?, email: String?, password: String?, biologicalSex: BodyIQBiologicalSex) {
        
        self.identifier     = identifier
        self.firstName      = firstName
        self.email          = email
        self.password       = password
        self.biologicalSex  = biologicalSex
    }
    
    public required init(coder aDecoder: NSCoder) {
        
        identifier      = aDecoder.decodeObjectForKey("identifier") as! String
        uuid            = aDecoder.decodeObjectForKey("uuid") as! String
        firstName       = aDecoder.decodeObjectForKey("firstName") as? String
        lastName        = aDecoder.decodeObjectForKey("lastName") as? String
        email           = aDecoder.decodeObjectForKey("email") as? String
        password        = aDecoder.decodeObjectForKey("password") as? String
        phoneNumber     = aDecoder.decodeObjectForKey("phoneNumber") as? String
        weight_kg       = aDecoder.decodeObjectForKey("weight_kg") as! Double
        height_cm       = aDecoder.decodeObjectForKey("height_cm") as! Double
        biologicalSex   = BodyIQBiologicalSex(rawValue: (aDecoder.decodeObjectForKey( "biologicalSex" ) as! Int)) ?? .Female
    }
    
    public func encodeWithCoder(aCoder: NSCoder) {
        
        aCoder.encodeObject(identifier, forKey: "identifier")
        aCoder.encodeObject(uuid, forKey: "uuid")
        aCoder.encodeObject(firstName, forKey: "firstName")
        aCoder.encodeObject(lastName, forKey: "lastName")
        aCoder.encodeObject(email, forKey: "email")
        aCoder.encodeObject(password, forKey: "password")
        aCoder.encodeObject(phoneNumber, forKey: "phoneNumber")
        aCoder.encodeObject(weight_kg, forKey: "weight_kg")
        aCoder.encodeObject(height_cm, forKey: "height_cm")
        aCoder.encodeObject(biologicalSex.rawValue, forKey:"biologicalSex" )
    }
}

/// Class to integrate IQCore and BodyIQ users and manage them.
// TODO: 1. persist userName - userID - UUID
//       2. add delete method (at least to "mark" a user as deleted even if persist on the Realm DB)
public class UserManager {
    
    // MARK: Members
    
    // MARK: Init / Deinit
    
    init() {
        
    }
    
    deinit {
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    
    // MARK: CRUD
    
    func createUser(user: ThunderUser) -> String {
        
        let coreUser            = UserIdentity(identifier: user.identifier)
        coreUser.firstName      = user.firstName
        coreUser.lastName       = user.lastName
        coreUser.email          = user.email
        coreUser.phoneNumber    = user.phoneNumber
        
        // Internally generated UUID
        let uuid                = createCoreUser(coreUser)
        
        let bodyUser            = BodyIQUserBodyStatistics(weight_kg: user.weight_kg, height_cm: user.height_cm, biologicalSex: user.biologicalSex)
        
        if let error = BodyIQ.configureWithUserIdentifier(user.identifier, bodyStats: bodyUser) {
            fatalError("Error generating user: \(error)")
        }
        
        return uuid
    }
    
    func getUserForIdentifier(identifier: String) -> ThunderUser? {
        
        let coreUser = LocalDataStore.userIdentityForIdentifier(identifier)
        
        let bodyUser = BodyIQ.bodyStatistics
        
        guard coreUser != nil && bodyUser != nil else {
            return nil
        }
        
        return buildUser(coreUser!, bodyUser: bodyUser!)
    }
    
    func getUserForUUID(uuid: String) -> ThunderUser? {
        
        let coreUser = LocalDataStore.userIdentityForUUID(uuid)
        
        let bodyUser = BodyIQ.bodyStatistics
        
        guard coreUser != nil && bodyUser != nil else {
            return nil
        }
        
        return buildUser(coreUser!, bodyUser: bodyUser!)
    }
    
    func getDeviceForUserIdentifier(serialNumber: String) -> WearableIdentity? {
        
        return LocalDataStore.wearableIdentityForSerialNumber(serialNumber)
    }
    
    
    // MARK: Private functions
    
    private func buildUser(coreUser: UserIdentity, bodyUser: BodyIQUserBodyStatistics) -> ThunderUser {
        
        return ThunderUser(identifier:      coreUser.identifier,
                            firstName:      coreUser.firstName,
                            lastName:       coreUser.lastName,
                            email:          coreUser.email,
                            phoneNumber:    coreUser.phoneNumber,
                            weight_kg:      bodyUser.weight_kg,
                            height_cm:      bodyUser.height_cm,
                            biologicalSex:  bodyUser.biologicalSex)
    }
    
    private func createCoreUser(user: UserIdentity) -> String {
        
        LocalDataStore.setCurrentUserIdentity(user)
        
        // FIXME: review this
        return user.uuid
    }
}