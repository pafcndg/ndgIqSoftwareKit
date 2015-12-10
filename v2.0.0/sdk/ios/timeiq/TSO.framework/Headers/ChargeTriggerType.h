#ifndef __JavaBridge_ChargeTriggerType__
#define __JavaBridge_ChargeTriggerType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class ChargeTriggerType;

typedef NS_ENUM(NSInteger, eChargeTriggerType)
{
 	ChargeTriggerType_CHARGING , 
 	ChargeTriggerType_NOT_CHARGING , 
 	ChargeTriggerType_BATTERY_LOW , 
 	ChargeTriggerType_BATTERY_OK , 

};

@interface ChargeTriggerType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ChargeTriggerType *)valueOfWithNSString:(NSString*)name;
+ (ChargeTriggerType *)valueOf:(NSString*)name;
+ (ChargeTriggerType *)parseWithInt:(int)name;
+ (ChargeTriggerType *)fromIdWithInt:(int)name;
+ (ChargeTriggerType *)fromId:(int)val;

+ (void)load;
-(eChargeTriggerType) ordinal;

 +(ChargeTriggerType*) CHARGING; 
 +(ChargeTriggerType*) NOT_CHARGING; 
 +(ChargeTriggerType*) BATTERY_LOW; 
 +(ChargeTriggerType*) BATTERY_OK; 


@end

 #define ChargeTriggerType_get_CHARGING() [ChargeTriggerType CHARGING] 
 #define ChargeTriggerType_get_NOT_CHARGING() [ChargeTriggerType NOT_CHARGING] 
 #define ChargeTriggerType_get_BATTERY_LOW() [ChargeTriggerType BATTERY_LOW] 
 #define ChargeTriggerType_get_BATTERY_OK() [ChargeTriggerType BATTERY_OK] 


#endif
