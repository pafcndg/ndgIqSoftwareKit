#ifndef __JavaBridge_GpsState__
#define __JavaBridge_GpsState__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class GpsState;

typedef NS_ENUM(NSInteger, eGpsState)
{
 	GpsState_GPS_STARTED , 
 	GpsState_GPS_STOPPED , 
 	GpsState_GPS_FIX , 
 	GpsState_GPS_SEARCHING , 

};

@interface GpsState : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (GpsState *)valueOfWithNSString:(NSString*)name;
+ (GpsState *)valueOf:(NSString*)name;
+ (GpsState *)parseWithInt:(int)name;
+ (GpsState *)fromIdWithInt:(int)name;
+ (GpsState *)fromId:(int)val;

+ (void)load;
-(eGpsState) ordinal;

 +(GpsState*) GPS_STARTED; 
 +(GpsState*) GPS_STOPPED; 
 +(GpsState*) GPS_FIX; 
 +(GpsState*) GPS_SEARCHING; 


@end

 #define GpsState_get_GPS_STARTED() [GpsState GPS_STARTED] 
 #define GpsState_get_GPS_STOPPED() [GpsState GPS_STOPPED] 
 #define GpsState_get_GPS_FIX() [GpsState GPS_FIX] 
 #define GpsState_get_GPS_SEARCHING() [GpsState GPS_SEARCHING] 


#endif
