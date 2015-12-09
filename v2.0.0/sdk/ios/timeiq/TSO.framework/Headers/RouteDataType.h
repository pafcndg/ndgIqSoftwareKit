#ifndef __JavaBridge_RouteDataType__
#define __JavaBridge_RouteDataType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class RouteDataType;

typedef NS_ENUM(NSInteger, eRouteDataType)
{
 	RouteDataType_TOO_FAR_FOR_DRIVING , 
 	RouteDataType_TOO_FAR_FOR_WALKING , 
 	RouteDataType_AT_DESTINATION_WHILE_DRIVING , 
 	RouteDataType_AT_DESTINATION , 
 	RouteDataType_DESTINATION_CLOSE_BY_WHILE_DRIVING , 
 	RouteDataType_DESTINATION_CLOSE_BY , 
 	RouteDataType_NEAR_DESTINATION , 
 	RouteDataType_WALK , 
 	RouteDataType_DRIVE , 
 	RouteDataType_DRIVE_WHILE_DRIVING , 

};

@interface RouteDataType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (RouteDataType *)valueOfWithNSString:(NSString*)name;
+ (RouteDataType *)valueOf:(NSString*)name;
+ (RouteDataType *)parseWithInt:(int)name;
+ (RouteDataType *)fromIdWithInt:(int)name;
+ (RouteDataType *)fromId:(int)val;

+ (void)load;
-(eRouteDataType) ordinal;

 +(RouteDataType*) TOO_FAR_FOR_DRIVING; 
 +(RouteDataType*) TOO_FAR_FOR_WALKING; 
 +(RouteDataType*) AT_DESTINATION_WHILE_DRIVING; 
 +(RouteDataType*) AT_DESTINATION; 
 +(RouteDataType*) DESTINATION_CLOSE_BY_WHILE_DRIVING; 
 +(RouteDataType*) DESTINATION_CLOSE_BY; 
 +(RouteDataType*) NEAR_DESTINATION; 
 +(RouteDataType*) WALK; 
 +(RouteDataType*) DRIVE; 
 +(RouteDataType*) DRIVE_WHILE_DRIVING; 


@end

 #define RouteDataType_get_TOO_FAR_FOR_DRIVING() [RouteDataType TOO_FAR_FOR_DRIVING] 
 #define RouteDataType_get_TOO_FAR_FOR_WALKING() [RouteDataType TOO_FAR_FOR_WALKING] 
 #define RouteDataType_get_AT_DESTINATION_WHILE_DRIVING() [RouteDataType AT_DESTINATION_WHILE_DRIVING] 
 #define RouteDataType_get_AT_DESTINATION() [RouteDataType AT_DESTINATION] 
 #define RouteDataType_get_DESTINATION_CLOSE_BY_WHILE_DRIVING() [RouteDataType DESTINATION_CLOSE_BY_WHILE_DRIVING] 
 #define RouteDataType_get_DESTINATION_CLOSE_BY() [RouteDataType DESTINATION_CLOSE_BY] 
 #define RouteDataType_get_NEAR_DESTINATION() [RouteDataType NEAR_DESTINATION] 
 #define RouteDataType_get_WALK() [RouteDataType WALK] 
 #define RouteDataType_get_DRIVE() [RouteDataType DRIVE] 
 #define RouteDataType_get_DRIVE_WHILE_DRIVING() [RouteDataType DRIVE_WHILE_DRIVING] 


#endif
