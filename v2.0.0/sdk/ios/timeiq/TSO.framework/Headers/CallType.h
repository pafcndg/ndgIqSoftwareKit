#ifndef __JavaBridge_CallType__
#define __JavaBridge_CallType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class CallType;

typedef NS_ENUM(NSInteger, eCallType)
{
 	CallType_UserInitiated , 
 	CallType_MissedCall , 
 	CallType_FailedAttempt , 
 	CallType_Sms , 

};

@interface CallType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (CallType *)valueOfWithNSString:(NSString*)name;
+ (CallType *)valueOf:(NSString*)name;
+ (CallType *)parseWithInt:(int)name;
+ (CallType *)fromIdWithInt:(int)name;
+ (CallType *)fromId:(int)val;

+ (void)load;
-(eCallType) ordinal;

 +(CallType*) UserInitiated; 
 +(CallType*) MissedCall; 
 +(CallType*) FailedAttempt; 
 +(CallType*) Sms; 


@end

 #define CallType_get_UserInitiated() [CallType UserInitiated] 
 #define CallType_get_MissedCall() [CallType MissedCall] 
 #define CallType_get_FailedAttempt() [CallType FailedAttempt] 
 #define CallType_get_Sms() [CallType Sms] 


#endif
