#ifndef __JavaBridge_AttendeeType__
#define __JavaBridge_AttendeeType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class AttendeeType;

typedef NS_ENUM(NSInteger, eAttendeeType)
{
 	AttendeeType_NONE , 
 	AttendeeType_REQUIRED , 
 	AttendeeType_OPTIONAL , 
 	AttendeeType_RESOURCE , 

};

@interface AttendeeType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (AttendeeType *)valueOfWithNSString:(NSString*)name;
+ (AttendeeType *)valueOf:(NSString*)name;
+ (AttendeeType *)parseWithInt:(int)name;
+ (AttendeeType *)fromIdWithInt:(int)name;
+ (AttendeeType *)fromId:(int)val;

+ (void)load;
-(eAttendeeType) ordinal;

 +(AttendeeType*) NONE; 
 +(AttendeeType*) REQUIRED; 
 +(AttendeeType*) OPTIONAL; 
 +(AttendeeType*) RESOURCE; 


@end

 #define AttendeeType_get_NONE() [AttendeeType NONE] 
 #define AttendeeType_get_REQUIRED() [AttendeeType REQUIRED] 
 #define AttendeeType_get_OPTIONAL() [AttendeeType OPTIONAL] 
 #define AttendeeType_get_RESOURCE() [AttendeeType RESOURCE] 


#endif
