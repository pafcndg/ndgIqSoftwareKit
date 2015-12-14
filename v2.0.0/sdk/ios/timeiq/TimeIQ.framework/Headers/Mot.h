#ifndef __JavaBridge_Mot__
#define __JavaBridge_Mot__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class Mot;

typedef NS_ENUM(NSInteger, eMot)
{
 	Mot_STATIONARY , 
 	Mot_WALKING , 
 	Mot_DRIVING , 
 	Mot_BUS , 
 	Mot_TRAIN , 
 	Mot_CYCLING , 
 	Mot_RUNNING , 
 	Mot_UNKNOWN , 

};

@interface Mot : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (Mot *)valueOfWithNSString:(NSString*)name;
+ (Mot *)valueOf:(NSString*)name;
+ (Mot *)parseWithInt:(int)name;
+ (Mot *)fromIdWithInt:(int)name;
+ (Mot *)fromId:(int)val;

+ (void)load;
-(eMot) ordinal;

 +(Mot*) STATIONARY; 
 +(Mot*) WALKING; 
 +(Mot*) DRIVING; 
 +(Mot*) BUS; 
 +(Mot*) TRAIN; 
 +(Mot*) CYCLING; 
 +(Mot*) RUNNING; 
 +(Mot*) UNKNOWN; 


@end

 #define Mot_get_STATIONARY() [Mot STATIONARY] 
 #define Mot_get_WALKING() [Mot WALKING] 
 #define Mot_get_DRIVING() [Mot DRIVING] 
 #define Mot_get_BUS() [Mot BUS] 
 #define Mot_get_TRAIN() [Mot TRAIN] 
 #define Mot_get_CYCLING() [Mot CYCLING] 
 #define Mot_get_RUNNING() [Mot RUNNING] 
 #define Mot_get_UNKNOWN() [Mot UNKNOWN] 


#endif
