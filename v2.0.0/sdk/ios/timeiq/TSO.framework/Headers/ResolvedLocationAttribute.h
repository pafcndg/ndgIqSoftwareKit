#ifndef __JavaBridge_ResolvedLocationAttribute__
#define __JavaBridge_ResolvedLocationAttribute__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ResolvedLocationAttribute;

typedef NS_ENUM(NSInteger, eResolvedLocationAttribute)
{
 	ResolvedLocationAttribute_LOCATION_INDEPENDENT , 
 	ResolvedLocationAttribute_LOCATION_DEPENDENT , 

};

@interface ResolvedLocationAttribute : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ResolvedLocationAttribute *)valueOfWithNSString:(NSString*)name;
+ (ResolvedLocationAttribute *)valueOf:(NSString*)name;
+ (ResolvedLocationAttribute *)parseWithInt:(int)name;
+ (ResolvedLocationAttribute *)fromIdWithInt:(int)name;
+ (ResolvedLocationAttribute *)fromId:(int)val;

+ (void)load;
-(eResolvedLocationAttribute) ordinal;

 +(ResolvedLocationAttribute*) LOCATION_INDEPENDENT; 
 +(ResolvedLocationAttribute*) LOCATION_DEPENDENT; 


@end

 #define ResolvedLocationAttribute_get_LOCATION_INDEPENDENT() [ResolvedLocationAttribute LOCATION_INDEPENDENT] 
 #define ResolvedLocationAttribute_get_LOCATION_DEPENDENT() [ResolvedLocationAttribute LOCATION_DEPENDENT] 


#endif
