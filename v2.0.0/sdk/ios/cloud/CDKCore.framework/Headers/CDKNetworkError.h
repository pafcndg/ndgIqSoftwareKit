//
//  CDKTimeoutError.h
//  CloudSDK
//
//  Created by Akshay Dua on 5/11/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import "CDKError.h"

@interface CDKNetworkError : CDKError

- (id) initWithMessage:(NSString*)msg;
- (id) initWithError:(NSError*)err statusCode:(NSInteger)statusCode;

@end
