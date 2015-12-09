#ifndef __JavaBridge_eExtendedEventReminderType__
#define __JavaBridge_eExtendedEventReminderType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class eExtendedEventReminderType;

typedef NS_ENUM(NSInteger, eeExtendedEventReminderType)
{
 	eExtendedEventReminderType_DO_AT_TIME_OF_DAY , 
 	eExtendedEventReminderType_DO_AT_TIME_BEFORE_EVENT , 
 	eExtendedEventReminderType_NOTIFY_AT_TIME_OF_DAY , 

};

@interface eExtendedEventReminderType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (eExtendedEventReminderType *)valueOfWithNSString:(NSString*)name;
+ (eExtendedEventReminderType *)valueOf:(NSString*)name;
+ (eExtendedEventReminderType *)parseWithInt:(int)name;
+ (eExtendedEventReminderType *)fromIdWithInt:(int)name;
+ (eExtendedEventReminderType *)fromId:(int)val;

+ (void)load;
-(eeExtendedEventReminderType) ordinal;

 +(eExtendedEventReminderType*) DO_AT_TIME_OF_DAY; 
 +(eExtendedEventReminderType*) DO_AT_TIME_BEFORE_EVENT; 
 +(eExtendedEventReminderType*) NOTIFY_AT_TIME_OF_DAY; 


@end

 #define eExtendedEventReminderType_get_DO_AT_TIME_OF_DAY() [eExtendedEventReminderType DO_AT_TIME_OF_DAY] 
 #define eExtendedEventReminderType_get_DO_AT_TIME_BEFORE_EVENT() [eExtendedEventReminderType DO_AT_TIME_BEFORE_EVENT] 
 #define eExtendedEventReminderType_get_NOTIFY_AT_TIME_OF_DAY() [eExtendedEventReminderType NOTIFY_AT_TIME_OF_DAY] 


#endif
