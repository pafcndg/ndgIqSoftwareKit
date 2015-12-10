#ifndef __JavaBridge_SensorStateType__
#define __JavaBridge_SensorStateType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class SensorStateType;

typedef NS_ENUM(NSInteger, eSensorStateType)
{
 	SensorStateType_NotInitiated , 
 	SensorStateType_Started , 
 	SensorStateType_Stopped , 
 	SensorStateType_Paused , 
 	SensorStateType_Resumed , 
 	SensorStateType_ERROR , 
 	SensorStateType_Suspended , 
 	SensorStateType_UNKNOWN , 

};

@interface SensorStateType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (SensorStateType *)valueOfWithNSString:(NSString*)name;
+ (SensorStateType *)valueOf:(NSString*)name;
+ (SensorStateType *)parseWithInt:(int)name;
+ (SensorStateType *)fromIdWithInt:(int)name;
+ (SensorStateType *)fromId:(int)val;

+ (void)load;
-(eSensorStateType) ordinal;

 +(SensorStateType*) NotInitiated; 
 +(SensorStateType*) Started; 
 +(SensorStateType*) Stopped; 
 +(SensorStateType*) Paused; 
 +(SensorStateType*) Resumed; 
 +(SensorStateType*) ERROR; 
 +(SensorStateType*) Suspended; 
 +(SensorStateType*) UNKNOWN; 


@end

 #define SensorStateType_get_NotInitiated() [SensorStateType NotInitiated] 
 #define SensorStateType_get_Started() [SensorStateType Started] 
 #define SensorStateType_get_Stopped() [SensorStateType Stopped] 
 #define SensorStateType_get_Paused() [SensorStateType Paused] 
 #define SensorStateType_get_Resumed() [SensorStateType Resumed] 
 #define SensorStateType_get_ERROR() [SensorStateType ERROR] 
 #define SensorStateType_get_Suspended() [SensorStateType Suspended] 
 #define SensorStateType_get_UNKNOWN() [SensorStateType UNKNOWN] 


#endif
