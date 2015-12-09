//
//  JBCalendar.h
//  TSO
//
//  Created by Gal Shirin on 9/30/15.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NSDate+JavaAPI.h"
#import "JBTimeZone.h"

enum
{
    Calendar_YEAR = 1,
    Calendar_MONTH = 2,
    Calendar_WEEK_OF_YEAR = 3,
    Calendar_DATE = 5,
    Calendar_DAY_OF_MONTH = 5,
    Calendar_DAY_OF_YEAR = 6,
    Calendar_DAY_OF_WEEK = 7,
    Calendar_DAY_OF_WEEK_IN_MONTH = 8,
    Calendar_AM_PM = 9,
    Calendar_HOUR = 10,
    Calendar_HOUR_OF_DAY = 11,
    Calendar_MINUTE = 12,
    Calendar_SECOND = 13,
    Calendar_MILLISECOND = 14,
};

enum
{
    Calendar_SUNDAY = 1,
    
    /**
     * Value of the {@link #DAY_OF_WEEK} field indicating
     * Monday.
     */
    Calendar_MONDAY = 2,
    
    /**
     * Value of the {@link #DAY_OF_WEEK} field indicating
     * Tuesday.
     */
    Calendar_TUESDAY = 3,
    
    /**
     * Value of the {@link #DAY_OF_WEEK} field indicating
     * Wednesday.
     */
    Calendar_WEDNESDAY = 4,
    
    /**
     * Value of the {@link #DAY_OF_WEEK} field indicating
     * Thursday.
     */
    Calendar_THURSDAY = 5,
    
    /**
     * Value of the {@link #DAY_OF_WEEK} field indicating
     * Friday.
     */
    Calendar_FRIDAY = 6,
    
    /**
     * Value of the {@link #DAY_OF_WEEK} field indicating
     * Saturday.
     */
    Calendar_SATURDAY = 7,
};
enum
{
    /**
     * Value of the {@link #MONTH} field indicating the
     * first month of the year in the Gregorian and Julian calendars.
     */
    Calendar_JANUARY = 1,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * second month of the year in the Gregorian and Julian calendars.
     */
    Calendar_FEBRUARY = 2,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * third month of the year in the Gregorian and Julian calendars.
     */
    Calendar_MARCH = 3,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * fourth month of the year in the Gregorian and Julian calendars.
     */
    Calendar_APRIL = 4,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * fifth month of the year in the Gregorian and Julian calendars.
     */
    Calendar_MAY = 5,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * sixth month of the year in the Gregorian and Julian calendars.
     */
    Calendar_JUNE = 6,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * seventh month of the year in the Gregorian and Julian calendars.
     */
    Calendar_JULY = 7,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * eighth month of the year in the Gregorian and Julian calendars.
     */
    Calendar_AUGUST = 8,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * ninth month of the year in the Gregorian and Julian calendars.
     */
    Calendar_SEPTEMBER = 9,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * tenth month of the year in the Gregorian and Julian calendars.
     */
    Calendar_OCTOBER = 10,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * eleventh month of the year in the Gregorian and Julian calendars.
     */
    Calendar_NOVEMBER = 11,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * twelfth month of the year in the Gregorian and Julian calendars.
     */
    Calendar_DECEMBER = 12,
    
    /**
     * Value of the {@link #MONTH} field indicating the
     * thirteenth month of the year. Although <code>GregorianCalendar</code>
     * does not use this value, lunar calendars do.
     */
    Calendar_UNDECIMBER = 13,
    
};

@interface Calendar : NSObject//NSCalendar

+ (Calendar *)getInstance;

- (void)setTimeInMillisWithLong:(long64)timeMillis;

-(void) setTimeZoneWithTimeZone:(TimeZone*)tz;

-(Date*) getTime;

-(TimeZone*)getTimeZone;

-(int)getWithInt:(int)type;
-(void) setWithInt:(int)field withInt:(int)value;
-(long64) getTimeInMillis;

-(void) addWithInt:(int)field withInt:(int)amount;

-(void) setTimeWithDate:(Date*)date;

-(void) setWithInt:(int)year withInt:(int)month withInt:(int)date withInt:(int)hourOfDay withInt:(int)minute withInt:(int)second;

@end
