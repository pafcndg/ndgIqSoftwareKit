#ifndef __JavaBridge_eDayOfReminder__
#define __JavaBridge_eDayOfReminder__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class eDayOfReminder;

typedef NS_ENUM(NSInteger, eeDayOfReminder)
{
 	eDayOfReminder_SAME_DAY , 
 	eDayOfReminder_DAY_BEFORE , 

};

@interface eDayOfReminder : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (eDayOfReminder *)valueOfWithNSString:(NSString*)name;
+ (eDayOfReminder *)valueOf:(NSString*)name;
+ (eDayOfReminder *)parseWithInt:(int)name;
+ (eDayOfReminder *)fromIdWithInt:(int)name;
+ (eDayOfReminder *)fromId:(int)val;

+ (void)load;
-(eeDayOfReminder) ordinal;

 +(eDayOfReminder*) SAME_DAY; 
 +(eDayOfReminder*) DAY_BEFORE; 


@end

 #define eDayOfReminder_get_SAME_DAY() [eDayOfReminder SAME_DAY] 
 #define eDayOfReminder_get_DAY_BEFORE() [eDayOfReminder DAY_BEFORE] 


#endif
