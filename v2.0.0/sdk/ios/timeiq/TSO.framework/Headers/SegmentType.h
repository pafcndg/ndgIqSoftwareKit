#ifndef __JavaBridge_SegmentType__
#define __JavaBridge_SegmentType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class SegmentType;

typedef NS_ENUM(NSInteger, eSegmentType)
{
 	SegmentType_TimeToTTL , 
 	SegmentType_OriginInDoor , 
 	SegmentType_TimeToCar , 
 	SegmentType_Travel , 
 	SegmentType_Park , 
 	SegmentType_DestinationInDoor , 

};

@interface SegmentType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (SegmentType *)valueOfWithNSString:(NSString*)name;
+ (SegmentType *)valueOf:(NSString*)name;
+ (SegmentType *)parseWithInt:(int)name;
+ (SegmentType *)fromIdWithInt:(int)name;
+ (SegmentType *)fromId:(int)val;

+ (void)load;
-(eSegmentType) ordinal;

 +(SegmentType*) TimeToTTL; 
 +(SegmentType*) OriginInDoor; 
 +(SegmentType*) TimeToCar; 
 +(SegmentType*) Travel; 
 +(SegmentType*) Park; 
 +(SegmentType*) DestinationInDoor; 


@end

 #define SegmentType_get_TimeToTTL() [SegmentType TimeToTTL] 
 #define SegmentType_get_OriginInDoor() [SegmentType OriginInDoor] 
 #define SegmentType_get_TimeToCar() [SegmentType TimeToCar] 
 #define SegmentType_get_Travel() [SegmentType Travel] 
 #define SegmentType_get_Park() [SegmentType Park] 
 #define SegmentType_get_DestinationInDoor() [SegmentType DestinationInDoor] 


#endif
