//
//  Executors.h
//  TSO
//
//  Created by Bari Levi on 3/18/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBExecutorService.h"
#include "JBRunnable.h"
#import "JBCallable.h"

@interface Executors : NSObject

+(id<ExecutorService>) newFixedThreadPoolWithInt:(int)n;

+(id<ExecutorService>) newCachedThreadPool;

+(id<ExecutorService>) newSingleThreadExecutor;

@end

@protocol Executor

-(void) executeWithRunnable:(id<Runnable>)runnable;

@end

@interface ExecutorImpl : NSObject <Executor>

@end
