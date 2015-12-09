#ifndef __JavaBridge_SnoozeTimeRange__
#define __JavaBridge_SnoozeTimeRange__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class SnoozeTimeRange;

typedef NS_ENUM(NSInteger, eSnoozeTimeRange)
{
 	SnoozeTimeRange_THIS_MORNING , 
 	SnoozeTimeRange_TODAY , 
 	SnoozeTimeRange_THIS_EVENING , 
 	SnoozeTimeRange_THIS_NIGHT , 
 	SnoozeTimeRange_TOMORROW_MORNING , 

};

@interface SnoozeTimeRange : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (SnoozeTimeRange *)valueOfWithNSString:(NSString*)name;
+ (SnoozeTimeRange *)valueOf:(NSString*)name;
+ (SnoozeTimeRange *)parseWithInt:(int)name;
+ (SnoozeTimeRange *)fromIdWithInt:(int)name;
+ (SnoozeTimeRange *)fromId:(int)val;

+ (void)load;
-(eSnoozeTimeRange) ordinal;

 +(SnoozeTimeRange*) THIS_MORNING; 
 +(SnoozeTimeRange*) TODAY; 
 +(SnoozeTimeRange*) THIS_EVENING; 
 +(SnoozeTimeRange*) THIS_NIGHT; 
 +(SnoozeTimeRange*) TOMORROW_MORNING; 


@end

 #define SnoozeTimeRange_get_THIS_MORNING() [SnoozeTimeRange THIS_MORNING] 
 #define SnoozeTimeRange_get_TODAY() [SnoozeTimeRange TODAY] 
 #define SnoozeTimeRange_get_THIS_EVENING() [SnoozeTimeRange THIS_EVENING] 
 #define SnoozeTimeRange_get_THIS_NIGHT() [SnoozeTimeRange THIS_NIGHT] 
 #define SnoozeTimeRange_get_TOMORROW_MORNING() [SnoozeTimeRange TOMORROW_MORNING] 


#endif
