#ifndef __JavaBridge_UserActionType__
#define __JavaBridge_UserActionType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class UserActionType;

typedef NS_ENUM(NSInteger, eUserActionType)
{
 	UserActionType_OK_GOT_IT , 
 	UserActionType_DISMISS , 
 	UserActionType_GO_NOW , 
 	UserActionType_TIME_SNOOZE , 

};

@interface UserActionType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (UserActionType *)valueOfWithNSString:(NSString*)name;
+ (UserActionType *)valueOf:(NSString*)name;
+ (UserActionType *)parseWithInt:(int)name;
+ (UserActionType *)fromIdWithInt:(int)name;
+ (UserActionType *)fromId:(int)val;

+ (void)load;
-(eUserActionType) ordinal;

 +(UserActionType*) OK_GOT_IT; 
 +(UserActionType*) DISMISS; 
 +(UserActionType*) GO_NOW; 
 +(UserActionType*) TIME_SNOOZE; 


@end

 #define UserActionType_get_OK_GOT_IT() [UserActionType OK_GOT_IT] 
 #define UserActionType_get_DISMISS() [UserActionType DISMISS] 
 #define UserActionType_get_GO_NOW() [UserActionType GO_NOW] 
 #define UserActionType_get_TIME_SNOOZE() [UserActionType TIME_SNOOZE] 


#endif
