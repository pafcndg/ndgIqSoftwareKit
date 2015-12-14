//
//  SimpleAuthCredentialsProvider.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 04/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import TimeIQ
import CDKCore

class SimpleAuthCredentialsProvider: NSObject, IAuthCredentialsProvider {
        
    var userInfo: TSOUserInfo?
    let authProvider: CloudUAAAuthProvider
    
    static var sharedInstance = SimpleAuthCredentialsProvider()
    
    private override init()
    {
        authProvider = CloudUAAAuthProvider(baseURL: "https://tsodev-uaa.td.wearables.infra-host.com/", clientId: "ndgThunderDome", clientSecret: "ndgThunderDomeSecRet");
        
        if authProvider.authCredentials.isUserLoggedIn() {
//            self.userInfo = TSOUserInfo(NSString: authProvider.authCredentials.userId, withNSString: authProvider.authCredentials.userName, withNSString: "")
            self.userInfo = TSOUserInfo(NSString: SimpleAuthCredentialsProvider.createUserIdentifier(self.authProvider.authCredentials), withNSString: authProvider.authCredentials.userName, withNSString: "")
        }
    }
    
    func login(username: String, password: String, onSuccess: (() -> Void)!, onFailure: ((CDKError!) -> Void)!)
    {
//        self.userInfo = TSOUserInfo(NSString: username, withNSString: username, withNSString: password)
        
        self.authProvider.login(username, password: password,
            success: {
                self.userInfo = TSOUserInfo(NSString: SimpleAuthCredentialsProvider.createUserIdentifier(self.authProvider.authCredentials), withNSString: username, withNSString: password)
                onSuccess()
            },
            failure: { (error: CDKError!) in
                onFailure(error)
            })
    }
    
    class func createUserIdentifier(cred: AuthCredentials) -> String? {
        if (cred.getUserId() != nil && cred.getClientId() != nil) {
            return "\(cred.getUserId())@\(cred.getClientId())"
        }
        return nil
    }
    
    func logout() {
        self.authProvider.logout()
    }
    
    func createAccount(email: String, password:String, firstName:String, lastName:String, onSuccess: (() -> Void)!, onFailure: ((CDKError!) -> Void)!) {
        
        self.authProvider.createAccount(email, password: password, firstName: firstName, lastName: lastName, success: { () -> Void in
            onSuccess();
            }, failure: { (error: CDKError!) -> Void in
                onFailure(error);
        })
        
    }
    
    //MARK: - IAuthCredentialsProvider
    
    func getCredentials() -> AnyObject! {
        return authProvider.getAuthCredentials();
    }
    
    func getUserInfo() -> TSOUserInfo! {
        return userInfo;
    }
    
    func isUserLoggedIn() -> Bool {
        return authProvider.getAuthCredentials().isUserLoggedIn();
    }
}