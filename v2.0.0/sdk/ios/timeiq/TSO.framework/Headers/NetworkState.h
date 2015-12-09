#ifndef __JavaBridge_NetworkState__
#define __JavaBridge_NetworkState__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class NetworkState;

typedef NS_ENUM(NSInteger, eNetworkState)
{
 	NetworkState_CONNECTED_WIFI , 
 	NetworkState_CONNECTED_NON_WIFI , 
 	NetworkState_CONNECTED_ROAMING_WIFI , 
 	NetworkState_CONNECTED_ROAMING_NON_WIFI , 
 	NetworkState_DISCONNECTED , 

};

@interface NetworkState : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (NetworkState *)valueOfWithNSString:(NSString*)name;
+ (NetworkState *)valueOf:(NSString*)name;
+ (NetworkState *)parseWithInt:(int)name;
+ (NetworkState *)fromIdWithInt:(int)name;
+ (NetworkState *)fromId:(int)val;

+ (void)load;
-(eNetworkState) ordinal;

 +(NetworkState*) CONNECTED_WIFI; 
 +(NetworkState*) CONNECTED_NON_WIFI; 
 +(NetworkState*) CONNECTED_ROAMING_WIFI; 
 +(NetworkState*) CONNECTED_ROAMING_NON_WIFI; 
 +(NetworkState*) DISCONNECTED; 


@end

 #define NetworkState_get_CONNECTED_WIFI() [NetworkState CONNECTED_WIFI] 
 #define NetworkState_get_CONNECTED_NON_WIFI() [NetworkState CONNECTED_NON_WIFI] 
 #define NetworkState_get_CONNECTED_ROAMING_WIFI() [NetworkState CONNECTED_ROAMING_WIFI] 
 #define NetworkState_get_CONNECTED_ROAMING_NON_WIFI() [NetworkState CONNECTED_ROAMING_NON_WIFI] 
 #define NetworkState_get_DISCONNECTED() [NetworkState DISCONNECTED] 


#endif
