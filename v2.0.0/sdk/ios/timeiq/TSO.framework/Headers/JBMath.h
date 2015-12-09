//
//  JBMath.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/15/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Math_h__
#define __JavaBridge__Math_h__

#import <Foundation/Foundation.h>
#import "JBDefines.h"

#define Math_PI M_PI

#define Math_E M_E

@interface Math : NSObject

+(double) sqrtWithDouble:(double)d;

+(int) minWithInt:(int)i1 withInt:(int)i2;

+(int) maxWithInt:(int)d1 withInt:(int)d2;

+(double) minWithDouble:(double)d1 withDouble:(double)d2;

+(double) maxWithDouble:(double)d1 withDouble:(double)d2;

+(long64) minWithLong:(long64)d1 withLong:(long64)d2;

+(long64) maxWithLong:(long64)d1 withLong:(long64)d2;

+(int) absWithInt:(int)i;

+(double) absWithDouble:(double)d;

+(double) absWithLong:(long64)l;

+(double) ceilWithDouble:(double)d;

+(double) sinWithDouble:(double)d;

+(double) cosWithDouble:(double)d;

+(double) atan2WithDouble:(double)d1 withDouble:(double)d2;

+(double) powWithDouble:(double)d1 withDouble:(double)d2;

+(double) expWithDouble:(double)d;

+(double) asinWithDouble:(double)d;

+(double) roundWithDouble:(double)d;

+(double) logWithDouble:(double)d;

+(double) floorWithDouble:(double)d;

+(double) random;

@end

#endif
