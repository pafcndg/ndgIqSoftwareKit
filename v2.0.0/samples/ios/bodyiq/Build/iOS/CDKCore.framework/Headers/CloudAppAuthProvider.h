//
//  CloudAppAuthProvider.h
//  CloudSDK
//

#import "CloudAuthProvider.h"
#import "CloudResponse.h"

/**
 * Application authentication provider
 */
@interface CloudAppAuthProvider : CloudAuthProvider


/**
 * Initialize CloudAppAuthProvider object & loads public token from plist
 */
-(id)init;

/**
 * Initialize object with parameters provided by the cloud
 * @param baseUrl the URL of the authentication server
 */
-(id)initWithBaseURL:(NSString*)baseUrl;

/**
 * Initialize object with parameters provided by the cloud
 * @param baseUrl the URL of the authentication server
 * @param cid identifies the application to the cloud. Obtained from the administration portal.
 * @param secret authenticates the application to the cloud. Obtained from the administration portal.
 * @param token authentication token as shown in the adminstration portal
 */
-(id)initWithBaseURL:(NSString *)baseUrl clientId:(NSString*)cid clientSecret:(NSString*)secret authToken:(NSString *)token;

/**
 * Method which initiates Login process
 */
- (void)login;

@end
