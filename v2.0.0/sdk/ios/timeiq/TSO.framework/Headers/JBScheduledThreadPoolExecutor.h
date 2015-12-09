//
//  JBScheduledThreadPoolExecutor.h
//  TSO
//
//  Created by Nir Bitton on 02/11/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBScheduledFuture.h"
#import "JBRunnable.h"
#import "JBCallable.h"
#import "JBTimeUnit.h"
#import "JBExecutorService.h"

@interface ScheduledThreadPoolExecutor : NSObject
{
    JBExecutorService *executorService;
}

-(instancetype)init;

-(instancetype)initWithInt:(int)corePoolSize;

-(id<ScheduledFuture>)scheduleWithRunnable:(id<Runnable>)runnable withLong:(long64)delay withTimeUnit:(TimeUnit*)timeUnit;

-(id<ScheduledFuture>)scheduleWithCallable:(id<Callable>)callable withLong:(long64)delay withTimeUnit:(TimeUnit*)timeUnit;

-(boolean)isCancelled;

-(boolean)cancelWithBoolean:(boolean)mayInterruptIfRunning;

-(void)shutdown;

-(boolean) awaitTerminationWithLong:(long64)timeout withTimeUnit:(TimeUnit*)unit;

@end
