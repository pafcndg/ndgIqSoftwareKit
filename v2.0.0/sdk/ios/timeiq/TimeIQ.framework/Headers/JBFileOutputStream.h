//
//  JBFileOutputStream.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/23/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__FileOutputStream_h__
#define __JavaBridge__FileOutputStream_h__

#import <Foundation/Foundation.h>
#import "JBFile.h"
#import "NSDate+JavaAPI.h"
#import "JBOutputStream.h"
#import "JBFileChannel.h"

@interface FileOutputStream : OutputStream
{
    File* m_file;
    NSOutputStream* m_oStream;
    FileChannel* channel;
}
-(instancetype) initWithNSString:(NSString*)fileName;

-(instancetype) initWithFile:(File*)file;

-(void) writeWithByteArray:(ByteArray*)bytes;

-(FileChannel*)getChannel; 

-(void)close;

-(void)flush;

@end

#endif
