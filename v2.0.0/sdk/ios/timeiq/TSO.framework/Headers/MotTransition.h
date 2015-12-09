#ifndef __JavaBridge_MotTransition__
#define __JavaBridge_MotTransition__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class MotTransition;

typedef NS_ENUM(NSInteger, eMotTransition)
{
 	MotTransition_START , 
 	MotTransition_DURING , 
 	MotTransition_State , 
 	MotTransition_END , 

};

@interface MotTransition : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (MotTransition *)valueOfWithNSString:(NSString*)name;
+ (MotTransition *)valueOf:(NSString*)name;
+ (MotTransition *)parseWithInt:(int)name;
+ (MotTransition *)fromIdWithInt:(int)name;
+ (MotTransition *)fromId:(int)val;

+ (void)load;
-(eMotTransition) ordinal;

 +(MotTransition*) START; 
 +(MotTransition*) DURING; 
 +(MotTransition*) State; 
 +(MotTransition*) END; 


@end

 #define MotTransition_get_START() [MotTransition START] 
 #define MotTransition_get_DURING() [MotTransition DURING] 
 #define MotTransition_get_State() [MotTransition State] 
 #define MotTransition_get_END() [MotTransition END] 


#endif
