#ifndef __JavaBridge_PolicyMode__
#define __JavaBridge_PolicyMode__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class PolicyMode;

typedef NS_ENUM(NSInteger, ePolicyMode)
{
 	PolicyMode_REGULAR , 
 	PolicyMode_STATIC , 
 	PolicyMode_POWER_SAVE , 
 	PolicyMode_POWER_SAVE_STATIC , 
 	PolicyMode_POWER_CRITICAL , 
 	PolicyMode_POWER_CRITICAL_STATIC , 
 	PolicyMode_TRANSFER , 

};

@interface PolicyMode : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (PolicyMode *)valueOfWithNSString:(NSString*)name;
+ (PolicyMode *)valueOf:(NSString*)name;
+ (PolicyMode *)parseWithInt:(int)name;
+ (PolicyMode *)fromIdWithInt:(int)name;
+ (PolicyMode *)fromId:(int)val;

+ (void)load;
-(ePolicyMode) ordinal;

 +(PolicyMode*) REGULAR; 
 +(PolicyMode*) STATIC; 
 +(PolicyMode*) POWER_SAVE; 
 +(PolicyMode*) POWER_SAVE_STATIC; 
 +(PolicyMode*) POWER_CRITICAL; 
 +(PolicyMode*) POWER_CRITICAL_STATIC; 
 +(PolicyMode*) TRANSFER; 


@end

 #define PolicyMode_get_REGULAR() [PolicyMode REGULAR] 
 #define PolicyMode_get_STATIC() [PolicyMode STATIC] 
 #define PolicyMode_get_POWER_SAVE() [PolicyMode POWER_SAVE] 
 #define PolicyMode_get_POWER_SAVE_STATIC() [PolicyMode POWER_SAVE_STATIC] 
 #define PolicyMode_get_POWER_CRITICAL() [PolicyMode POWER_CRITICAL] 
 #define PolicyMode_get_POWER_CRITICAL_STATIC() [PolicyMode POWER_CRITICAL_STATIC] 
 #define PolicyMode_get_TRANSFER() [PolicyMode TRANSFER] 


#endif
