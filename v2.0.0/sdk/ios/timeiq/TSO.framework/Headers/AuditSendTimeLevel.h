#ifndef __JavaBridge_AuditSendTimeLevel__
#define __JavaBridge_AuditSendTimeLevel__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class AuditSendTimeLevel;

typedef NS_ENUM(NSInteger, eAuditSendTimeLevel)
{
 	AuditSendTimeLevel_SEND_TIME_LEVEL1 , 
 	AuditSendTimeLevel_SEND_TIME_LEVEL2 , 
 	AuditSendTimeLevel_SEND_TIME_LEVEL3 , 
 	AuditSendTimeLevel_SEND_TIME_LEVEL4 , 
 	AuditSendTimeLevel_SEND_TIME_LEVEL5 , 

};

@interface AuditSendTimeLevel : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (AuditSendTimeLevel *)valueOfWithNSString:(NSString*)name;
+ (AuditSendTimeLevel *)valueOf:(NSString*)name;
+ (AuditSendTimeLevel *)parseWithInt:(int)name;
+ (AuditSendTimeLevel *)fromIdWithInt:(int)name;
+ (AuditSendTimeLevel *)fromId:(int)val;

+ (void)load;
-(eAuditSendTimeLevel) ordinal;

 +(AuditSendTimeLevel*) SEND_TIME_LEVEL1; 
 +(AuditSendTimeLevel*) SEND_TIME_LEVEL2; 
 +(AuditSendTimeLevel*) SEND_TIME_LEVEL3; 
 +(AuditSendTimeLevel*) SEND_TIME_LEVEL4; 
 +(AuditSendTimeLevel*) SEND_TIME_LEVEL5; 


@end

 #define AuditSendTimeLevel_get_SEND_TIME_LEVEL1() [AuditSendTimeLevel SEND_TIME_LEVEL1] 
 #define AuditSendTimeLevel_get_SEND_TIME_LEVEL2() [AuditSendTimeLevel SEND_TIME_LEVEL2] 
 #define AuditSendTimeLevel_get_SEND_TIME_LEVEL3() [AuditSendTimeLevel SEND_TIME_LEVEL3] 
 #define AuditSendTimeLevel_get_SEND_TIME_LEVEL4() [AuditSendTimeLevel SEND_TIME_LEVEL4] 
 #define AuditSendTimeLevel_get_SEND_TIME_LEVEL5() [AuditSendTimeLevel SEND_TIME_LEVEL5] 


#endif
