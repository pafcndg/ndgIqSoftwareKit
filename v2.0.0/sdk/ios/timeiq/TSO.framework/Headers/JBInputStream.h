//
//  NSInputStream+InputStream.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/22/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__InputStream_h__
#define __JavaBridge__InputStream_h__

#import <Foundation/Foundation.h>
#import "JBArrays.h"

@interface InputStream: NSObject

-(int)readWithData:(NSMutableData*)data withSize:(int)size;//ABSTRUCT

- (void)skipBytes:(int)n;

-(void) close;

-(boolean) hasBytesAvailable;

-(int) available;

-(NSInputStream*) getNSInputStream;

@end

#endif
