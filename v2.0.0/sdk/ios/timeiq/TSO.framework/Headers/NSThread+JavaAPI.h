//
//  NSThread+JavaApi.h
//  TSO
//
//  Created by Bari Levi on 3/24/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NSNumber+JavaAPI.h"

@interface NSThread (JavaApi)

+(void) sleepWithLong:(long64)ms withInt:(int)ns;

@end
