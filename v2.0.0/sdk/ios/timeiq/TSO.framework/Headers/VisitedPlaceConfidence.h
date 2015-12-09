#ifndef __JavaBridge_VisitedPlaceConfidence__
#define __JavaBridge_VisitedPlaceConfidence__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class VisitedPlaceConfidence;

typedef NS_ENUM(NSInteger, eVisitedPlaceConfidence)
{
 	VisitedPlaceConfidence_Low , 
 	VisitedPlaceConfidence_Medium , 
 	VisitedPlaceConfidence_High , 

};

@interface VisitedPlaceConfidence : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (VisitedPlaceConfidence *)valueOfWithNSString:(NSString*)name;
+ (VisitedPlaceConfidence *)valueOf:(NSString*)name;
+ (VisitedPlaceConfidence *)parseWithInt:(int)name;
+ (VisitedPlaceConfidence *)fromIdWithInt:(int)name;
+ (VisitedPlaceConfidence *)fromId:(int)val;

+ (void)load;
-(eVisitedPlaceConfidence) ordinal;

 +(VisitedPlaceConfidence*) Low; 
 +(VisitedPlaceConfidence*) Medium; 
 +(VisitedPlaceConfidence*) High; 


@end

 #define VisitedPlaceConfidence_get_Low() [VisitedPlaceConfidence Low] 
 #define VisitedPlaceConfidence_get_Medium() [VisitedPlaceConfidence Medium] 
 #define VisitedPlaceConfidence_get_High() [VisitedPlaceConfidence High] 


#endif
