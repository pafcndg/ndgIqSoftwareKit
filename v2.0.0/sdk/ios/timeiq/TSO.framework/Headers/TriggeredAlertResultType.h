#ifndef __JavaBridge_TriggeredAlertResultType__
#define __JavaBridge_TriggeredAlertResultType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class TriggeredAlertResultType;

typedef NS_ENUM(NSInteger, eTriggeredAlertResultType)
{
 	TriggeredAlertResultType_TimeToLeaveAlert , 
 	TriggeredAlertResultType_LateAlert , 

};

@interface TriggeredAlertResultType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (TriggeredAlertResultType *)valueOfWithNSString:(NSString*)name;
+ (TriggeredAlertResultType *)valueOf:(NSString*)name;
+ (TriggeredAlertResultType *)parseWithInt:(int)name;
+ (TriggeredAlertResultType *)fromIdWithInt:(int)name;
+ (TriggeredAlertResultType *)fromId:(int)val;

+ (void)load;
-(eTriggeredAlertResultType) ordinal;

 +(TriggeredAlertResultType*) TimeToLeaveAlert; 
 +(TriggeredAlertResultType*) LateAlert; 


@end

 #define TriggeredAlertResultType_get_TimeToLeaveAlert() [TriggeredAlertResultType TimeToLeaveAlert] 
 #define TriggeredAlertResultType_get_LateAlert() [TriggeredAlertResultType LateAlert] 


#endif
