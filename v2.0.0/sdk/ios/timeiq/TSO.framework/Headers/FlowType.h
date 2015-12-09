#ifndef __JavaBridge_FlowType__
#define __JavaBridge_FlowType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class FlowType;

typedef NS_ENUM(NSInteger, eFlowType)
{
 	FlowType_LOCATION , 
 	FlowType_MEETING , 
 	FlowType_PHONE , 
 	FlowType_SEARCH_AS_YOU_TYPE , 
 	FlowType_DETAILS , 
 	FlowType_RESOLVE_LOCATION , 
 	FlowType_CALENDAR , 
 	FlowType_MDMD , 
 	FlowType_UNKNOWN , 

};

@interface FlowType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (FlowType *)valueOfWithNSString:(NSString*)name;
+ (FlowType *)valueOf:(NSString*)name;
+ (FlowType *)parseWithInt:(int)name;
+ (FlowType *)fromIdWithInt:(int)name;
+ (FlowType *)fromId:(int)val;

+ (void)load;
-(eFlowType) ordinal;

 +(FlowType*) LOCATION; 
 +(FlowType*) MEETING; 
 +(FlowType*) PHONE; 
 +(FlowType*) SEARCH_AS_YOU_TYPE; 
 +(FlowType*) DETAILS; 
 +(FlowType*) RESOLVE_LOCATION; 
 +(FlowType*) CALENDAR; 
 +(FlowType*) MDMD; 
 +(FlowType*) UNKNOWN; 


@end

 #define FlowType_get_LOCATION() [FlowType LOCATION] 
 #define FlowType_get_MEETING() [FlowType MEETING] 
 #define FlowType_get_PHONE() [FlowType PHONE] 
 #define FlowType_get_SEARCH_AS_YOU_TYPE() [FlowType SEARCH_AS_YOU_TYPE] 
 #define FlowType_get_DETAILS() [FlowType DETAILS] 
 #define FlowType_get_RESOLVE_LOCATION() [FlowType RESOLVE_LOCATION] 
 #define FlowType_get_CALENDAR() [FlowType CALENDAR] 
 #define FlowType_get_MDMD() [FlowType MDMD] 
 #define FlowType_get_UNKNOWN() [FlowType UNKNOWN] 


#endif
