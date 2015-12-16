//
//  CloudUAAAuthProvider.h
//  CloudSDK
//
//  Handle UAA login
//
#import "CloudAuthProvider.h"

/**
 * Handles the authentication flows for UAA which include create account and login.
 */
@interface CloudUAAAuthProvider : CloudAuthProvider

/**
 * Initialize object with parameters provided by the cloud
 * @param baseUrl the URL of the authentication server
 * @param clientId application identifier
 * @param clientSecret application secret needed to authenticate application to the cloud
 */
-(id)initWithBaseURL:(NSString*)baseUrl clientId:(NSString*)clientId clientSecret:(NSString*)clientSecret;

/**
 * Initialize object,loads all cloud params from plist
 */
-(instancetype)init;

/**
 * Method which initiates UAA Login process
 * @param email the user identifier
 * @param password the password for native UAA
 * @param onSuccess notifies app if login completed successfully.
 * @param onFailure notifies app if login failure occurs.
 */
- (void)login:(NSString *)email password:(NSString *)password
      success:(void(^)(void))onSuccess
      failure:(void(^)(CDKError*))onFailure;

/**
 * Method which create an UAA account
 * @param email the user identifier
 * @param password the password for the user
 * @param firstName the user's first name
 * @param lastName the user's last name
 * @param onSuccess notifies app if createAccount success.
 * @param onFailure notifies app if createAccount failure occurs.
 */
- (void)createAccount:(NSString *)email password:(NSString *)password
         firstName:(NSString *)firstName lastName:(NSString *)lastName
              success:(void(^)(void))onSuccess
              failure:(void(^)(CDKError*))onFailure;

/**
 * Method which change the password for an UAA account
 * @param email the user identifier
 * @param password the password for the user
 * @param newPassword the new password for the user
 * @param onSuccess notifies app if changePassword success.
 * @param onFailure notifies app if changePassword failure occurs.
 */
- (void)changePassword:(NSString *)email password:(NSString *)password
           newPassword:(NSString *)newPassword
               success:(void(^)(void))onSuccess
               failure:(void(^)(CDKError*))onFailure;

/**
 * Method which delete an UAA account
 * @param email the user identifier
 * @param onSuccess notifies app if deleteAccount success.
 * @param onFailure notifies app if deleteAccount failure occurs.
 */
- (void)deleteAccount:(NSString *)email
              success:(void(^)(void))onSuccess
              failure:(void(^)(CDKError*))onFailure;

- (void)logout;

@end
