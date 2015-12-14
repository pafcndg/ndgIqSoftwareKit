#ifndef __JavaBridge_TriggerStatusInner__
#define __JavaBridge_TriggerStatusInner__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class TriggerStatusInner;

typedef NS_ENUM(NSInteger, eTriggerStatusInner)
{
 	TriggerStatusInner_New , 
 	TriggerStatusInner_Registered , 
 	TriggerStatusInner_Triggered , 
 	TriggerStatusInner_Ended , 

};

@interface TriggerStatusInner : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (TriggerStatusInner *)valueOfWithNSString:(NSString*)name;
+ (TriggerStatusInner *)valueOf:(NSString*)name;
+ (TriggerStatusInner *)parseWithInt:(int)name;
+ (TriggerStatusInner *)fromIdWithInt:(int)name;
+ (TriggerStatusInner *)fromId:(int)val;

+ (void)load;
-(eTriggerStatusInner) ordinal;

 +(TriggerStatusInner*) New; 
 +(TriggerStatusInner*) Registered; 
 +(TriggerStatusInner*) Triggered; 
 +(TriggerStatusInner*) Ended; 


@end

 #define TriggerStatusInner_get_New() [TriggerStatusInner New] 
 #define TriggerStatusInner_get_Registered() [TriggerStatusInner Registered] 
 #define TriggerStatusInner_get_Triggered() [TriggerStatusInner Triggered] 
 #define TriggerStatusInner_get_Ended() [TriggerStatusInner Ended] 


#endif
