//
//  JBNumberFormat.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/15/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__NumberFormat_h__
#define __JavaBridge__NumberFormat_h__

#import <Foundation/Foundation.h>

@interface NumberFormat : NSObject
{
    NSNumberFormatter* mNumberFormat;
}

+(NumberFormat*) getNumberInstance;

-(void) setMaximumFractionDigitsWithInt:(int)num;
-(NSString*) formatWithDouble:(double)d;

@end

#endif
