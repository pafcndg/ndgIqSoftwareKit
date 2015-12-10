#ifndef __JavaBridge_DeviceStateType__
#define __JavaBridge_DeviceStateType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class DeviceStateType;

typedef NS_ENUM(NSInteger, eDeviceStateType)
{
 	DeviceStateType_NETWORK_SEND_DATA , 
 	DeviceStateType_NETWORK_ROAMING , 
 	DeviceStateType_NETWORK_WIFI_AVAILABLE , 
 	DeviceStateType_NETWORK_AVAILABLE , 
 	DeviceStateType_BATTERY_LEVEL , 
 	DeviceStateType_BATTERY_CHARGE , 
 	DeviceStateType_LOCATION_SERVICES_GPS_AVAILABLE , 
 	DeviceStateType_PHONE_AIRPLANE_MODE , 
 	DeviceStateType_TIMEZONE , 

};

@interface DeviceStateType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (DeviceStateType *)valueOfWithNSString:(NSString*)name;
+ (DeviceStateType *)valueOf:(NSString*)name;
+ (DeviceStateType *)parseWithInt:(int)name;
+ (DeviceStateType *)fromIdWithInt:(int)name;
+ (DeviceStateType *)fromId:(int)val;

+ (void)load;
-(eDeviceStateType) ordinal;

 +(DeviceStateType*) NETWORK_SEND_DATA; 
 +(DeviceStateType*) NETWORK_ROAMING; 
 +(DeviceStateType*) NETWORK_WIFI_AVAILABLE; 
 +(DeviceStateType*) NETWORK_AVAILABLE; 
 +(DeviceStateType*) BATTERY_LEVEL; 
 +(DeviceStateType*) BATTERY_CHARGE; 
 +(DeviceStateType*) LOCATION_SERVICES_GPS_AVAILABLE; 
 +(DeviceStateType*) PHONE_AIRPLANE_MODE; 
 +(DeviceStateType*) TIMEZONE; 


@end

 #define DeviceStateType_get_NETWORK_SEND_DATA() [DeviceStateType NETWORK_SEND_DATA] 
 #define DeviceStateType_get_NETWORK_ROAMING() [DeviceStateType NETWORK_ROAMING] 
 #define DeviceStateType_get_NETWORK_WIFI_AVAILABLE() [DeviceStateType NETWORK_WIFI_AVAILABLE] 
 #define DeviceStateType_get_NETWORK_AVAILABLE() [DeviceStateType NETWORK_AVAILABLE] 
 #define DeviceStateType_get_BATTERY_LEVEL() [DeviceStateType BATTERY_LEVEL] 
 #define DeviceStateType_get_BATTERY_CHARGE() [DeviceStateType BATTERY_CHARGE] 
 #define DeviceStateType_get_LOCATION_SERVICES_GPS_AVAILABLE() [DeviceStateType LOCATION_SERVICES_GPS_AVAILABLE] 
 #define DeviceStateType_get_PHONE_AIRPLANE_MODE() [DeviceStateType PHONE_AIRPLANE_MODE] 
 #define DeviceStateType_get_TIMEZONE() [DeviceStateType TIMEZONE] 


#endif
