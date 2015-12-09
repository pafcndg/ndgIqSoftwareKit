//
//  JBFutureTask.h
//  TSO
//
//  Created by Nir Bitton on 7/13/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBRunnable.h"
#import "JBCallable.h"
#import "JBTimeUnit.h"

@interface FutureTask : NSObject <Runnable>
{
    id<Callable> callable;
}

-(instancetype)init;

-(instancetype)initWithCallable:(id<Callable>)call;

- (void)run;

-(id)getWithLong:(long64)l withTimeUnit:(TimeUnit*)tu;

@end
