#ifndef __JavaBridge_DeviceConnectionState__
#define __JavaBridge_DeviceConnectionState__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class DeviceConnectionState;

typedef NS_ENUM(NSInteger, eDeviceConnectionState)
{
 	DeviceConnectionState_DISCONNECTED , 
 	DeviceConnectionState_CONNECTED , 
 	DeviceConnectionState_UNKNOWN , 

};

@interface DeviceConnectionState : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (DeviceConnectionState *)valueOfWithNSString:(NSString*)name;
+ (DeviceConnectionState *)valueOf:(NSString*)name;
+ (DeviceConnectionState *)parseWithInt:(int)name;
+ (DeviceConnectionState *)fromIdWithInt:(int)name;
+ (DeviceConnectionState *)fromId:(int)val;

+ (void)load;
-(eDeviceConnectionState) ordinal;

 +(DeviceConnectionState*) DISCONNECTED; 
 +(DeviceConnectionState*) CONNECTED; 
 +(DeviceConnectionState*) UNKNOWN; 


@end

 #define DeviceConnectionState_get_DISCONNECTED() [DeviceConnectionState DISCONNECTED] 
 #define DeviceConnectionState_get_CONNECTED() [DeviceConnectionState CONNECTED] 
 #define DeviceConnectionState_get_UNKNOWN() [DeviceConnectionState UNKNOWN] 


#endif
