#ifndef __JavaBridge_UpdateAlertResultType__
#define __JavaBridge_UpdateAlertResultType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class UpdateAlertResultType;

typedef NS_ENUM(NSInteger, eUpdateAlertResultType)
{
 	UpdateAlertResultType_TtlUpdated , 
 	UpdateAlertResultType_AlertTerminated_CannotReachDestinationInTime , 
 	UpdateAlertResultType_AlertTerminated_EventIsOverdue , 

};

@interface UpdateAlertResultType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (UpdateAlertResultType *)valueOfWithNSString:(NSString*)name;
+ (UpdateAlertResultType *)valueOf:(NSString*)name;
+ (UpdateAlertResultType *)parseWithInt:(int)name;
+ (UpdateAlertResultType *)fromIdWithInt:(int)name;
+ (UpdateAlertResultType *)fromId:(int)val;

+ (void)load;
-(eUpdateAlertResultType) ordinal;

 +(UpdateAlertResultType*) TtlUpdated; 
 +(UpdateAlertResultType*) AlertTerminated_CannotReachDestinationInTime; 
 +(UpdateAlertResultType*) AlertTerminated_EventIsOverdue; 


@end

 #define UpdateAlertResultType_get_TtlUpdated() [UpdateAlertResultType TtlUpdated] 
 #define UpdateAlertResultType_get_AlertTerminated_CannotReachDestinationInTime() [UpdateAlertResultType AlertTerminated_CannotReachDestinationInTime] 
 #define UpdateAlertResultType_get_AlertTerminated_EventIsOverdue() [UpdateAlertResultType AlertTerminated_EventIsOverdue] 


#endif
