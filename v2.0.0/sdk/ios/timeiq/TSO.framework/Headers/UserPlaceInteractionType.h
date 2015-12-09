#ifndef __JavaBridge_UserPlaceInteractionType__
#define __JavaBridge_UserPlaceInteractionType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class UserPlaceInteractionType;

typedef NS_ENUM(NSInteger, eUserPlaceInteractionType)
{
 	UserPlaceInteractionType_Accepted , 
 	UserPlaceInteractionType_Rejected , 

};

@interface UserPlaceInteractionType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (UserPlaceInteractionType *)valueOfWithNSString:(NSString*)name;
+ (UserPlaceInteractionType *)valueOf:(NSString*)name;
+ (UserPlaceInteractionType *)parseWithInt:(int)name;
+ (UserPlaceInteractionType *)fromIdWithInt:(int)name;
+ (UserPlaceInteractionType *)fromId:(int)val;

+ (void)load;
-(eUserPlaceInteractionType) ordinal;

 +(UserPlaceInteractionType*) Accepted; 
 +(UserPlaceInteractionType*) Rejected; 


@end

 #define UserPlaceInteractionType_get_Accepted() [UserPlaceInteractionType Accepted] 
 #define UserPlaceInteractionType_get_Rejected() [UserPlaceInteractionType Rejected] 


#endif
