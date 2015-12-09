#ifndef __JavaBridge_PlaceStatus__
#define __JavaBridge_PlaceStatus__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class PlaceStatus;

typedef NS_ENUM(NSInteger, ePlaceStatus)
{
 	PlaceStatus_PLACE_STATUS_ACTIVE , 
 	PlaceStatus_PLACE_STATUS_INACTIVE , 

};

@interface PlaceStatus : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (PlaceStatus *)valueOfWithNSString:(NSString*)name;
+ (PlaceStatus *)valueOf:(NSString*)name;
+ (PlaceStatus *)parseWithInt:(int)name;
+ (PlaceStatus *)fromIdWithInt:(int)name;
+ (PlaceStatus *)fromId:(int)val;

+ (void)load;
-(ePlaceStatus) ordinal;

 +(PlaceStatus*) PLACE_STATUS_ACTIVE; 
 +(PlaceStatus*) PLACE_STATUS_INACTIVE; 


@end

 #define PlaceStatus_get_PLACE_STATUS_ACTIVE() [PlaceStatus PLACE_STATUS_ACTIVE] 
 #define PlaceStatus_get_PLACE_STATUS_INACTIVE() [PlaceStatus PLACE_STATUS_INACTIVE] 


#endif
