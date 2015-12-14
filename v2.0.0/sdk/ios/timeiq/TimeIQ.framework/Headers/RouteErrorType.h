#ifndef __JavaBridge_RouteErrorType__
#define __JavaBridge_RouteErrorType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class RouteErrorType;

typedef NS_ENUM(NSInteger, eRouteErrorType)
{
 	RouteErrorType_NOT_AUTHENTICATED , 
 	RouteErrorType_EXTERNAL_PROVIDER_DID_NOT_RESPOND , 
 	RouteErrorType_EXTERNAL_PROVIDER_FAILURE , 
 	RouteErrorType_MISSING_MANDATORY_FIELD , 
 	RouteErrorType_ILLEGAL_PARAMETER_VALUE , 
 	RouteErrorType_SERVER_ERROR , 
 	RouteErrorType_EVENT_TOO_FAR_FOR_NAVIGATION , 
 	RouteErrorType_EVENT_IS_OVERDUE , 
 	RouteErrorType_EVENT_CANNOT_BE_REACHED_IN_TIME , 
 	RouteErrorType_NO_ROUTE_TO_EVENT , 
 	RouteErrorType_SEARCH_TERM_NOT_SUPPORTED , 
 	RouteErrorType_INSUFFICIENT_ACCURACY , 
 	RouteErrorType_UNRESOLVED_LOCATION , 

};

@interface RouteErrorType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (RouteErrorType *)valueOfWithNSString:(NSString*)name;
+ (RouteErrorType *)valueOf:(NSString*)name;
+ (RouteErrorType *)parseWithInt:(int)name;
+ (RouteErrorType *)fromIdWithInt:(int)name;
+ (RouteErrorType *)fromId:(int)val;

+ (void)load;
-(eRouteErrorType) ordinal;

 +(RouteErrorType*) NOT_AUTHENTICATED; 
 +(RouteErrorType*) EXTERNAL_PROVIDER_DID_NOT_RESPOND; 
 +(RouteErrorType*) EXTERNAL_PROVIDER_FAILURE; 
 +(RouteErrorType*) MISSING_MANDATORY_FIELD; 
 +(RouteErrorType*) ILLEGAL_PARAMETER_VALUE; 
 +(RouteErrorType*) SERVER_ERROR; 
 +(RouteErrorType*) EVENT_TOO_FAR_FOR_NAVIGATION; 
 +(RouteErrorType*) EVENT_IS_OVERDUE; 
 +(RouteErrorType*) EVENT_CANNOT_BE_REACHED_IN_TIME; 
 +(RouteErrorType*) NO_ROUTE_TO_EVENT; 
 +(RouteErrorType*) SEARCH_TERM_NOT_SUPPORTED; 
 +(RouteErrorType*) INSUFFICIENT_ACCURACY; 
 +(RouteErrorType*) UNRESOLVED_LOCATION; 


@end

 #define RouteErrorType_get_NOT_AUTHENTICATED() [RouteErrorType NOT_AUTHENTICATED] 
 #define RouteErrorType_get_EXTERNAL_PROVIDER_DID_NOT_RESPOND() [RouteErrorType EXTERNAL_PROVIDER_DID_NOT_RESPOND] 
 #define RouteErrorType_get_EXTERNAL_PROVIDER_FAILURE() [RouteErrorType EXTERNAL_PROVIDER_FAILURE] 
 #define RouteErrorType_get_MISSING_MANDATORY_FIELD() [RouteErrorType MISSING_MANDATORY_FIELD] 
 #define RouteErrorType_get_ILLEGAL_PARAMETER_VALUE() [RouteErrorType ILLEGAL_PARAMETER_VALUE] 
 #define RouteErrorType_get_SERVER_ERROR() [RouteErrorType SERVER_ERROR] 
 #define RouteErrorType_get_EVENT_TOO_FAR_FOR_NAVIGATION() [RouteErrorType EVENT_TOO_FAR_FOR_NAVIGATION] 
 #define RouteErrorType_get_EVENT_IS_OVERDUE() [RouteErrorType EVENT_IS_OVERDUE] 
 #define RouteErrorType_get_EVENT_CANNOT_BE_REACHED_IN_TIME() [RouteErrorType EVENT_CANNOT_BE_REACHED_IN_TIME] 
 #define RouteErrorType_get_NO_ROUTE_TO_EVENT() [RouteErrorType NO_ROUTE_TO_EVENT] 
 #define RouteErrorType_get_SEARCH_TERM_NOT_SUPPORTED() [RouteErrorType SEARCH_TERM_NOT_SUPPORTED] 
 #define RouteErrorType_get_INSUFFICIENT_ACCURACY() [RouteErrorType INSUFFICIENT_ACCURACY] 
 #define RouteErrorType_get_UNRESOLVED_LOCATION() [RouteErrorType UNRESOLVED_LOCATION] 


#endif
