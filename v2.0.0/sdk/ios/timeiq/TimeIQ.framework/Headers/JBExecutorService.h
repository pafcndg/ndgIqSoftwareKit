//
//  JBExecutorService.h
//  TSO
//
//  Created by Bari Levi on 3/18/15.
//  Copyright (c) 2015 intel. All rights reserved.
//
#include <Foundation/Foundation.h>
#include "JBRunnable.h"
#import "JBCallable.h"
#import "JBTimeUnit.h"

@protocol ExecutorService <NSObject>

-(void)executeWithRunnable:(id<Runnable>)runnable;

-(void) shutdown;

-(BOOL) isShutdown;

-(void) setSuspended:(BOOL)isSuspende;

-(BOOL) isSuspended;

-(id) submitWithCallable:(id<Callable>)callable;

-(id) submitWithRunnable:(id<Runnable>)runnable;

-(boolean)isCancelled;

-(boolean)cancelWithBoolean:(boolean)mayInterruptIfRunning;

-(BOOL)awaitTerminationWithLong:(long64)var1 withTimeUnit:(TimeUnit *)var3;

@end

@interface JBExecutorService : NSObject <ExecutorService>
{

}
-(instancetype) init;

-(instancetype) initWithThreadNum:(int)n;

-(void) newCachedThreadPool;

-(void) executeWithRunnable:(id<Runnable>)runnable;

-(void) newSingleThreadExecutor;

@end
