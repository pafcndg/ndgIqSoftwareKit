#ifndef __JavaBridge_MessageType__
#define __JavaBridge_MessageType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class MessageType;

typedef NS_ENUM(NSInteger, eMessageType)
{
 	MessageType_ON_REMINDERS_TRIGGERED , 
 	MessageType_ON_EVENT_TRIGGERED , 
 	MessageType_ON_EVENT_START , 
 	MessageType_ON_EVENT_END , 

};

@interface MessageType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (MessageType *)valueOfWithNSString:(NSString*)name;
+ (MessageType *)valueOf:(NSString*)name;
+ (MessageType *)parseWithInt:(int)name;
+ (MessageType *)fromIdWithInt:(int)name;
+ (MessageType *)fromId:(int)val;

+ (void)load;
-(eMessageType) ordinal;

 +(MessageType*) ON_REMINDERS_TRIGGERED; 
 +(MessageType*) ON_EVENT_TRIGGERED; 
 +(MessageType*) ON_EVENT_START; 
 +(MessageType*) ON_EVENT_END; 


@end

 #define MessageType_get_ON_REMINDERS_TRIGGERED() [MessageType ON_REMINDERS_TRIGGERED] 
 #define MessageType_get_ON_EVENT_TRIGGERED() [MessageType ON_EVENT_TRIGGERED] 
 #define MessageType_get_ON_EVENT_START() [MessageType ON_EVENT_START] 
 #define MessageType_get_ON_EVENT_END() [MessageType ON_EVENT_END] 


#endif
