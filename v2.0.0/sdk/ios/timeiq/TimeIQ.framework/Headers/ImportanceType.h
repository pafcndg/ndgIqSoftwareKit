#ifndef __JavaBridge_ImportanceType__
#define __JavaBridge_ImportanceType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class ImportanceType;

typedef NS_ENUM(NSInteger, eImportanceType)
{
 	ImportanceType_HIGH , 
 	ImportanceType_LOW , 

};

@interface ImportanceType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ImportanceType *)valueOfWithNSString:(NSString*)name;
+ (ImportanceType *)valueOf:(NSString*)name;
+ (ImportanceType *)parseWithInt:(int)name;
+ (ImportanceType *)fromIdWithInt:(int)name;
+ (ImportanceType *)fromId:(int)val;

+ (void)load;
-(eImportanceType) ordinal;

 +(ImportanceType*) HIGH; 
 +(ImportanceType*) LOW; 


@end

 #define ImportanceType_get_HIGH() [ImportanceType HIGH] 
 #define ImportanceType_get_LOW() [ImportanceType LOW] 


#endif
