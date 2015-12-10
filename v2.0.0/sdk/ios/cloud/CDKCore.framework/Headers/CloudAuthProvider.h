#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "AuthCredentials.h"
#import "CloudAuthWebViewClient.h"
#import "CloudAuthRequestFactory.h"
#import "CloudResponse.h"
#import "CDKError.h"

#define ACCESS_TOKEN_KEY @"access_token"
#define REFRESH_TOKEN_KEY @"refresh_token"
#define EMPTY_STRING @""
#define NSERROR_OBJECT @"NSError_Object"

/**
 * Handles the login process. A new web view is launched where
 * the user enters their credentials or logs in using a third-party
 * account.
 */
@interface CloudAuthProvider : NSObject

@property(readonly,nonatomic,retain)NSString *baseUrl;
@property(readonly,nonatomic,retain)NSString *clientId;
@property(readonly,nonatomic,retain)NSString *clientSecret;
@property(readonly,nonatomic,retain)UIViewController *callerVc;
@property (nonatomic) AuthCredentials *authCredentials;
@property (nonatomic) CloudAuthRequestFactory *authReqFactory;

//@property(copy,nonatomic) void(^appCallback)(CDKError*);

@property(copy,nonatomic) void(^onSuccess)(void);
@property(copy,nonatomic) void(^onFailure)(CDKError*);
@property(readonly) NSString *identifier;

//-(id)init __attribute__((unavailable("Must Create CloudAuth object using \"initializeWith\" method")));

/**
 * Initialize object with parameters provided by the cloud
 * @param baseUrl the URL of the authentication server
 * @param clientId application identifier
 * @param clientSecret application secret needed to authenticate application to the cloud
 * @param vc the view controller for the new web view that will be launched
 *        to do the login.
 */
-(id)initWithBaseURL:(NSString*)baseUrl clientId:(NSString*)clientId
        clientSecret:(NSString*)clientSecret onViewController:(UIViewController*)vc;

/**
 * Initialize object with viewController,loads all cloud params from plist
 * @param vc the view controller for the new web view that will be launched
 *        to do the login.
 */
-(instancetype)initWithViewController:(UIViewController*)vc;

/**
 * Remove all the saved tokens from the keychains
 * @param identifier deletes the tokens from keychain based on identifer
 */
- (void)deleteSavedTokens:(NSString*)identifier;

/**
 * Get the user's authentication credentials. Note
 * that credentials may not contain any tokens if the
 * user is not logged in.
 */
- (AuthCredentials*)getAuthCredentials;

/**
 * Method which initiates validation of refresh token request
 * @param callback that will received the result
 */
- (void)validateRefreshToken:(void (^)(int statusCode, NSData* payLoad, CDKError *error))callback;

/**
 * Get the auth request factory for getting request
 */
- (CloudAuthRequestFactory *)getAuthRequestFactory;


/**
 *  login the user. Launches a web view if
 *  cached credentials are no longer valid.
 *
 *  @param onSuccess notifies app if login completed successfully.
 *  @param onFailure notifies app if login failure occurs.
 */
- (void)login:(void(^)(void))onSuccess failure:(void(^)(CDKError*))onFailure;

/**
 *  logout the user
 *  Basically remove the cached credentials which force a new login.
 */
- (void)logout;

/**
 *  function that does the actually logout. This should be override by the subclass and call
 */
- (void)onLogout;

/**
 * Method which initiates UAA Login process using 3rd party provider such as google, facebook,...etc.
 * @param email the user identifier
 * @param token the token of 3rd party provider or password for native UAA
 * @param provider identifies the 3rd party auth or native uaa
 * @param scope the scope for the user
 * @param onSuccess notifies app if login completed successfully.
 * @param onFailure notifies app if login failure occurs.
 */
- (void)loginNatively:(NSString *)email token:(NSString *)token
          provider:(NSString *)provider scope:(NSString *)scope
              success:(void(^)(void))onSuccess
              failure:(void(^)(CDKError*))onFailure;

/**
 *  login the user with a provided web view if
 *  cached credentials are no longer valid.
 *
 *  @param appWVC the view controller
 *  @param onSuccess notifies app if login completed successfully.
 *  @param onFailure notifies app if login failure occurs.
 */
- (void)login:(CloudAuthWebViewClient *)appWVC
      success:(void(^)(void))onSuccess
      failure:(void(^)(CDKError*))onFailure;



/**
 * getUserInfo
 * Get the user info for the current login user. User info contains user id, user name,
 *              given name, family anme, email address and login type. The JSON returned in the payload
 *
 * @param successCallback notifies app if login completed successfully.
 * @param failureCallback notifies app if login failure occurs.
 */
-(void)getUserInfo:(ICloudResponseSuccessCb)successCallback failure:(ICloudResponseFailureCb)failureCallback;


// Internal use only
- (NSString *)getHTTPAuthorizationString;

@end
