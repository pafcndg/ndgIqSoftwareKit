#ifndef __JavaBridge_ModuleState__
#define __JavaBridge_ModuleState__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ModuleState;

typedef NS_ENUM(NSInteger, eModuleState)
{
 	ModuleState_Active , 
 	ModuleState_NonActive , 
 	ModuleState_Unknown , 

};

@interface ModuleState : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ModuleState *)valueOfWithNSString:(NSString*)name;
+ (ModuleState *)valueOf:(NSString*)name;
+ (ModuleState *)parseWithInt:(int)name;
+ (ModuleState *)fromIdWithInt:(int)name;
+ (ModuleState *)fromId:(int)val;

+ (void)load;
-(eModuleState) ordinal;

 +(ModuleState*) Active; 
 +(ModuleState*) NonActive; 
 +(ModuleState*) Unknown; 


@end

 #define ModuleState_get_Active() [ModuleState Active] 
 #define ModuleState_get_NonActive() [ModuleState NonActive] 
 #define ModuleState_get_Unknown() [ModuleState Unknown] 


#endif
