//
//  CloudError.h
//  CloudSDK
//
//  Created by Akshay Dua on 5/6/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>

extern const NSInteger CDK_ERROR_CODE_GENERIC;

extern NSString*const CDK_ERROR_DOMAIN_NET;
extern NSString*const CDK_ERROR_DOMAIN_HTTP;
extern NSString*const CDK_ERROR_DOMAIN_UNEXPECTED;
extern NSString*const CDK_ERROR_DOMAIN_LOGIN;
extern NSString*const CDK_ERROR_DOMAIN_JSON;

@interface CDKError : NSError

@property (nonatomic) NSString* message;

- (id) initWithError:(NSError*)err;
- (id) initWithMessage:(NSString*)msg code:(NSInteger)code domain:(NSString*)domain;
- (id) initWithMessage:(NSString*)msg domain:(NSString*)domain;

@end
