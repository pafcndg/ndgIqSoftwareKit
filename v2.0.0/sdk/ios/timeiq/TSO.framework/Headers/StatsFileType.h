#ifndef __JavaBridge_StatsFileType__
#define __JavaBridge_StatsFileType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class StatsFileType;

typedef NS_ENUM(NSInteger, eStatsFileType)
{
 	StatsFileType_SENSORS , 
 	StatsFileType_POLICY , 
 	StatsFileType_DEVICE_STATE , 
 	StatsFileType_NETWORK_CONSUMPTION , 

};

@interface StatsFileType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (StatsFileType *)valueOfWithNSString:(NSString*)name;
+ (StatsFileType *)valueOf:(NSString*)name;
+ (StatsFileType *)parseWithInt:(int)name;
+ (StatsFileType *)fromIdWithInt:(int)name;
+ (StatsFileType *)fromId:(int)val;

+ (void)load;
-(eStatsFileType) ordinal;

 +(StatsFileType*) SENSORS; 
 +(StatsFileType*) POLICY; 
 +(StatsFileType*) DEVICE_STATE; 
 +(StatsFileType*) NETWORK_CONSUMPTION; 


@end

 #define StatsFileType_get_SENSORS() [StatsFileType SENSORS] 
 #define StatsFileType_get_POLICY() [StatsFileType POLICY] 
 #define StatsFileType_get_DEVICE_STATE() [StatsFileType DEVICE_STATE] 
 #define StatsFileType_get_NETWORK_CONSUMPTION() [StatsFileType NETWORK_CONSUMPTION] 


#endif
