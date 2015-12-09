//
//  JBBufferedReader.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/22/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__BufferedReader_h__
#define __JavaBridge__BufferedReader_h__

#import <Foundation/Foundation.h>
#import "JBFileReader.h"
#import "JBReader.h"
#import "JBNativeArray.h"

@interface BufferedReader : Reader
{
    Reader* in;

    NSMutableString* cb;
}
-(instancetype) initWithReader:(Reader*)fr;

-(NSString*) readLine;

-(int) readWithData:(NSMutableData*)array withInt:(int)offset withInt:(int)len;

-(int) readWithCharArray:(IOSCharArray*)array withInt:(int)offset withInt:(int)len;

-(void) close;

-(void)skipWithLong:(long64)n;

@end

#endif