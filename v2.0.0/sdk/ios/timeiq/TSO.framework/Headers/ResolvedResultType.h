#ifndef __JavaBridge_ResolvedResultType__
#define __JavaBridge_ResolvedResultType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ResolvedResultType;

typedef NS_ENUM(NSInteger, eResolvedResultType)
{
 	ResolvedResultType_PHONE , 
 	ResolvedResultType_ONLINE , 
 	ResolvedResultType_POI_LOCATION , 
 	ResolvedResultType_LOCATION_AREA , 
 	ResolvedResultType_LOCATION_POINT , 
 	ResolvedResultType_MY_PLACES , 
 	ResolvedResultType_INTEL_BUILDING , 
 	ResolvedResultType_UNKNOWN , 

};

@interface ResolvedResultType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ResolvedResultType *)valueOfWithNSString:(NSString*)name;
+ (ResolvedResultType *)valueOf:(NSString*)name;
+ (ResolvedResultType *)parseWithInt:(int)name;
+ (ResolvedResultType *)fromIdWithInt:(int)name;
+ (ResolvedResultType *)fromId:(int)val;

+ (void)load;
-(eResolvedResultType) ordinal;

 +(ResolvedResultType*) PHONE; 
 +(ResolvedResultType*) ONLINE; 
 +(ResolvedResultType*) POI_LOCATION; 
 +(ResolvedResultType*) LOCATION_AREA; 
 +(ResolvedResultType*) LOCATION_POINT; 
 +(ResolvedResultType*) MY_PLACES; 
 +(ResolvedResultType*) INTEL_BUILDING; 
 +(ResolvedResultType*) UNKNOWN; 


@end

 #define ResolvedResultType_get_PHONE() [ResolvedResultType PHONE] 
 #define ResolvedResultType_get_ONLINE() [ResolvedResultType ONLINE] 
 #define ResolvedResultType_get_POI_LOCATION() [ResolvedResultType POI_LOCATION] 
 #define ResolvedResultType_get_LOCATION_AREA() [ResolvedResultType LOCATION_AREA] 
 #define ResolvedResultType_get_LOCATION_POINT() [ResolvedResultType LOCATION_POINT] 
 #define ResolvedResultType_get_MY_PLACES() [ResolvedResultType MY_PLACES] 
 #define ResolvedResultType_get_INTEL_BUILDING() [ResolvedResultType INTEL_BUILDING] 
 #define ResolvedResultType_get_UNKNOWN() [ResolvedResultType UNKNOWN] 


#endif
