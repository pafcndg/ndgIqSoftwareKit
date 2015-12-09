#ifndef __JavaBridge_TriggerStatus__
#define __JavaBridge_TriggerStatus__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class TriggerStatus;

typedef NS_ENUM(NSInteger, eTriggerStatus)
{
 	TriggerStatus_NA , 
 	TriggerStatus_Active , 
 	TriggerStatus_Triggered , 

};

@interface TriggerStatus : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (TriggerStatus *)valueOfWithNSString:(NSString*)name;
+ (TriggerStatus *)valueOf:(NSString*)name;
+ (TriggerStatus *)parseWithInt:(int)name;
+ (TriggerStatus *)fromIdWithInt:(int)name;
+ (TriggerStatus *)fromId:(int)val;

+ (void)load;
-(eTriggerStatus) ordinal;

 +(TriggerStatus*) NA; 
 +(TriggerStatus*) Active; 
 +(TriggerStatus*) Triggered; 


@end

 #define TriggerStatus_get_NA() [TriggerStatus NA] 
 #define TriggerStatus_get_Active() [TriggerStatus Active] 
 #define TriggerStatus_get_Triggered() [TriggerStatus Triggered] 


#endif
