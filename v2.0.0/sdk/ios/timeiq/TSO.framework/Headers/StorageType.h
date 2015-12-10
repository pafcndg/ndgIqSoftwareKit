#ifndef __JavaBridge_StorageType__
#define __JavaBridge_StorageType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class StorageType;

typedef NS_ENUM(NSInteger, eStorageType)
{
 	StorageType_AUDIT , 

};

@interface StorageType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (StorageType *)valueOfWithNSString:(NSString*)name;
+ (StorageType *)valueOf:(NSString*)name;
+ (StorageType *)parseWithInt:(int)name;
+ (StorageType *)fromIdWithInt:(int)name;
+ (StorageType *)fromId:(int)val;

+ (void)load;
-(eStorageType) ordinal;

 +(StorageType*) AUDIT; 


@end

 #define StorageType_get_AUDIT() [StorageType AUDIT] 


#endif
