//
//  IOSAuthCredentialsProvider.h
//  TSO
//
//  Created by Gal Shirin on 02/11/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "IAuthCredentialsProvider.h"
#import <CDKCore/AuthCredentials.h>

@interface IOSAuthCredentialsProvider : NSObject <IAuthCredentialsProvider>

/* Protocol methods */

- (boolean)isUserLoggedIn;

- (TSOUserInfo *)getUserInfo;

- (id)getCredentials;

/* End protocol methods */

- (instancetype)initWithAuthCredentials:(AuthCredentials*)authCredentials
                        withTSOUserInfo:(TSOUserInfo*)userInfo;

@end
