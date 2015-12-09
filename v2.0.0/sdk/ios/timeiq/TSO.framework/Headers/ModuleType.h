#ifndef __JavaBridge_ModuleType__
#define __JavaBridge_ModuleType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ModuleType;

typedef NS_ENUM(NSInteger, eModuleType)
{
 	ModuleType_Places , 
 	ModuleType_Visits , 
 	ModuleType_VisitInPlaces , 
 	ModuleType_Motion , 
 	ModuleType_ExternalPlaces , 
 	ModuleType_SemanticDetected , 

};

@interface ModuleType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ModuleType *)valueOfWithNSString:(NSString*)name;
+ (ModuleType *)valueOf:(NSString*)name;
+ (ModuleType *)parseWithInt:(int)name;
+ (ModuleType *)fromIdWithInt:(int)name;
+ (ModuleType *)fromId:(int)val;

+ (void)load;
-(eModuleType) ordinal;

 +(ModuleType*) Places; 
 +(ModuleType*) Visits; 
 +(ModuleType*) VisitInPlaces; 
 +(ModuleType*) Motion; 
 +(ModuleType*) ExternalPlaces; 
 +(ModuleType*) SemanticDetected; 


@end

 #define ModuleType_get_Places() [ModuleType Places] 
 #define ModuleType_get_Visits() [ModuleType Visits] 
 #define ModuleType_get_VisitInPlaces() [ModuleType VisitInPlaces] 
 #define ModuleType_get_Motion() [ModuleType Motion] 
 #define ModuleType_get_ExternalPlaces() [ModuleType ExternalPlaces] 
 #define ModuleType_get_SemanticDetected() [ModuleType SemanticDetected] 


#endif
