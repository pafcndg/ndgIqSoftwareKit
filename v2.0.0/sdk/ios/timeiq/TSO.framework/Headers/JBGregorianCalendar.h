//
//  GregorianCalendar.h
//  TSO
//
//  Created by Nir Bitton on 8/17/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"

@interface GregorianCalendar : NSObject
{
    long64 NSCalendar_currentTimeNillis;
}

-(instancetype)initWithInt:(int)year withInt:(int)month withInt:(int)dayOfMonth;

-(instancetype)initWithInt:(int)year withInt:(int)month withInt:(int)dayOfMont withInt:(int)hourOfDay withInt:(int)minute withInt:(int)second withInt:(int)millis;

-(long64)getTimeInMillis;

@end
