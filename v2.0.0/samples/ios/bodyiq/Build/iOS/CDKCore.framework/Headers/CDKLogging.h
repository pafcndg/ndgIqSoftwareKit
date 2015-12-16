//
//  CDKLogging.h
//  CloudSDK
//
//  Created by RaghavendraX Gutta on 01/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

#ifndef CDKLogging_h
#define CDKLogging_h
#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, LogLevel) {
    CLDSDK_EMERGENCY,
    CLDSDK_ALERT,
    CLDSDK_CRITICAL,
    CLDSDK_ERROR,
    CLDSDK_WARNING,
    CLDSDK_NOTICE,
    CLDSDK_INFO,
    CLDSDK_DEBUG,
};

void printLog(LogLevel level, NSString * message);

#endif /* CDKLogging_h */
