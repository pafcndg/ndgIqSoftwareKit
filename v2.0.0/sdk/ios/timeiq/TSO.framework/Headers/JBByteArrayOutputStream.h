//
//  JBByteArrayOutputStream.h
//  TSO
//
//  Created by Bari Levi on 3/25/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import "JBOutputStream.h"
#import "JBDefines.h"

@interface ByteArrayOutputStream : OutputStream

-(instancetype) init;

-(instancetype) initWithInt:(int)size;

-(void) writeWithByteArray:(NSData*)data;

-(void) writeWithByteArray:(NSData*)data withOffset:(int)offset withLen:(int)len;

-(void) writeWithByteArray:(NSData*)b withInt:(int)offset withInt:(int)len;

-(void) reset;

- (ByteArray *)toByteArray;

-(int) size;

-(void) flush;

-(void) close;

@end
