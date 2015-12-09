#ifndef __JavaBridge_ModuleProductType__
#define __JavaBridge_ModuleProductType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ModuleProductType;

typedef NS_ENUM(NSInteger, eModuleProductType)
{
 	ModuleProductType_VISIT_START , 
 	ModuleProductType_VISIT_END , 
 	ModuleProductType_MOT_CHANGED , 

};

@interface ModuleProductType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ModuleProductType *)valueOfWithNSString:(NSString*)name;
+ (ModuleProductType *)valueOf:(NSString*)name;
+ (ModuleProductType *)parseWithInt:(int)name;
+ (ModuleProductType *)fromIdWithInt:(int)name;
+ (ModuleProductType *)fromId:(int)val;

+ (void)load;
-(eModuleProductType) ordinal;

 +(ModuleProductType*) VISIT_START; 
 +(ModuleProductType*) VISIT_END; 
 +(ModuleProductType*) MOT_CHANGED; 


@end

 #define ModuleProductType_get_VISIT_START() [ModuleProductType VISIT_START] 
 #define ModuleProductType_get_VISIT_END() [ModuleProductType VISIT_END] 
 #define ModuleProductType_get_MOT_CHANGED() [ModuleProductType MOT_CHANGED] 


#endif
