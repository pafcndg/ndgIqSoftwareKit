#ifndef __JavaBridge_ReminderType__
#define __JavaBridge_ReminderType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ReminderType;

typedef NS_ENUM(NSInteger, eReminderType)
{
 	ReminderType_CALL , 
 	ReminderType_NOTIFY , 
 	ReminderType_DO , 

};

@interface ReminderType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ReminderType *)valueOfWithNSString:(NSString*)name;
+ (ReminderType *)valueOf:(NSString*)name;
+ (ReminderType *)parseWithInt:(int)name;
+ (ReminderType *)fromIdWithInt:(int)name;
+ (ReminderType *)fromId:(int)val;

+ (void)load;
-(eReminderType) ordinal;

 +(ReminderType*) CALL; 
 +(ReminderType*) NOTIFY; 
 +(ReminderType*) DO; 


@end

 #define ReminderType_get_CALL() [ReminderType CALL] 
 #define ReminderType_get_NOTIFY() [ReminderType NOTIFY] 
 #define ReminderType_get_DO() [ReminderType DO] 


#endif
