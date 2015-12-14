#ifndef __JavaBridge_TriggerType__
#define __JavaBridge_TriggerType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class TriggerType;

typedef NS_ENUM(NSInteger, eTriggerType)
{
 	TriggerType_CHARGE , 
 	TriggerType_MOT , 
 	TriggerType_PLACE , 
 	TriggerType_TIME , 

};

@interface TriggerType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (TriggerType *)valueOfWithNSString:(NSString*)name;
+ (TriggerType *)valueOf:(NSString*)name;
+ (TriggerType *)parseWithInt:(int)name;
+ (TriggerType *)fromIdWithInt:(int)name;
+ (TriggerType *)fromId:(int)val;

+ (void)load;
-(eTriggerType) ordinal;

 +(TriggerType*) CHARGE; 
 +(TriggerType*) MOT; 
 +(TriggerType*) PLACE; 
 +(TriggerType*) TIME; 


@end

 #define TriggerType_get_CHARGE() [TriggerType CHARGE] 
 #define TriggerType_get_MOT() [TriggerType MOT] 
 #define TriggerType_get_PLACE() [TriggerType PLACE] 
 #define TriggerType_get_TIME() [TriggerType TIME] 


#endif
