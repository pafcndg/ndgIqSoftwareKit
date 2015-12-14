//
//  JBPrintStream.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/17/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __PRINT_STREAM__
#define __PRINT_STREAM__

#import <Foundation/Foundation.h>
#import "JBOutputStream.h"
#import "JBDefines.h"
#import "JBArrays.h"

@interface PrintStream : OutputStream

-(void) printlnWithId:(id)o;

-(void) printWithId:(id)o;

-(void) printlnWithNSString:(NSString*)str;

-(void) printlnWithInt:(int)n;

-(void) printlnWithBoolean:(boolean)b;

-(void) printWithNSString:(NSString*)str;

-(void) formatWithNSString:(NSString*)format withNSObjectArray:(Array*)array;

-(void) printfWithNSString:(NSString*)format withNSObjectArray:(Array*)array;

@end

#endif