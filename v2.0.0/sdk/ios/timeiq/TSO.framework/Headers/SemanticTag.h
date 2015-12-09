#ifndef __JavaBridge_SemanticTag__
#define __JavaBridge_SemanticTag__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class SemanticTag;

typedef NS_ENUM(NSInteger, eSemanticTag)
{
 	SemanticTag_PLACE_SEMATIC_HOME , 
 	SemanticTag_PLACE_SEMATIC_WORK , 
 	SemanticTag_PLACE_SEMATIC_UNKOWN , 

};

@interface SemanticTag : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (SemanticTag *)valueOfWithNSString:(NSString*)name;
+ (SemanticTag *)valueOf:(NSString*)name;
+ (SemanticTag *)parseWithInt:(int)name;
+ (SemanticTag *)fromIdWithInt:(int)name;
+ (SemanticTag *)fromId:(int)val;

+ (void)load;
-(eSemanticTag) ordinal;

 +(SemanticTag*) PLACE_SEMATIC_HOME; 
 +(SemanticTag*) PLACE_SEMATIC_WORK; 
 +(SemanticTag*) PLACE_SEMATIC_UNKOWN; 


@end

 #define SemanticTag_get_PLACE_SEMATIC_HOME() [SemanticTag PLACE_SEMATIC_HOME] 
 #define SemanticTag_get_PLACE_SEMATIC_WORK() [SemanticTag PLACE_SEMATIC_WORK] 
 #define SemanticTag_get_PLACE_SEMATIC_UNKOWN() [SemanticTag PLACE_SEMATIC_UNKOWN] 


#endif
