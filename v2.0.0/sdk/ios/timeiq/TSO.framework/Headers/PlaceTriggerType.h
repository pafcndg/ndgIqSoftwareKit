#ifndef __JavaBridge_PlaceTriggerType__
#define __JavaBridge_PlaceTriggerType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class PlaceTriggerType;

typedef NS_ENUM(NSInteger, ePlaceTriggerType)
{
 	PlaceTriggerType_ARRIVE , 
 	PlaceTriggerType_LEAVE , 

};

@interface PlaceTriggerType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (PlaceTriggerType *)valueOfWithNSString:(NSString*)name;
+ (PlaceTriggerType *)valueOf:(NSString*)name;
+ (PlaceTriggerType *)parseWithInt:(int)name;
+ (PlaceTriggerType *)fromIdWithInt:(int)name;
+ (PlaceTriggerType *)fromId:(int)val;

+ (void)load;
-(ePlaceTriggerType) ordinal;

 +(PlaceTriggerType*) ARRIVE; 
 +(PlaceTriggerType*) LEAVE; 


@end

 #define PlaceTriggerType_get_ARRIVE() [PlaceTriggerType ARRIVE] 
 #define PlaceTriggerType_get_LEAVE() [PlaceTriggerType LEAVE] 


#endif
