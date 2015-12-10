#ifndef __JavaBridge_TrafficIndication__
#define __JavaBridge_TrafficIndication__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class TrafficIndication;

typedef NS_ENUM(NSInteger, eTrafficIndication)
{
 	TrafficIndication_UNKNOWN , 
 	TrafficIndication_NO_TRAFFIC , 
 	TrafficIndication_LIGHT_TRAFFIC , 
 	TrafficIndication_MEDIUM_TRAFFIC , 
 	TrafficIndication_HEAVY_TRAFFIC , 

};

@interface TrafficIndication : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (TrafficIndication *)valueOfWithNSString:(NSString*)name;
+ (TrafficIndication *)valueOf:(NSString*)name;
+ (TrafficIndication *)parseWithInt:(int)name;
+ (TrafficIndication *)fromIdWithInt:(int)name;
+ (TrafficIndication *)fromId:(int)val;

+ (void)load;
-(eTrafficIndication) ordinal;

 +(TrafficIndication*) UNKNOWN; 
 +(TrafficIndication*) NO_TRAFFIC; 
 +(TrafficIndication*) LIGHT_TRAFFIC; 
 +(TrafficIndication*) MEDIUM_TRAFFIC; 
 +(TrafficIndication*) HEAVY_TRAFFIC; 


@end

 #define TrafficIndication_get_UNKNOWN() [TrafficIndication UNKNOWN] 
 #define TrafficIndication_get_NO_TRAFFIC() [TrafficIndication NO_TRAFFIC] 
 #define TrafficIndication_get_LIGHT_TRAFFIC() [TrafficIndication LIGHT_TRAFFIC] 
 #define TrafficIndication_get_MEDIUM_TRAFFIC() [TrafficIndication MEDIUM_TRAFFIC] 
 #define TrafficIndication_get_HEAVY_TRAFFIC() [TrafficIndication HEAVY_TRAFFIC] 


#endif
