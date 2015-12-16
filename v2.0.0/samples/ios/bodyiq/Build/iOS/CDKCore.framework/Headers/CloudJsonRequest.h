//
//  CloudJsonRequest.h
//  CloudSDK
//
//  Created by RaghavendraX Gutta on 07/08/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//
#import "CloudRequest.h"

@class CloudResponse;

/**
 * The successful response callback i.e. the corresponding cloud request was
 * processed successfully and resulted in a valid response.
 * @param task The successful request/response session
 * @param responseObject The response object received from the cloud
 */
typedef void (^SuccessResponse)(NSURLSessionDataTask *task, id responseObject);

/**
 * The cloud request failed and an error was returned as part of the response
 * @param The failed request/response session
 * @param error The error that occured
 */
typedef void (^FailureResponse)(NSURLSessionDataTask *task, NSError *error);

/**
 * Json request object for the cloud
 */
@interface CloudJsonRequest : CloudRequest

/**
 *  Schedule the request so its sent to network ASAP.
 *
 *  @param callback see ICloudResponseCb
 */
-(void)fire:(ICloudResponseCb)callback;

/**
 *  Schedule the request and block until it returns from the network.
 */
-(CloudResponse*)fireSynchronous;

@end
