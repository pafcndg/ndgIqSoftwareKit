//
//  CDKUnknownError.h
//  CloudSDK
//
//  Created by Akshay Dua on 5/11/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import "CDKError.h"

@interface CDKUnexpectedError : CDKError

- (id) initWithMessage:(NSString*)msg;
- (id) initWithMessage:(NSString*)msg code:(NSInteger)code;

@end
