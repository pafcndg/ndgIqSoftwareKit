#ifndef __JavaBridge_TSOEventChangeType__
#define __JavaBridge_TSOEventChangeType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class TSOEventChangeType;

typedef NS_ENUM(NSInteger, eTSOEventChangeType)
{
 	TSOEventChangeType_ADD , 
 	TSOEventChangeType_UPDATE , 
 	TSOEventChangeType_DELETE , 

};

@interface TSOEventChangeType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (TSOEventChangeType *)valueOfWithNSString:(NSString*)name;
+ (TSOEventChangeType *)valueOf:(NSString*)name;
+ (TSOEventChangeType *)parseWithInt:(int)name;
+ (TSOEventChangeType *)fromIdWithInt:(int)name;
+ (TSOEventChangeType *)fromId:(int)val;

+ (void)load;
-(eTSOEventChangeType) ordinal;

 +(TSOEventChangeType*) ADD; 
 +(TSOEventChangeType*) UPDATE; 
 +(TSOEventChangeType*) DELETE; 


@end

 #define TSOEventChangeType_get_ADD() [TSOEventChangeType ADD] 
 #define TSOEventChangeType_get_UPDATE() [TSOEventChangeType UPDATE] 
 #define TSOEventChangeType_get_DELETE() [TSOEventChangeType DELETE] 


#endif
