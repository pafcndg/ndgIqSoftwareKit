//
//  JBArrays.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/22/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Arrays_h__
#define __JavaBridge__Arrays_h__

#import <Foundation/Foundation.h>
#include "NSArray+JavaAPI.h"
#import "JBComparator.h"
#import "JBNativeArray.h"

@interface Arrays : NSObject

+(Array*) copyOfRangeWithNSObjectArray:(Array*)arr withInt:(int)start withInt:(int)end;

+(Array*) asListWithNSObjectArray:(Array*)arr;

+(void) sortWithNSObjectArray:(Array*)arr withIComparator:(id<IComparator>)comparator;

+(void) sortWithLongArray:(IOSIntArray *)longArray;

+(void) fillWithIntArray:(IOSIntArray*)arr withInt:(int)i;

+(void) fillWithNSObjectArray:(Array*)arr withId:(id)item;

+(NSString*) toStringWithNSObjectArray:(Array*)arr;

@end

#endif
