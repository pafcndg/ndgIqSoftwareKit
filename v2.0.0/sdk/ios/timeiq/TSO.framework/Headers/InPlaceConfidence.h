#ifndef __JavaBridge_InPlaceConfidence__
#define __JavaBridge_InPlaceConfidence__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class InPlaceConfidence;

typedef NS_ENUM(NSInteger, eInPlaceConfidence)
{
 	InPlaceConfidence_Far , 
 	InPlaceConfidence_Low , 
 	InPlaceConfidence_Medium , 
 	InPlaceConfidence_High , 

};

@interface InPlaceConfidence : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (InPlaceConfidence *)valueOfWithNSString:(NSString*)name;
+ (InPlaceConfidence *)valueOf:(NSString*)name;
+ (InPlaceConfidence *)parseWithInt:(int)name;
+ (InPlaceConfidence *)fromIdWithInt:(int)name;
+ (InPlaceConfidence *)fromId:(int)val;

+ (void)load;
-(eInPlaceConfidence) ordinal;

 +(InPlaceConfidence*) Far; 
 +(InPlaceConfidence*) Low; 
 +(InPlaceConfidence*) Medium; 
 +(InPlaceConfidence*) High; 


@end

 #define InPlaceConfidence_get_Far() [InPlaceConfidence Far] 
 #define InPlaceConfidence_get_Low() [InPlaceConfidence Low] 
 #define InPlaceConfidence_get_Medium() [InPlaceConfidence Medium] 
 #define InPlaceConfidence_get_High() [InPlaceConfidence High] 


#endif
