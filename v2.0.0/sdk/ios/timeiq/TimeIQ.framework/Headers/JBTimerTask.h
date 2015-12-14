//
//  TimerTask.h
//  TSO
//
//  Created by Bari Levi on 4/1/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"
#import "JBRunnable.h"

@interface TimerTask: NSObject <Runnable>

/* Cancels the TimerTask and removes it from the Timer's queue.*/
-(boolean) cancel;

/*The task to run should be specified in the implementation of the run() method.*/
-(void) run;

/* Returns the scheduled execution time. */
-(long64)scheduledExecutionTime;

@end