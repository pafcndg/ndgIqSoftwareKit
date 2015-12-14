#ifndef __JavaBridge_ResolverApiType__
#define __JavaBridge_ResolverApiType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class ResolverApiType;

typedef NS_ENUM(NSInteger, eResolverApiType)
{
 	ResolverApiType_RESOLVE_LOCATION , 
 	ResolverApiType_RESOLVE_PHONE , 
 	ResolverApiType_SEARCH_AS_YOU_TYPE , 
 	ResolverApiType_GET_DETAILS , 

};

@interface ResolverApiType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ResolverApiType *)valueOfWithNSString:(NSString*)name;
+ (ResolverApiType *)valueOf:(NSString*)name;
+ (ResolverApiType *)parseWithInt:(int)name;
+ (ResolverApiType *)fromIdWithInt:(int)name;
+ (ResolverApiType *)fromId:(int)val;

+ (void)load;
-(eResolverApiType) ordinal;

 +(ResolverApiType*) RESOLVE_LOCATION; 
 +(ResolverApiType*) RESOLVE_PHONE; 
 +(ResolverApiType*) SEARCH_AS_YOU_TYPE; 
 +(ResolverApiType*) GET_DETAILS; 


@end

 #define ResolverApiType_get_RESOLVE_LOCATION() [ResolverApiType RESOLVE_LOCATION] 
 #define ResolverApiType_get_RESOLVE_PHONE() [ResolverApiType RESOLVE_PHONE] 
 #define ResolverApiType_get_SEARCH_AS_YOU_TYPE() [ResolverApiType SEARCH_AS_YOU_TYPE] 
 #define ResolverApiType_get_GET_DETAILS() [ResolverApiType GET_DETAILS] 


#endif
