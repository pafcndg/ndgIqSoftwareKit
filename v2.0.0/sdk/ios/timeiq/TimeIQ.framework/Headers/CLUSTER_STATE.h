#ifndef __JavaBridge_CLUSTER_STATE__
#define __JavaBridge_CLUSTER_STATE__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class CLUSTER_STATE;

typedef NS_ENUM(NSInteger, eCLUSTER_STATE)
{
 	CLUSTER_STATE_BEGIN , 
 	CLUSTER_STATE_PAUSE , 
 	CLUSTER_STATE_STAY , 
 	CLUSTER_STATE_TRAVEL , 
 	CLUSTER_STATE_UNKNOWN , 

};

@interface CLUSTER_STATE : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (CLUSTER_STATE *)valueOfWithNSString:(NSString*)name;
+ (CLUSTER_STATE *)valueOf:(NSString*)name;
+ (CLUSTER_STATE *)parseWithInt:(int)name;
+ (CLUSTER_STATE *)fromIdWithInt:(int)name;
+ (CLUSTER_STATE *)fromId:(int)val;

+ (void)load;
-(eCLUSTER_STATE) ordinal;

 +(CLUSTER_STATE*) BEGIN; 
 +(CLUSTER_STATE*) PAUSE; 
 +(CLUSTER_STATE*) STAY; 
 +(CLUSTER_STATE*) TRAVEL; 
 +(CLUSTER_STATE*) UNKNOWN; 


@end

 #define CLUSTER_STATE_get_BEGIN() [CLUSTER_STATE BEGIN] 
 #define CLUSTER_STATE_get_PAUSE() [CLUSTER_STATE PAUSE] 
 #define CLUSTER_STATE_get_STAY() [CLUSTER_STATE STAY] 
 #define CLUSTER_STATE_get_TRAVEL() [CLUSTER_STATE TRAVEL] 
 #define CLUSTER_STATE_get_UNKNOWN() [CLUSTER_STATE UNKNOWN] 


#endif
