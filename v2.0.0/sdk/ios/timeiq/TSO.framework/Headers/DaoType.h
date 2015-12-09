#ifndef __JavaBridge_DaoType__
#define __JavaBridge_DaoType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class DaoType;

typedef NS_ENUM(NSInteger, eDaoType)
{
 	DaoType_MANUAL_PLACE , 
 	DaoType_DETECTED_PLACE , 
 	DaoType_VISIT , 
 	DaoType_REF_POINT , 
 	DaoType_POLICY_HISTORY , 
 	DaoType_SEMANTIC_DETECTED_PLACE , 
 	DaoType_MOT , 
 	DaoType_USER_PLACE_INTERACTION , 

};

@interface DaoType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (DaoType *)valueOfWithNSString:(NSString*)name;
+ (DaoType *)valueOf:(NSString*)name;
+ (DaoType *)parseWithInt:(int)name;
+ (DaoType *)fromIdWithInt:(int)name;
+ (DaoType *)fromId:(int)val;

+ (void)load;
-(eDaoType) ordinal;

 +(DaoType*) MANUAL_PLACE; 
 +(DaoType*) DETECTED_PLACE; 
 +(DaoType*) VISIT; 
 +(DaoType*) REF_POINT; 
 +(DaoType*) POLICY_HISTORY; 
 +(DaoType*) SEMANTIC_DETECTED_PLACE; 
 +(DaoType*) MOT; 
 +(DaoType*) USER_PLACE_INTERACTION; 


@end

 #define DaoType_get_MANUAL_PLACE() [DaoType MANUAL_PLACE] 
 #define DaoType_get_DETECTED_PLACE() [DaoType DETECTED_PLACE] 
 #define DaoType_get_VISIT() [DaoType VISIT] 
 #define DaoType_get_REF_POINT() [DaoType REF_POINT] 
 #define DaoType_get_POLICY_HISTORY() [DaoType POLICY_HISTORY] 
 #define DaoType_get_SEMANTIC_DETECTED_PLACE() [DaoType SEMANTIC_DETECTED_PLACE] 
 #define DaoType_get_MOT() [DaoType MOT] 
 #define DaoType_get_USER_PLACE_INTERACTION() [DaoType USER_PLACE_INTERACTION] 


#endif
