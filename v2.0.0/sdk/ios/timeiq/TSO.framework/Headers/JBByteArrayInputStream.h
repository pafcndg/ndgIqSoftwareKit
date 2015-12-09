//
//  JBByteArrayInputStream.h
//  TSO
//
//  Created by Bari Levi on 3/25/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import "JBInputStream.h"
#include "NSData+JavaAPI.h"

@interface ByteArrayInputStream : InputStream

-(instancetype) initWithByteArray:(ByteArray*)arr;

-(int) readWithByteArray:(ByteArray*)data;

-(int) readWithData:(ByteArray*)data withSize:(int)size;

-(int) readWithByteArray:(ByteArray*)data withInt:(int)offset withInt:(int)len;

- (void)skipWithLong:(int)n;

-(void) close;

-(int) available;

-(NSInputStream*) getNSInputStream;

@end
