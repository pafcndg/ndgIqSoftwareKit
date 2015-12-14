//
//  JBBufferedInputStream.h
//  TSO
//
//  Created by AviadX Ganon on 20/10/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBInputStream.h"

@interface BufferedInputStream : InputStream

-(instancetype)initWithInputStream:(InputStream*)is;

-(instancetype)initWithInputStream:(InputStream*)is withInt:(int)size;

-(int)readWithByteArray:(NSMutableData*)data withInt:(int)i withInt:(int)buffer;

-(int)readWithData:(NSMutableData*)data withSize:(int)size;

- (void)skipBytes:(int)n;

-(void) close;

-(boolean) hasBytesAvailable;

-(int) available;

-(NSInputStream*) getNSInputStream;


@end
