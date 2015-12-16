#import <Foundation/Foundation.h>
#import "CDKError.h"


/**
 * Object in which the response from the cloud is returned
 */
@interface CloudResponse : NSObject

-(instancetype)init;
-(instancetype)initWithResponse:(CloudResponse*) response;
-(instancetype)initWithPayLoad:(NSData*)payLoad statusCode:(int)statusCode;
-(instancetype)initWithError:(CDKError*)error statusCode:(int)statusCode;
-(instancetype)initWithPayload:(NSData*)payLoad error:(CDKError*)error statusCode:(int)statusCode;
-(BOOL)hasErrorOccured;
/**
 * Get the Raw payload(byte buffer)
 */
-(NSData*)getRawPayload;

/**
 * Get the cloud http response code.
 */
-(int)getHttpStatusCode;

/**
 * Get the cloud json dictionary.
 */
-(NSDictionary*)getJsonPayload;

/**
 * Get the cloudError.
 */
-(CDKError*)getCloudError;

@end

/**
 *  An app callback that returns the cloud's response to the application.
 *
 *  @param cloudResponse contains the response payload and headers
 */
typedef void (^ICloudResponseSuccessCb)(CloudResponse *cloudResponse);

/**
 *  An app callback that returns the cloud's error response to the application.
 *
 *  @param error contains the cloud error response
 */
typedef void (^ICloudResponseFailureCb)(CloudResponse *cloudResponse);


