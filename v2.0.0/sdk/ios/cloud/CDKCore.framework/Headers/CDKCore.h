/**
 * Provides authentication and HTTP request handing functionality
 * common to all cloud SDK modules
 */

#import <UIKit/UIKit.h>

#import "CloudResponse.h"
#import "CloudAuthProvider.h"
#import "CloudAppAuthProvider.h"
#import "CloudFacebookAuthProvider.h"
#import "CloudGoogleAuthProvider.h"
#import "CloudUAAAuthProvider.h"
#import "CloudAuthWebViewClient.h"
#import "CloudAuthRequestFactory.h"
#import "AuthCredentials.h"
#import "CDKError.h"
#import "CDKHTTPError.h"
#import "CDKJSONError.h"
#import "CDKCredentialDecodeError.h"
#import "CDKNetworkError.h"
#import "CDKUnexpectedError.h"
#import "CDKUserNotLoggedInError.h"
//Temporary imports-Importing headers necessary to modules which are getting swiftified
#import "CloudRequest.h"
#import "CloudJsonRequest.h"
#import "CloudDownloadRequest.h"
#import "CloudMultipartRequest.h"
#import "AuthCredentials+SetTokens.h"
#import "NSFileManager+hashCalculator.h"
#import "CloudConfigurationHelper.h"

//! Project version number for Core.
FOUNDATION_EXPORT double CoreVersionNumber;

//! Project version string for Core.
FOUNDATION_EXPORT const unsigned char CoreVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <Core/PublicHeader.h>


