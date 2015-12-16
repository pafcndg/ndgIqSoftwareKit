#import <Foundation/Foundation.h>

/**
 *  The credential object that stores OAuth tokens
 *  received from the cloud after the user logs in.
 *  These credentials will be included in all future
 *  cloud requests that need authentication. In general, 
 *  applications do not need to worry about the credentials
 *  stored here. They are managed (i.e. refreshed, stored, 
 *  received) as needed by the cloud SDK.
 */
@interface AuthCredentials : NSObject

/**
 *  Checks if user is logged in.
 *
 *  @return Yes if logged in. No otherwise.
 */
-(BOOL)isUserLoggedIn;

/**
 *  Get access token
 */
-(NSString*)getAccessToken;

/**
 *  Get refresh token
 */
-(NSString*)getRefreshToken;

/**
 *  Get the cloud ID of the user
 */
-(NSString*)getUserId;

/**
 *  Get the user name associated with
 *  this authentication credential.
 */
-(NSString*)getUserName;

/**
 *  Get the cloud ID of this device.
 */
-(NSString*)getClientId;

/**
 *  Get the auth identifier.
 */
-(NSString*)getIdentifier;

@end
