#ifndef __JavaBridge_WifiState__
#define __JavaBridge_WifiState__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class WifiState;

typedef NS_ENUM(NSInteger, eWifiState)
{
 	WifiState_CONNECTED , 
 	WifiState_SCANNING , 
 	WifiState_HANDSHAKE , 
 	WifiState_DISCONNECTED , 

};

@interface WifiState : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (WifiState *)valueOfWithNSString:(NSString*)name;
+ (WifiState *)valueOf:(NSString*)name;
+ (WifiState *)parseWithInt:(int)name;
+ (WifiState *)fromIdWithInt:(int)name;
+ (WifiState *)fromId:(int)val;

+ (void)load;
-(eWifiState) ordinal;

 +(WifiState*) CONNECTED; 
 +(WifiState*) SCANNING; 
 +(WifiState*) HANDSHAKE; 
 +(WifiState*) DISCONNECTED; 


@end

 #define WifiState_get_CONNECTED() [WifiState CONNECTED] 
 #define WifiState_get_SCANNING() [WifiState SCANNING] 
 #define WifiState_get_HANDSHAKE() [WifiState HANDSHAKE] 
 #define WifiState_get_DISCONNECTED() [WifiState DISCONNECTED] 


#endif
