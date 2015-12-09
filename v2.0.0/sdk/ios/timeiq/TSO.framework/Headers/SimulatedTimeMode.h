#ifndef __JavaBridge_SimulatedTimeMode__
#define __JavaBridge_SimulatedTimeMode__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class SimulatedTimeMode;

typedef NS_ENUM(NSInteger, eSimulatedTimeMode)
{
 	SimulatedTimeMode_Displaced , 
 	SimulatedTimeMode_FasterByFactor , 

};

@interface SimulatedTimeMode : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (SimulatedTimeMode *)valueOfWithNSString:(NSString*)name;
+ (SimulatedTimeMode *)valueOf:(NSString*)name;
+ (SimulatedTimeMode *)parseWithInt:(int)name;
+ (SimulatedTimeMode *)fromIdWithInt:(int)name;
+ (SimulatedTimeMode *)fromId:(int)val;

+ (void)load;
-(eSimulatedTimeMode) ordinal;

 +(SimulatedTimeMode*) Displaced; 
 +(SimulatedTimeMode*) FasterByFactor; 


@end

 #define SimulatedTimeMode_get_Displaced() [SimulatedTimeMode Displaced] 
 #define SimulatedTimeMode_get_FasterByFactor() [SimulatedTimeMode FasterByFactor] 


#endif
