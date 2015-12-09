//
//  NSTimeZone+JavaUtil.h
//  TSO
//
//  Created by Bari Levi on 3/23/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSTimeZone (TimeZone)

-(NSString*) getDisplayName;

+(NSTimeZone*)getTimeZoneWithNSString:(NSString*)ID;

-(NSString*)getID;

+(NSTimeZone*) getDefault;

@end

//typedef NSTimeZone TimeZone;