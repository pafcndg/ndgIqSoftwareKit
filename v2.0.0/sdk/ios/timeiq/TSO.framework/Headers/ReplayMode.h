#ifndef __JavaBridge_ReplayMode__
#define __JavaBridge_ReplayMode__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ReplayMode;

typedef NS_ENUM(NSInteger, eReplayMode)
{
 	ReplayMode_BATCH , 
 	ReplayMode_FAST_FORWARD , 
 	ReplayMode_REAL_TIME , 
 	ReplayMode_INSANE , 
 	ReplayMode_LUDICROUS , 

};

@interface ReplayMode : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ReplayMode *)valueOfWithNSString:(NSString*)name;
+ (ReplayMode *)valueOf:(NSString*)name;
+ (ReplayMode *)parseWithInt:(int)name;
+ (ReplayMode *)fromIdWithInt:(int)name;
+ (ReplayMode *)fromId:(int)val;

+ (void)load;
-(eReplayMode) ordinal;

 +(ReplayMode*) BATCH; 
 +(ReplayMode*) FAST_FORWARD; 
 +(ReplayMode*) REAL_TIME; 
 +(ReplayMode*) INSANE; 
 +(ReplayMode*) LUDICROUS; 


@end

 #define ReplayMode_get_BATCH() [ReplayMode BATCH] 
 #define ReplayMode_get_FAST_FORWARD() [ReplayMode FAST_FORWARD] 
 #define ReplayMode_get_REAL_TIME() [ReplayMode REAL_TIME] 
 #define ReplayMode_get_INSANE() [ReplayMode INSANE] 
 #define ReplayMode_get_LUDICROUS() [ReplayMode LUDICROUS] 


#endif
