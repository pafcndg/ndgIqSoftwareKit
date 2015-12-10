//
//  CloudRequest.h
//  CloudSDK
//
//  Created by RaghavendraX Gutta on 07/08/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CDKLogging.h"

@class AFHTTPSessionManager;
@class AuthCredentials;
@class CloudAuthProvider;
@class CDKError;

/**
 * Default cloud request timeout in seconds. Timeout implies that
 * cloud does not respond to request within stated timeout value.
 */
#define DEFAULT_TIMEOUT_INTERVAL 20

/**
 * When a request fails, the time in seconds to wait
 * before retrying.
 */
#define RETRY_INTERVAL 20

/**
 * Number of times to retry
 */
#define TIMES_TO_RETRY 3

/**
 * An empty response from the cloud is returned in the
 * JSON format. The response is a text string accessible
 * via the following key
 */
#define EMPTY_RESPONSE_KEY @"Response"

// Removed when we remove the code in CloudRequest.m to ignore SSL Cert because of
// pre-production server using a self sign cert.
#define OAKLEY_BASE_URL  @"https://hyb-pre-prod-www.oakley.com"

/**
 * Empty cloud responses contain the following JSON data
 */
#define EMPTY_RESPONSE_VALUE @"Response does not contain any data."


#define BACKGROUND_DOWNLOAD_ID @"sdk.cloud.backgroundDownload"

#define BACKGROUND_UPLOAD_ID @"sdk.cloud.backgroundUpload"

/**
 * Cloud HTTP/HTTPS request type enumeration.
 */
typedef enum MethodType{
    GET,
    POST,
    PUT,
    DELETE,
    NONE
}HttpMethodType;

/**
 * A cloud request that results in a long-term cloud response (e.g. file download)
 * is classified as a BACKGROUND_SESSION. This also implies that it is OK for
 * the device to reboot or the app to go to sleep; when the long-term response
 * is received completely the app notified via a callback. See the BLOB STORE
 * API for more information.
 */
typedef enum SessionType{
    DEFAULT_SESSION,
    BACKGROUND_DOWNLOAD_SESSION,
    BACKGROUND_UPLOAD_SESSION
}HttpSessionType;


/**
 * The application callback that returns the response. Not to be confused
 * with SuccessResponse and FailureResponse which are callbacks the SDK
 * receives from the operating system.
 */
typedef void (^ICloudResponseCb)(int statusCode, NSData* payLoad, CDKError *error);

/**
 * Request object for the cloud
 */
@interface CloudRequest : NSObject

/**
 * The base server URL
 */
@property(readonly,nonatomic,retain)NSString *baseUrl;

/**
 * The request endpoint i.e. the portion that is appended
 * to the base URL to form the entire request URL
 */
@property(readonly,nonatomic,retain)NSString *urlEndPoint;

/**
 * GET, POST, PUT etc.
 */
@property(readonly,nonatomic,assign)HttpMethodType methodType;

/**
 * HTTP headers on this request.
 */
@property(readonly,nonatomic,retain)NSDictionary *headers;

/**
 * The body in key/value pairs. Will be translated to JSON or other appropriate
 * format as necessary.
 */
@property(readonly,nonatomic,retain)NSDictionary *bodyParams;

/**
 * Short term request/response session or long-term (e.g. download)
 */
@property(readonly,nonatomic,assign)HttpSessionType sessionType;

/**
 * Request needs to be authenticated by cloud first or not
 */
@property(readonly,nonatomic,assign)AuthCredentials *creds;

/**
 * Flag which tracks refreshing token request.
 */
@property(readonly,nonatomic,assign)BOOL isTokenRefreshing;

@property(assign,nonatomic) int originalRetryCount;

@property(assign,nonatomic) int retriesRemainingCount;

-(id)init __attribute__((unavailable("Must Create CloudRequest object using \"initCloudRequestWith\" method")));

/**
 * create the request
 * @param baseUrl see property with same name
 * @param urlEndPoint see property with same name
 * @param methodType see property with same name
 * @param bodyParams see property with same name
 * @param headers see property with same name
 * @param sessionType see property with same name
 * @param creds the authentication credentials
 */
-(id)initCloudRequestWith:(NSString*)baseUrl
                 endPoint:(NSString*)urlEndPoint
           httpMethodType:(HttpMethodType)methodType
               formParams:(NSDictionary*)bodyParams
              httpHeaders:(NSDictionary*)headers
          httpSessionType:(HttpSessionType)sessionType
              credentials:(AuthCredentials *)creds;

/**
 * Sets the isTokenRefreshing property.
 *
 * @param isTokenRefreshing boolean value indicating YES or NO
 */
-(void)setIsTokenRefreshing:(BOOL)isTokenRefreshing;

-(void)setHeaders:(NSDictionary *)headers;

- (AFHTTPSessionManager*)getSessionManager:(HttpSessionType)sessionType;

+ (AFHTTPSessionManager *)defaultManager;

-(BOOL)removeFileAtPath:(NSString*)path;

-(NSData*)createEmptyResponsePayLoad;

+(void)setCompletionHandler:(void(^)())handler onSessionId:(NSString *)identifier;

@end
