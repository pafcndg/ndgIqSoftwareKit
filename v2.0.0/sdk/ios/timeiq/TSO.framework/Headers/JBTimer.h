//
//  Timer.swift
//  TSO
//
//  Created by Bari.Levi on 9/14/14.
//  Copyright (c) 2014 intel. All rights reserved.
//
#ifndef __TIMER_TASK__
#define __TIMER_TASK__

#import "JBDefines.h"
#import "JBTimerTask.h"

@interface Timer: NSObject
{
    TimerTask* m_task;//TODO:Bari should support queue
    
    NSTimer* m_timer;
}
-(instancetype) init;

-(void) scheduleWithTimerTask:(TimerTask*)timerTask withLong:(long64)delay;

-(void) timerFireMethod:(NSTimer*) timer;

-(void) cancel;

-(int) purge;

@end
#endif
