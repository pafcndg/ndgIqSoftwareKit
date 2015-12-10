#ifndef __JavaBridge_eTriggerBuildExceptionType__
#define __JavaBridge_eTriggerBuildExceptionType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class eTriggerBuildExceptionType;

typedef NS_ENUM(NSInteger, eeTriggerBuildExceptionType)
{
 	eTriggerBuildExceptionType_PlaceTriggerMustHaveType , 
 	eTriggerBuildExceptionType_PlaceTriggerMustHavePlace , 
 	eTriggerBuildExceptionType_MotTriggerMustHaveType , 
 	eTriggerBuildExceptionType_MotTriggerMustHaveTransitionType , 
 	eTriggerBuildExceptionType_ChargeTriggerMustHaveType , 
 	eTriggerBuildExceptionType_ChargeTriggerIllegalPercent , 
 	eTriggerBuildExceptionType_TriggerMustHaveId , 
 	eTriggerBuildExceptionType_TriggerMustHaveType , 

};

@interface eTriggerBuildExceptionType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (eTriggerBuildExceptionType *)valueOfWithNSString:(NSString*)name;
+ (eTriggerBuildExceptionType *)valueOf:(NSString*)name;
+ (eTriggerBuildExceptionType *)parseWithInt:(int)name;
+ (eTriggerBuildExceptionType *)fromIdWithInt:(int)name;
+ (eTriggerBuildExceptionType *)fromId:(int)val;

+ (void)load;
-(eeTriggerBuildExceptionType) ordinal;

 +(eTriggerBuildExceptionType*) PlaceTriggerMustHaveType; 
 +(eTriggerBuildExceptionType*) PlaceTriggerMustHavePlace; 
 +(eTriggerBuildExceptionType*) MotTriggerMustHaveType; 
 +(eTriggerBuildExceptionType*) MotTriggerMustHaveTransitionType; 
 +(eTriggerBuildExceptionType*) ChargeTriggerMustHaveType; 
 +(eTriggerBuildExceptionType*) ChargeTriggerIllegalPercent; 
 +(eTriggerBuildExceptionType*) TriggerMustHaveId; 
 +(eTriggerBuildExceptionType*) TriggerMustHaveType; 


@end

 #define eTriggerBuildExceptionType_get_PlaceTriggerMustHaveType() [eTriggerBuildExceptionType PlaceTriggerMustHaveType] 
 #define eTriggerBuildExceptionType_get_PlaceTriggerMustHavePlace() [eTriggerBuildExceptionType PlaceTriggerMustHavePlace] 
 #define eTriggerBuildExceptionType_get_MotTriggerMustHaveType() [eTriggerBuildExceptionType MotTriggerMustHaveType] 
 #define eTriggerBuildExceptionType_get_MotTriggerMustHaveTransitionType() [eTriggerBuildExceptionType MotTriggerMustHaveTransitionType] 
 #define eTriggerBuildExceptionType_get_ChargeTriggerMustHaveType() [eTriggerBuildExceptionType ChargeTriggerMustHaveType] 
 #define eTriggerBuildExceptionType_get_ChargeTriggerIllegalPercent() [eTriggerBuildExceptionType ChargeTriggerIllegalPercent] 
 #define eTriggerBuildExceptionType_get_TriggerMustHaveId() [eTriggerBuildExceptionType TriggerMustHaveId] 
 #define eTriggerBuildExceptionType_get_TriggerMustHaveType() [eTriggerBuildExceptionType TriggerMustHaveType] 


#endif
