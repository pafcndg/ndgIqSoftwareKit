#ifndef __JavaBridge_LocationType__
#define __JavaBridge_LocationType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class LocationType;

typedef NS_ENUM(NSInteger, eLocationType)
{
 	LocationType_ADDRESS , 
 	LocationType_ONLINE , 
 	LocationType_PHONE , 
 	LocationType_POI , 
 	LocationType_USER_PLACES , 
 	LocationType_AREA , 
 	LocationType_INTEL_BUILDINGS , 
 	LocationType_UNKNOWN , 

};

@interface LocationType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (LocationType *)valueOfWithNSString:(NSString*)name;
+ (LocationType *)valueOf:(NSString*)name;
+ (LocationType *)parseWithInt:(int)name;
+ (LocationType *)fromIdWithInt:(int)name;
+ (LocationType *)fromId:(int)val;

+ (void)load;
-(eLocationType) ordinal;

 +(LocationType*) ADDRESS; 
 +(LocationType*) ONLINE; 
 +(LocationType*) PHONE; 
 +(LocationType*) POI; 
 +(LocationType*) USER_PLACES; 
 +(LocationType*) AREA; 
 +(LocationType*) INTEL_BUILDINGS; 
 +(LocationType*) UNKNOWN; 


@end

 #define LocationType_get_ADDRESS() [LocationType ADDRESS] 
 #define LocationType_get_ONLINE() [LocationType ONLINE] 
 #define LocationType_get_PHONE() [LocationType PHONE] 
 #define LocationType_get_POI() [LocationType POI] 
 #define LocationType_get_USER_PLACES() [LocationType USER_PLACES] 
 #define LocationType_get_AREA() [LocationType AREA] 
 #define LocationType_get_INTEL_BUILDINGS() [LocationType INTEL_BUILDINGS] 
 #define LocationType_get_UNKNOWN() [LocationType UNKNOWN] 


#endif
