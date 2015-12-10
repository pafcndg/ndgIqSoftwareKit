#ifndef __JavaBridge_ReminderBuildExceptionType__
#define __JavaBridge_ReminderBuildExceptionType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class ReminderBuildExceptionType;

typedef NS_ENUM(NSInteger, eReminderBuildExceptionType)
{
 	ReminderBuildExceptionType_ReminderMustIncludeATrigger , 
 	ReminderBuildExceptionType_ReminderMustHaveId , 
 	ReminderBuildExceptionType_ReminderMustHaveType , 
 	ReminderBuildExceptionType_DoReminderMustIncludeAValidAction , 
 	ReminderBuildExceptionType_PhoneBasedReminderMustIncludeContactInfo , 
 	ReminderBuildExceptionType_PhoneBasedReminderMustIncludeAtLeastOnePhoneNumber , 
 	ReminderBuildExceptionType_NotificationReminderMustIncludeAValidMessage , 

};

@interface ReminderBuildExceptionType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ReminderBuildExceptionType *)valueOfWithNSString:(NSString*)name;
+ (ReminderBuildExceptionType *)valueOf:(NSString*)name;
+ (ReminderBuildExceptionType *)parseWithInt:(int)name;
+ (ReminderBuildExceptionType *)fromIdWithInt:(int)name;
+ (ReminderBuildExceptionType *)fromId:(int)val;

+ (void)load;
-(eReminderBuildExceptionType) ordinal;

 +(ReminderBuildExceptionType*) ReminderMustIncludeATrigger; 
 +(ReminderBuildExceptionType*) ReminderMustHaveId; 
 +(ReminderBuildExceptionType*) ReminderMustHaveType; 
 +(ReminderBuildExceptionType*) DoReminderMustIncludeAValidAction; 
 +(ReminderBuildExceptionType*) PhoneBasedReminderMustIncludeContactInfo; 
 +(ReminderBuildExceptionType*) PhoneBasedReminderMustIncludeAtLeastOnePhoneNumber; 
 +(ReminderBuildExceptionType*) NotificationReminderMustIncludeAValidMessage; 


@end

 #define ReminderBuildExceptionType_get_ReminderMustIncludeATrigger() [ReminderBuildExceptionType ReminderMustIncludeATrigger] 
 #define ReminderBuildExceptionType_get_ReminderMustHaveId() [ReminderBuildExceptionType ReminderMustHaveId] 
 #define ReminderBuildExceptionType_get_ReminderMustHaveType() [ReminderBuildExceptionType ReminderMustHaveType] 
 #define ReminderBuildExceptionType_get_DoReminderMustIncludeAValidAction() [ReminderBuildExceptionType DoReminderMustIncludeAValidAction] 
 #define ReminderBuildExceptionType_get_PhoneBasedReminderMustIncludeContactInfo() [ReminderBuildExceptionType PhoneBasedReminderMustIncludeContactInfo] 
 #define ReminderBuildExceptionType_get_PhoneBasedReminderMustIncludeAtLeastOnePhoneNumber() [ReminderBuildExceptionType PhoneBasedReminderMustIncludeAtLeastOnePhoneNumber] 
 #define ReminderBuildExceptionType_get_NotificationReminderMustIncludeAValidMessage() [ReminderBuildExceptionType NotificationReminderMustIncludeAValidMessage] 


#endif
