#ifndef __JavaBridge_ReminderStatus__
#define __JavaBridge_ReminderStatus__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class ReminderStatus;

typedef NS_ENUM(NSInteger, eReminderStatus)
{
 	ReminderStatus_NA , 
 	ReminderStatus_Active , 
 	ReminderStatus_Triggered , 

};

@interface ReminderStatus : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ReminderStatus *)valueOfWithNSString:(NSString*)name;
+ (ReminderStatus *)valueOf:(NSString*)name;
+ (ReminderStatus *)parseWithInt:(int)name;
+ (ReminderStatus *)fromIdWithInt:(int)name;
+ (ReminderStatus *)fromId:(int)val;

+ (void)load;
-(eReminderStatus) ordinal;

 +(ReminderStatus*) NA; 
 +(ReminderStatus*) Active; 
 +(ReminderStatus*) Triggered; 


@end

 #define ReminderStatus_get_NA() [ReminderStatus NA] 
 #define ReminderStatus_get_Active() [ReminderStatus Active] 
 #define ReminderStatus_get_Triggered() [ReminderStatus Triggered] 


#endif
