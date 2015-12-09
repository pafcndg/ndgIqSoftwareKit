#ifndef __JavaBridge_AlertState__
#define __JavaBridge_AlertState__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class AlertState;

typedef NS_ENUM(NSInteger, eAlertState)
{
 	AlertState_UN_REGISTERED , 
 	AlertState_REGISTERED , 
 	AlertState_WAITING_BEFORE_START , 
 	AlertState_WAITING , 
 	AlertState_RETRYING , 
 	AlertState_DONE , 

};

@interface AlertState : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (AlertState *)valueOfWithNSString:(NSString*)name;
+ (AlertState *)valueOf:(NSString*)name;
+ (AlertState *)parseWithInt:(int)name;
+ (AlertState *)fromIdWithInt:(int)name;
+ (AlertState *)fromId:(int)val;

+ (void)load;
-(eAlertState) ordinal;

 +(AlertState*) UN_REGISTERED; 
 +(AlertState*) REGISTERED; 
 +(AlertState*) WAITING_BEFORE_START; 
 +(AlertState*) WAITING; 
 +(AlertState*) RETRYING; 
 +(AlertState*) DONE; 


@end

 #define AlertState_get_UN_REGISTERED() [AlertState UN_REGISTERED] 
 #define AlertState_get_REGISTERED() [AlertState REGISTERED] 
 #define AlertState_get_WAITING_BEFORE_START() [AlertState WAITING_BEFORE_START] 
 #define AlertState_get_WAITING() [AlertState WAITING] 
 #define AlertState_get_RETRYING() [AlertState RETRYING] 
 #define AlertState_get_DONE() [AlertState DONE] 


#endif
