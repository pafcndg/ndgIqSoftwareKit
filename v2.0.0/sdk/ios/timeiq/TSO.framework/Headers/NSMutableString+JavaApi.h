//
//  NSMutableString+JavaApi.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/17/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__StringBuffer_h__
#define __JavaBridge__StringBuffer_h__

#import <Foundation/Foundation.h>
#import "JBComparator.h"

@interface NSMutableString (String)

- (NSMutableString*) initWithNSString:(NSString*)aString;

- (NSMutableString*) initWithInt:(int)capacity;

- (NSMutableString*)append:(NSString*)aString;

- (NSMutableString*)appendWithDouble:(double)d;

- (NSMutableString*)appendWithBoolean:(boolean)b;

- (NSMutableString*)appendWithChar:(char)c;

- (NSMutableString*)appendWithLong:(long64)l;

- (NSMutableString*)appendWithInt:(int)num;

- (NSMutableString*)appendWithNSString:(NSString*)str;

- (NSMutableString*)appendWithId:(id)id_;

- (NSMutableString*) replace:(NSString*)orig with:(NSString*)dest;

- (NSMutableString*) replaceWithInt:(int)start withInt:(int)end withNSString:(NSString*) str;

- (NSMutableString*) appendWithFloat:(float)f;

-(NSUInteger) sequenceLength;

@end

#endif
