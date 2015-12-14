#ifndef __JavaBridge_ReminderStatusInner__
#define __JavaBridge_ReminderStatusInner__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class ReminderStatusInner;

typedef NS_ENUM(NSInteger, eReminderStatusInner)
{
 	ReminderStatusInner_New , 
 	ReminderStatusInner_Registered , 
 	ReminderStatusInner_Triggered , 
 	ReminderStatusInner_Ended , 

};

@interface ReminderStatusInner : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ReminderStatusInner *)valueOfWithNSString:(NSString*)name;
+ (ReminderStatusInner *)valueOf:(NSString*)name;
+ (ReminderStatusInner *)parseWithInt:(int)name;
+ (ReminderStatusInner *)fromIdWithInt:(int)name;
+ (ReminderStatusInner *)fromId:(int)val;

+ (void)load;
-(eReminderStatusInner) ordinal;

 +(ReminderStatusInner*) New; 
 +(ReminderStatusInner*) Registered; 
 +(ReminderStatusInner*) Triggered; 
 +(ReminderStatusInner*) Ended; 


@end

 #define ReminderStatusInner_get_New() [ReminderStatusInner New] 
 #define ReminderStatusInner_get_Registered() [ReminderStatusInner Registered] 
 #define ReminderStatusInner_get_Triggered() [ReminderStatusInner Triggered] 
 #define ReminderStatusInner_get_Ended() [ReminderStatusInner Ended] 


#endif
