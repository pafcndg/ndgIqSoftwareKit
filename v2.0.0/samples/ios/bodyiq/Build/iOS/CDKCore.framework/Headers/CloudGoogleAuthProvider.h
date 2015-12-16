//
//  CloudGoogleAuthProvider.h
//  CloudSDK
//
//  Handle Google login
//
#import "CloudAuthProvider.h"

/**
 * Handles the authentication flows for Google and login.
 */
@interface CloudGoogleAuthProvider : CloudAuthProvider


/**
 * Initialize object with parameters provided by the cloud
 * @param baseUrl the URL of the authentication server
 * @param clientId application identifier
 * @param clientSecret application secret needed to authenticate application to the cloud
 * @param googleClientId the Google application id
 * @param vc the view controller
 */
-(id)initWithBaseURL:(NSString*)baseUrl clientId:(NSString*)clientId clientSecret:(NSString*)clientSecret
      googleClientId:(NSString *)googleClientId onViewController:(UIViewController*)vc;

/**
 * Initialize object with params
 * @param googleClientId the Google application id
 * @param vc the view controller
 */
-(instancetype)initWithGoogleClientId:(NSString *)googleClientId onViewController:(UIViewController*)vc;

/**
 * Method which initiates Google Login process
 *
 * @param onSuccess notifies app if login completed successfully.
 * @param onFailure notifies app if login failure occurs.
 */
- (void)login:(void(^)(void))onSuccess
      failure:(void(^)(CDKError*))onFailure;

/**
 * This is just a pass thru to the Google SDK.
 *
 * Call this method from the [UIApplicationDelegate application:openURL:sourceApplication:annotation:] method
 * of the AppDelegate for your app. It should be invoked for the proper processing of responses during interaction
 * with the native Google Plus app or Safari as part of SSO authorization flow or Google dialogs.
 *
 * @param application The application as passed to [UIApplicationDelegate application:openURL:sourceApplication:annotation:].
 * @param url The URL as passed to [UIApplicationDelegate application:openURL:sourceApplication:annotation:].
 * @param sourceApplication The sourceApplication as passed to [UIApplicationDelegate application:openURL:sourceApplication:annotation:].
 * @param annotation The annotation as passed to [UIApplicationDelegate application:openURL:sourceApplication:annotation:].
 *
 * @return YES if the url was intended for the Google SDK, NO if not.
 */
+ (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication
         annotation:(id)annotation;

/**
 * This is just a pass thru to the Google SDK
 *
 * Call this method from the [UIApplicationDelegate application:didFinishLaunchingWithOptions:] method
 * of the AppDelegate for your app. It should be invoked for the proper use of the Google SDK.
 *
 * @param application The application as passed to [UIApplicationDelegate application:didFinishLaunchingWithOptions:].
 * @param launchOptions The launchOptions as passed to [UIApplicationDelegate application:didFinishLaunchingWithOptions:].
 *
 * @return YES if the url was intended for the Google SDK, NO if not.
 */
+ (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;

/**
 * This is just a pass thru to the Google SDK
 * @param application The application as passed to Google SDK
 */
+ (void)applicationDidBecomeActive:(UIApplication *)application;
@end
