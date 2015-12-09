//
//  JBPrintWriter.h
//  places-api
//
//  Created by Bari Levi on 3/1/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBWriter.h"
#import "JBDefines.h"
#import "JBOutputStream.h"
#import "JBFile.h"

@interface PrintWriter : Writer
{
@protected

    Writer* out_;
//    StringBuffer* _buf;
    bool autoFlush;
    
}

-(instancetype)initWithWriter:(Writer*)w;

-(instancetype)initWithNSString:(NSString*)str; // TODO AVIAD

-(instancetype)initWithOutputStream:(OutputStream*)outputStream;

-(instancetype)initWithFile:(File*)file;

-(instancetype)initWithFileName:(NSString*)fileName;

-(void) writeWithNSString:(NSString*)buf;

-(void) flush;

-(void) close;

-(void) printWithBoolean:(bool)b;

-(void) printWithChar:(char)c;

-(void) printWithDouble:(double)d;

-(void) printWithFloat:(float)f;

-(void) printWithInt:(int)i;

-(void) printWithLong:(long64)l;

-(void) printId:(id)object;

-(void) printWithNSString:(NSString*)s;

-(void) println;

-(void) reset;

@end
