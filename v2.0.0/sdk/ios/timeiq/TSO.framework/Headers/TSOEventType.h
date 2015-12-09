#ifndef __JavaBridge_TSOEventType__
#define __JavaBridge_TSOEventType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class TSOEventType;

typedef NS_ENUM(NSInteger, eTSOEventType)
{
 	TSOEventType_BE , 
 	TSOEventType_CALENDAR , 

};

@interface TSOEventType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (TSOEventType *)valueOfWithNSString:(NSString*)name;
+ (TSOEventType *)valueOf:(NSString*)name;
+ (TSOEventType *)parseWithInt:(int)name;
+ (TSOEventType *)fromIdWithInt:(int)name;
+ (TSOEventType *)fromId:(int)val;

+ (void)load;
-(eTSOEventType) ordinal;

 +(TSOEventType*) BE; 
 +(TSOEventType*) CALENDAR; 


@end

 #define TSOEventType_get_BE() [TSOEventType BE] 
 #define TSOEventType_get_CALENDAR() [TSOEventType CALENDAR] 


#endif
