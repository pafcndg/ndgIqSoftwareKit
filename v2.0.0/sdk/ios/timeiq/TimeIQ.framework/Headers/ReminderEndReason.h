#ifndef __JavaBridge_ReminderEndReason__
#define __JavaBridge_ReminderEndReason__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class ReminderEndReason;

typedef NS_ENUM(NSInteger, eReminderEndReason)
{
 	ReminderEndReason_Dismiss , 
 	ReminderEndReason_Done , 

};

@interface ReminderEndReason : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ReminderEndReason *)valueOfWithNSString:(NSString*)name;
+ (ReminderEndReason *)valueOf:(NSString*)name;
+ (ReminderEndReason *)parseWithInt:(int)name;
+ (ReminderEndReason *)fromIdWithInt:(int)name;
+ (ReminderEndReason *)fromId:(int)val;

+ (void)load;
-(eReminderEndReason) ordinal;

 +(ReminderEndReason*) Dismiss; 
 +(ReminderEndReason*) Done; 


@end

 #define ReminderEndReason_get_Dismiss() [ReminderEndReason Dismiss] 
 #define ReminderEndReason_get_Done() [ReminderEndReason Done] 


#endif
