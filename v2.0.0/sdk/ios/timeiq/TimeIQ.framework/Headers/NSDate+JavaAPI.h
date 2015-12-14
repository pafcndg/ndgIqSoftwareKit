//
//  JBDate.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/18/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Date_h__
#define __JavaBridge__Date_h__

#import <Foundation/Foundation.h>
#import "NSObject+JavaAPI.h"

typedef NSDate Date;

@interface NSDate (Date_JavaAPI)
{
    
}
-(instancetype)initWithLong:(long64)l;

-(long64) getTime;

-(int)getMinutes;

@end

#endif
