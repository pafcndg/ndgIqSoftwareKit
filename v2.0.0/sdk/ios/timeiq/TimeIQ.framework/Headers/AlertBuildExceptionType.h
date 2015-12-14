#ifndef __JavaBridge_AlertBuildExceptionType__
#define __JavaBridge_AlertBuildExceptionType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class AlertBuildExceptionType;

typedef NS_ENUM(NSInteger, eAlertBuildExceptionType)
{
 	AlertBuildExceptionType_AlertMustHaveALocation , 
 	AlertBuildExceptionType_AlertLocationMustHaveCoordinates , 
 	AlertBuildExceptionType_AlertArrivalTimeMustBeInTheFuture , 
 	AlertBuildExceptionType_AlertMustHaveAtLeastOneReminder , 

};

@interface AlertBuildExceptionType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (AlertBuildExceptionType *)valueOfWithNSString:(NSString*)name;
+ (AlertBuildExceptionType *)valueOf:(NSString*)name;
+ (AlertBuildExceptionType *)parseWithInt:(int)name;
+ (AlertBuildExceptionType *)fromIdWithInt:(int)name;
+ (AlertBuildExceptionType *)fromId:(int)val;

+ (void)load;
-(eAlertBuildExceptionType) ordinal;

 +(AlertBuildExceptionType*) AlertMustHaveALocation; 
 +(AlertBuildExceptionType*) AlertLocationMustHaveCoordinates; 
 +(AlertBuildExceptionType*) AlertArrivalTimeMustBeInTheFuture; 
 +(AlertBuildExceptionType*) AlertMustHaveAtLeastOneReminder; 


@end

 #define AlertBuildExceptionType_get_AlertMustHaveALocation() [AlertBuildExceptionType AlertMustHaveALocation] 
 #define AlertBuildExceptionType_get_AlertLocationMustHaveCoordinates() [AlertBuildExceptionType AlertLocationMustHaveCoordinates] 
 #define AlertBuildExceptionType_get_AlertArrivalTimeMustBeInTheFuture() [AlertBuildExceptionType AlertArrivalTimeMustBeInTheFuture] 
 #define AlertBuildExceptionType_get_AlertMustHaveAtLeastOneReminder() [AlertBuildExceptionType AlertMustHaveAtLeastOneReminder] 


#endif
