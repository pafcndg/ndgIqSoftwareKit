//
//  JBSimpleDateFormat.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/22/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__SimpleDateFormat_h__
#define __JavaBridge__SimpleDateFormat_h__

#import <Foundation/Foundation.h>
#import "NSDate+JavaAPI.h"
#import "JBLocale.h"
#import "JBTimeZone.h"

@interface JBDateFormatter: NSObject
{
    NSDateFormatter* dateFormatter;
}
-(instancetype) initWithNSString:(NSString*)str;

-(instancetype) initWithNSString:(NSString*)str withJavaUtilLocale:(Locale*)l;

-(NSString*) formatWithDate:(Date*) date;

-(Date*) parseWithNSString:(NSString*)str;

-(void)setTimeZoneWithTimeZone:(TimeZone*)tz;

-(NSString*) formatWithId:(id) obj;

@end
#define SimpleDateFormat JBDateFormatter
#define DateFormat JBDateFormatter 
//typedef JBDateFormatter SimpleDateFormat;
//typedef JBDateFormatter DateFormat;

#endif
