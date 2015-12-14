#ifndef __JavaBridge_SensorType__
#define __JavaBridge_SensorType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class SensorType;

typedef NS_ENUM(NSInteger, eSensorType)
{
 	SensorType_GPS , 
 	SensorType_FUSED , 
 	SensorType_WIFI , 
 	SensorType_ACTIVITY , 
 	SensorType_BLUETOOTH , 
 	SensorType_BATTERY , 
 	SensorType_USER_INPUT , 
 	SensorType_PEDOMETER , 
 	SensorType_DEVICE_STATE , 
 	SensorType_NETWORK , 
 	SensorType_NA , 

};

@interface SensorType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (SensorType *)valueOfWithNSString:(NSString*)name;
+ (SensorType *)valueOf:(NSString*)name;
+ (SensorType *)parseWithInt:(int)name;
+ (SensorType *)fromIdWithInt:(int)name;
+ (SensorType *)fromId:(int)val;

+ (void)load;
-(eSensorType) ordinal;

 +(SensorType*) GPS; 
 +(SensorType*) FUSED; 
 +(SensorType*) WIFI; 
 +(SensorType*) ACTIVITY; 
 +(SensorType*) BLUETOOTH; 
 +(SensorType*) BATTERY; 
 +(SensorType*) USER_INPUT; 
 +(SensorType*) PEDOMETER; 
 +(SensorType*) DEVICE_STATE; 
 +(SensorType*) NETWORK; 
 +(SensorType*) NA; 


@end

 #define SensorType_get_GPS() [SensorType GPS] 
 #define SensorType_get_FUSED() [SensorType FUSED] 
 #define SensorType_get_WIFI() [SensorType WIFI] 
 #define SensorType_get_ACTIVITY() [SensorType ACTIVITY] 
 #define SensorType_get_BLUETOOTH() [SensorType BLUETOOTH] 
 #define SensorType_get_BATTERY() [SensorType BATTERY] 
 #define SensorType_get_USER_INPUT() [SensorType USER_INPUT] 
 #define SensorType_get_PEDOMETER() [SensorType PEDOMETER] 
 #define SensorType_get_DEVICE_STATE() [SensorType DEVICE_STATE] 
 #define SensorType_get_NETWORK() [SensorType NETWORK] 
 #define SensorType_get_NA() [SensorType NA] 


#endif
