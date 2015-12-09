//
//  JBFileInputStream.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/23/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__FileInputStream_h__
#define __JavaBridge__FileInputStream_h__

#import <Foundation/Foundation.h>

#include "JBInputStream.h"
#import "JBFile.h"

@interface FileInputStream : InputStream
{
    File* m_file;
    NSInputStream* m_is;
}

-(instancetype) initWithFile:(File*)file;

-(instancetype) initWithNSString:(NSString*)file;

-(int) readWithData:(NSMutableData*)data withSize:(int)size;

-(int) readWithByteArray:(NSMutableData*)data;

-(void) skipBytes:(int)n;

-(void) close;

-(boolean) hasBytesAvailable;

-(NSInputStream*) getNSInputStream;

@end

#endif
