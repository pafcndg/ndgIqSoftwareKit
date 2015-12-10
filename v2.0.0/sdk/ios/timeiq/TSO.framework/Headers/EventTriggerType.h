#ifndef __JavaBridge_EventTriggerType__
#define __JavaBridge_EventTriggerType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class EventTriggerType;

typedef NS_ENUM(NSInteger, eEventTriggerType)
{
 	EventTriggerType_START_EVENT , 
 	EventTriggerType_END_EVENT , 
 	EventTriggerType_GENERAL_REMINDER , 
 	EventTriggerType_TTL_FOR_EVENT , 

};

@interface EventTriggerType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (EventTriggerType *)valueOfWithNSString:(NSString*)name;
+ (EventTriggerType *)valueOf:(NSString*)name;
+ (EventTriggerType *)parseWithInt:(int)name;
+ (EventTriggerType *)fromIdWithInt:(int)name;
+ (EventTriggerType *)fromId:(int)val;

+ (void)load;
-(eEventTriggerType) ordinal;

 +(EventTriggerType*) START_EVENT; 
 +(EventTriggerType*) END_EVENT; 
 +(EventTriggerType*) GENERAL_REMINDER; 
 +(EventTriggerType*) TTL_FOR_EVENT; 


@end

 #define EventTriggerType_get_START_EVENT() [EventTriggerType START_EVENT] 
 #define EventTriggerType_get_END_EVENT() [EventTriggerType END_EVENT] 
 #define EventTriggerType_get_GENERAL_REMINDER() [EventTriggerType GENERAL_REMINDER] 
 #define EventTriggerType_get_TTL_FOR_EVENT() [EventTriggerType TTL_FOR_EVENT] 


#endif
