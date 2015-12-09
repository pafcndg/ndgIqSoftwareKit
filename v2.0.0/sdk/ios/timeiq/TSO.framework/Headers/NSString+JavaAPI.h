//
//  NSString+JavaAPI.h
//  Java-Bridge
//
//  Created by Bari.Levi on 12/11/14.
//  Copyright (c) 2014 Intel. All rights reserved.
//

#ifndef __JavaBridge__NSString_h__
#define __JavaBridge__NSString_h__

#import <Foundation/Foundation.h>
#import "NSArray+JavaAPI.h"
#import "JBDefines.h"

#define CharToString(x) [NSString stringWithFormat:@"%c", x]
#define IntegerToString(x) [NSString stringWithFormat:@"%d",x]
#define FloatToString(x) [NSString stringWithFormat:@"%f",x]
#define DoubleToString(x) [NSString stringWithFormat:@"%f",x]
#define Long64ToString(x) [NSString stringWithFormat:@"%lld", x]
#define Long32ToString(x) [[NSNumber numberWithLong:x] stringValue]
#define BooleanToString(x) (x? @"true" : @"false")

@interface NSString (String)

- (BOOL)equals:(NSString *)aString;

- (BOOL)equalsIgnoreCase:(NSString *)aString;

-(int)lastIndexOfString:(NSString*)s;

- (NSString *)substring:(int)beginIndex
               endIndex:(int)endIndex;

- (NSString *)substring:(int)beginIndex;

- (NSString *)subSequenceFrom:(int)beginIndex
                 to:(int)endIndex;

-(boolean) isEmpty;

-(unsigned long) hashCode;

-(Array*) split:(NSString*) delimiter;

-(NSString*) subString:(int)from to:(int)to;

-(NSString*) replace:(NSString*)fromStr to:(NSString*)toStr;

-(NSString*) replace:(NSString*)fromStr withSequence:(NSString*)sequence;

-(NSString*) replaceAll:(NSString*)strFrom withReplacement:(NSString*)strTo;

-(ByteArray *)getBytes;

-(boolean) contains:(NSString*)str;

-(NSString*) trim;

+(NSString*)formatWithNSString:(NSString*)str withNSObjectArray:(Array*)arr;

+(NSString*) valueOfInt:(int)number;

+(NSString*) valueOfLong:(long64)number;

+(NSString*) valueOf:(NSString*)str;

+(NSString* )stringWithBytes:(ByteArray*)arr charsetName:(NSString*)charsetName;

+(NSString*) stringWithBytes:(ByteArray*)arr;

-(int) indexOfString:(NSString*)str;

-(int) lastIndexOf:(char)c;

-(char16)charAtWithInt:(int)index;

-(int)sequenceLength;
@end

#endif