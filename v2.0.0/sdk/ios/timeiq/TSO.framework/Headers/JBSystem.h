//
//  System.h
//  Java-Bridge
//
//  Created by Bari.Levi on 11/24/14.
//  Copyright (c) 2014 Intel. All rights reserved.
//

#ifndef __JavaBridge__System_h__
#define __JavaBridge__System_h__

#include <Foundation/Foundation.h>
#include "JBPrintStream.h"
#include "NSArray+JavaAPI.h"
#include "nanotime.h"

@interface System : NSObject

+(long64) currentTimeMillis;

+(PrintStream*) out;

+(PrintStream*) err;

+(void) arraycopyWithId:(Array*)origArray withInt:(int)origStartIndex withId:(Array*)destArray withInt:(int)destStartIndex withInt:(int)size;


+(NSUInteger)identityHashCodeWithId:(id)obj;

+(long64)nanoTime;

+(NSString*)getPropertyWithNSString:(NSString*)key;


@end

#endif 
