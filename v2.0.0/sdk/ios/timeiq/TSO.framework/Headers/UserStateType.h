#ifndef __JavaBridge_UserStateType__
#define __JavaBridge_UserStateType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class UserStateType;

typedef NS_ENUM(NSInteger, eUserStateType)
{
 	UserStateType_VISIT , 
 	UserStateType_MOT , 
 	UserStateType_CAR_LOCATION , 

};

@interface UserStateType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (UserStateType *)valueOfWithNSString:(NSString*)name;
+ (UserStateType *)valueOf:(NSString*)name;
+ (UserStateType *)parseWithInt:(int)name;
+ (UserStateType *)fromIdWithInt:(int)name;
+ (UserStateType *)fromId:(int)val;

+ (void)load;
-(eUserStateType) ordinal;

 +(UserStateType*) VISIT; 
 +(UserStateType*) MOT; 
 +(UserStateType*) CAR_LOCATION; 


@end

 #define UserStateType_get_VISIT() [UserStateType VISIT] 
 #define UserStateType_get_MOT() [UserStateType MOT] 
 #define UserStateType_get_CAR_LOCATION() [UserStateType CAR_LOCATION] 


#endif
