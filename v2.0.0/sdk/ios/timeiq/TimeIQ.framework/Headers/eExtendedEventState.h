#ifndef __JavaBridge_eExtendedEventState__
#define __JavaBridge_eExtendedEventState__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class eExtendedEventState;

typedef NS_ENUM(NSInteger, eeExtendedEventState)
{
 	eExtendedEventState_NEW , 
 	eExtendedEventState_UPCOMING , 
 	eExtendedEventState_COMPLETE , 
 	eExtendedEventState_MISSED , 
 	eExtendedEventState_CANCEL , 
 	eExtendedEventState_DELETED , 
 	eExtendedEventState_ON_GOING , 

};

@interface eExtendedEventState : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (eExtendedEventState *)valueOfWithNSString:(NSString*)name;
+ (eExtendedEventState *)valueOf:(NSString*)name;
+ (eExtendedEventState *)parseWithInt:(int)name;
+ (eExtendedEventState *)fromIdWithInt:(int)name;
+ (eExtendedEventState *)fromId:(int)val;

+ (void)load;
-(eeExtendedEventState) ordinal;

 +(eExtendedEventState*) NEW; 
 +(eExtendedEventState*) UPCOMING; 
 +(eExtendedEventState*) COMPLETE; 
 +(eExtendedEventState*) MISSED; 
 +(eExtendedEventState*) CANCEL; 
 +(eExtendedEventState*) DELETED; 
 +(eExtendedEventState*) ON_GOING; 


@end

 #define eExtendedEventState_get_NEW() [eExtendedEventState NEW] 
 #define eExtendedEventState_get_UPCOMING() [eExtendedEventState UPCOMING] 
 #define eExtendedEventState_get_COMPLETE() [eExtendedEventState COMPLETE] 
 #define eExtendedEventState_get_MISSED() [eExtendedEventState MISSED] 
 #define eExtendedEventState_get_CANCEL() [eExtendedEventState CANCEL] 
 #define eExtendedEventState_get_DELETED() [eExtendedEventState DELETED] 
 #define eExtendedEventState_get_ON_GOING() [eExtendedEventState ON_GOING] 


#endif
