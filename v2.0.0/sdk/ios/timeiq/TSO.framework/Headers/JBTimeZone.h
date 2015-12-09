//
//  JBTimeZone.h
//  TSO
//
//  Created by Gal Shirin on 9/9/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TimeZone : NSObject

-(instancetype)init;

-(NSString*) getDisplayName;

+(TimeZone*)getTimeZoneWithNSString:(NSString*)ID;

-(NSString*)getID;

+(TimeZone*) getDefault;

-(void)setRawOffsetWithInt:(int)offsetInMillis;

+(TimeZone*) localTimeZone;

-(NSTimeZone*) getNSTimeZone;

@end
