#ifndef __JavaBridge_SnoozeType__
#define __JavaBridge_SnoozeType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class SnoozeType;

typedef NS_ENUM(NSInteger, eSnoozeType)
{
 	SnoozeType_WHEN_CHARGING , 
 	SnoozeType_FROM_CAR , 
 	SnoozeType_NEXT_DRIVE , 
 	SnoozeType_DEFINE_HOME , 
 	SnoozeType_DEFINE_WORK , 
 	SnoozeType_FROM_PLACE , 
 	SnoozeType_NEXT_TIME_AT_CURRENT_PLACE , 
 	SnoozeType_LEAVE_CURRENT_PLACE , 
 	SnoozeType_IN_X_MIN , 
 	SnoozeType_TIME_RANGE , 

};

@interface SnoozeType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (SnoozeType *)valueOfWithNSString:(NSString*)name;
+ (SnoozeType *)valueOf:(NSString*)name;
+ (SnoozeType *)parseWithInt:(int)name;
+ (SnoozeType *)fromIdWithInt:(int)name;
+ (SnoozeType *)fromId:(int)val;

+ (void)load;
-(eSnoozeType) ordinal;

 +(SnoozeType*) WHEN_CHARGING; 
 +(SnoozeType*) FROM_CAR; 
 +(SnoozeType*) NEXT_DRIVE; 
 +(SnoozeType*) DEFINE_HOME; 
 +(SnoozeType*) DEFINE_WORK; 
 +(SnoozeType*) FROM_PLACE; 
 +(SnoozeType*) NEXT_TIME_AT_CURRENT_PLACE; 
 +(SnoozeType*) LEAVE_CURRENT_PLACE; 
 +(SnoozeType*) IN_X_MIN; 
 +(SnoozeType*) TIME_RANGE; 


@end

 #define SnoozeType_get_WHEN_CHARGING() [SnoozeType WHEN_CHARGING] 
 #define SnoozeType_get_FROM_CAR() [SnoozeType FROM_CAR] 
 #define SnoozeType_get_NEXT_DRIVE() [SnoozeType NEXT_DRIVE] 
 #define SnoozeType_get_DEFINE_HOME() [SnoozeType DEFINE_HOME] 
 #define SnoozeType_get_DEFINE_WORK() [SnoozeType DEFINE_WORK] 
 #define SnoozeType_get_FROM_PLACE() [SnoozeType FROM_PLACE] 
 #define SnoozeType_get_NEXT_TIME_AT_CURRENT_PLACE() [SnoozeType NEXT_TIME_AT_CURRENT_PLACE] 
 #define SnoozeType_get_LEAVE_CURRENT_PLACE() [SnoozeType LEAVE_CURRENT_PLACE] 
 #define SnoozeType_get_IN_X_MIN() [SnoozeType IN_X_MIN] 
 #define SnoozeType_get_TIME_RANGE() [SnoozeType TIME_RANGE] 


#endif
