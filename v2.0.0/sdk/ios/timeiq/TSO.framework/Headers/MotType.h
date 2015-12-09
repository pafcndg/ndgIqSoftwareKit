#ifndef __JavaBridge_MotType__
#define __JavaBridge_MotType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class MotType;

typedef NS_ENUM(NSInteger, eMotType)
{
 	MotType_CAR , 
 	MotType_WALK , 
 	MotType_PUBLIC_TRANSPORT , 
 	MotType_STATIONARY , 

};

@interface MotType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (MotType *)valueOfWithNSString:(NSString*)name;
+ (MotType *)valueOf:(NSString*)name;
+ (MotType *)parseWithInt:(int)name;
+ (MotType *)fromIdWithInt:(int)name;
+ (MotType *)fromId:(int)val;

+ (void)load;
-(eMotType) ordinal;

 +(MotType*) CAR; 
 +(MotType*) WALK; 
 +(MotType*) PUBLIC_TRANSPORT; 
 +(MotType*) STATIONARY; 


@end

 #define MotType_get_CAR() [MotType CAR] 
 #define MotType_get_WALK() [MotType WALK] 
 #define MotType_get_PUBLIC_TRANSPORT() [MotType PUBLIC_TRANSPORT] 
 #define MotType_get_STATIONARY() [MotType STATIONARY] 


#endif
