#ifndef __JavaBridge_TransportType__
#define __JavaBridge_TransportType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class TransportType;

typedef NS_ENUM(NSInteger, eTransportType)
{
 	TransportType_CAR , 
 	TransportType_WALK , 

};

@interface TransportType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (TransportType *)valueOfWithNSString:(NSString*)name;
+ (TransportType *)valueOf:(NSString*)name;
+ (TransportType *)parseWithInt:(int)name;
+ (TransportType *)fromIdWithInt:(int)name;
+ (TransportType *)fromId:(int)val;

+ (void)load;
-(eTransportType) ordinal;

 +(TransportType*) CAR; 
 +(TransportType*) WALK; 


@end

 #define TransportType_get_CAR() [TransportType CAR] 
 #define TransportType_get_WALK() [TransportType WALK] 


#endif
