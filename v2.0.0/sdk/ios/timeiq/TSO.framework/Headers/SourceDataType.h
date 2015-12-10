#ifndef __JavaBridge_SourceDataType__
#define __JavaBridge_SourceDataType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class SourceDataType;

typedef NS_ENUM(NSInteger, eSourceDataType)
{
 	SourceDataType_USER_DEFINED , 
 	SourceDataType_DETECTED , 
 	SourceDataType_SEMANTIC_DETECTED , 

};

@interface SourceDataType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (SourceDataType *)valueOfWithNSString:(NSString*)name;
+ (SourceDataType *)valueOf:(NSString*)name;
+ (SourceDataType *)parseWithInt:(int)name;
+ (SourceDataType *)fromIdWithInt:(int)name;
+ (SourceDataType *)fromId:(int)val;

+ (void)load;
-(eSourceDataType) ordinal;

 +(SourceDataType*) USER_DEFINED; 
 +(SourceDataType*) DETECTED; 
 +(SourceDataType*) SEMANTIC_DETECTED; 


@end

 #define SourceDataType_get_USER_DEFINED() [SourceDataType USER_DEFINED] 
 #define SourceDataType_get_DETECTED() [SourceDataType DETECTED] 
 #define SourceDataType_get_SEMANTIC_DETECTED() [SourceDataType SEMANTIC_DETECTED] 


#endif
