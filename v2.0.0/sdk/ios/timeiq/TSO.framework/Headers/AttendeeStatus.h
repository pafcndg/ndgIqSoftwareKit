#ifndef __JavaBridge_AttendeeStatus__
#define __JavaBridge_AttendeeStatus__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class AttendeeStatus;

typedef NS_ENUM(NSInteger, eAttendeeStatus)
{
 	AttendeeStatus_NONE , 
 	AttendeeStatus_ACCEPTED , 
 	AttendeeStatus_DECLINED , 
 	AttendeeStatus_INVITED , 
 	AttendeeStatus_TENTATIVE , 

};

@interface AttendeeStatus : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (AttendeeStatus *)valueOfWithNSString:(NSString*)name;
+ (AttendeeStatus *)valueOf:(NSString*)name;
+ (AttendeeStatus *)parseWithInt:(int)name;
+ (AttendeeStatus *)fromIdWithInt:(int)name;
+ (AttendeeStatus *)fromId:(int)val;

+ (void)load;
-(eAttendeeStatus) ordinal;

 +(AttendeeStatus*) NONE; 
 +(AttendeeStatus*) ACCEPTED; 
 +(AttendeeStatus*) DECLINED; 
 +(AttendeeStatus*) INVITED; 
 +(AttendeeStatus*) TENTATIVE; 


@end

 #define AttendeeStatus_get_NONE() [AttendeeStatus NONE] 
 #define AttendeeStatus_get_ACCEPTED() [AttendeeStatus ACCEPTED] 
 #define AttendeeStatus_get_DECLINED() [AttendeeStatus DECLINED] 
 #define AttendeeStatus_get_INVITED() [AttendeeStatus INVITED] 
 #define AttendeeStatus_get_TENTATIVE() [AttendeeStatus TENTATIVE] 


#endif
