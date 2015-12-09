//
//  JBFileReader.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/22/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__FileReader_h__
#define __JavaBridge__FileReader_h__

#import <Foundation/Foundation.h>
#import "JBReader.h"
#import "JBFile.h"
#import "JBFileInputStream.h"

@interface FileReader : Reader
{
    FileInputStream* fis;
}

@property (nonatomic, copy) NSString * lineDelimiter;
@property (nonatomic) NSUInteger chunkSize;

- (instancetype) initWithFile:(File *)file;
- (instancetype)initWithNSString:(NSString *)aPath;

-(int) read;

-(int) readWithData:(NSMutableData*) buf withOffset:(int) off withLen:(int)len;

-(int) readWithCharArray:(NSMutableData*) buf;

-(boolean) ready;

-(void) reset;

-(void) close;

-(void) skipBytes:(int)n;

#if NS_BLOCKS_AVAILABLE
- (void) enumerateLinesUsingBlock:(void(^)(NSString*, BOOL *))block;
#endif

@end

#endif