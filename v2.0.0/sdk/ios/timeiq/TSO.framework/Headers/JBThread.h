//
//  NSThread+Thread.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/21/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Thread_h__
#define __JavaBridge__Thread_h__

#import <Foundation/Foundation.h>

#import "JBRunnable.h"
#import "NSThread+JavaAPI.h"
#import "NSArray+JavaAPI.h"

@interface Thread: NSObject <Runnable>
{
    NSThread* thread;
    id<Runnable> runnable;
}
-(instancetype) init;

-(instancetype) initWithRunnable:(id<Runnable>)runnable;

-(instancetype) initWithRunnable:(id<Runnable>)runnable withNSString:(NSString*) threadName;

-(void) interrupt;

- (void)start;

- (void)run;

+(void) sleepWithLong:(double)sleep;

+(Array*)getStackTrace;

@end

#endif
