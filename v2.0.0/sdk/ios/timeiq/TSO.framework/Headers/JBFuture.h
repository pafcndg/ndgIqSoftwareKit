//
//  JBFuture.h
//  TSO
//
//  Created by Bari Levi on 6/28/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBTimeUnit.h"

@protocol Future

-(id) getWithLong:(long64)l withTimeUnit:(TimeUnit*)tu;

-(id)get;

-(boolean)isCancelled;

-(boolean)cancelWithBoolean:(boolean)mayInterruptIfRunning;

@end
