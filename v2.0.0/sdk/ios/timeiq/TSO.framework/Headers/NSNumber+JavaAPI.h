//
//  NSNumber+JavaAPI.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/16/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Number_h__
#define __JavaBridge__Number_h__

#import <Foundation/Foundation.h>
#import "JBDefines.h"

//typedef NSNumber Integer;
//typedef NSNumber Float;
//typedef NSNumber JBDouble;
//typedef NSNumber Long;
//typedef NSNumber JBBoolean;
//typedef NSNumber JBShort;

#define Integer NSNumber
#define Float NSNumber
#define JBDouble NSNumber
#define Long NSNumber
#define JBBoolean NSNumber
#define JBShort NSNumber
#define JBByte NSNumber
#define Character NSNumber

#define Integer_MAX_VALUE INT_MAX
#define Integer_MIN_VALUE INT_MIN
#define Long_MAX_VALUE LONG_MAX
#define Long_MIN_VALUE LONG_MIN
#define JBDouble_MAX_VALUE DBL_MAX
#define JBDouble_MIN_VALUE DBL_MIN

#define JBBoolean_get_FALSE__() false

#define booleanValue boolValue

void PostDecrInt(Integer** i);
void PostIncrInt(Integer** i);

@interface NSNumber (Number)

+(int) compareWithDouble:(double) d1 withDouble:(double)d2;
+(int) compareWithFloat:(float) f1 withFloat:(float)f2;
+(int) compareWithLong:(long64) l1 withLong:(long64)l2;
+(int) compareWithInt:(int) i1 withInt:(int)i2;
-(int) compareToWithId:(NSNumber*)num;


+(JBDouble*)    valueOfWithDouble:(double)d;
+(Float*)       valueOfWithFloat:(float)f;
+(Long*)        valueOfWithLong:(long64)l;
+(Long*)        valueOfWithNSString:(NSString*)str;
+(Integer*)     valueOfWithInt:(int)n;
+(JBShort*)     valueOfWithShort:(short)srt;
+(JBBoolean*)   valueOfWithBoolean:(boolean)b;

+(double)       parseDoubleWithNSString:(NSString*)str;
+(float)        parseFloatWithNSString:(NSString*)str;
+(long64)       parseLongWithNSString:(NSString*)str;
+(int)          parseIntWithNSString:(NSString*)str;
+(short)        parseShortWithNSString:(NSString*)str;
+(boolean)      parseBooleanWithNSString:(NSString*)str;

+(NSString*)    toStringWithDouble:(double)d;
+(NSString*)    toStringWithFloat:(float)number;
+(NSString*)    toStringWithLong:(long64)number;
+(NSString*)    toStringWithInt:(int)number;
+(NSString*)    toStringWithShort:(short)number;
+(NSString*)    toStringWithBoolean:(boolean)b;

+(long64) doubleToLongBitsWithDouble:(double)d;
+(int) floatToIntBitsWithFloat:(float) value;

+(boolean) isWhitespaceWithChar:(char16)c;

@end

#endif