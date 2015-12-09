#ifndef __JavaBridge_LOG_LEVEL__
#define __JavaBridge_LOG_LEVEL__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class LOG_LEVEL;

typedef NS_ENUM(NSInteger, eLOG_LEVEL)
{
 	LOG_LEVEL_INFO , 
 	LOG_LEVEL_DEBUG_ , 
 	LOG_LEVEL_ERROR , 

};

@interface LOG_LEVEL : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (LOG_LEVEL *)valueOfWithNSString:(NSString*)name;
+ (LOG_LEVEL *)valueOf:(NSString*)name;
+ (LOG_LEVEL *)parseWithInt:(int)name;
+ (LOG_LEVEL *)fromIdWithInt:(int)name;
+ (LOG_LEVEL *)fromId:(int)val;

+ (void)load;
-(eLOG_LEVEL) ordinal;

 +(LOG_LEVEL*) INFO; 
 +(LOG_LEVEL*) DEBUG_; 
 +(LOG_LEVEL*) ERROR; 


@end

 #define LOG_LEVEL_get_INFO() [LOG_LEVEL INFO] 
 #define LOG_LEVEL_get_DEBUG_() [LOG_LEVEL DEBUG_] 
 #define LOG_LEVEL_get_ERROR() [LOG_LEVEL ERROR] 


#endif
