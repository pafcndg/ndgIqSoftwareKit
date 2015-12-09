#ifndef __JavaBridge_MeetingReminderType__
#define __JavaBridge_MeetingReminderType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class MeetingReminderType;

typedef NS_ENUM(NSInteger, eMeetingReminderType)
{
 	MeetingReminderType_NOTIFICATION , 
 	MeetingReminderType_EMAIL , 
 	MeetingReminderType_SMS , 

};

@interface MeetingReminderType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (MeetingReminderType *)valueOfWithNSString:(NSString*)name;
+ (MeetingReminderType *)valueOf:(NSString*)name;
+ (MeetingReminderType *)parseWithInt:(int)name;
+ (MeetingReminderType *)fromIdWithInt:(int)name;
+ (MeetingReminderType *)fromId:(int)val;

+ (void)load;
-(eMeetingReminderType) ordinal;

 +(MeetingReminderType*) NOTIFICATION; 
 +(MeetingReminderType*) EMAIL; 
 +(MeetingReminderType*) SMS; 


@end

 #define MeetingReminderType_get_NOTIFICATION() [MeetingReminderType NOTIFICATION] 
 #define MeetingReminderType_get_EMAIL() [MeetingReminderType EMAIL] 
 #define MeetingReminderType_get_SMS() [MeetingReminderType SMS] 


#endif
