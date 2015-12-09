#ifndef __JavaBridge_ManualPlaceSource__
#define __JavaBridge_ManualPlaceSource__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ManualPlaceSource;

typedef NS_ENUM(NSInteger, eManualPlaceSource)
{
 	ManualPlaceSource_USER , 
 	ManualPlaceSource_CALENDAR , 
 	ManualPlaceSource_RECENT , 

};

@interface ManualPlaceSource : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ManualPlaceSource *)valueOfWithNSString:(NSString*)name;
+ (ManualPlaceSource *)valueOf:(NSString*)name;
+ (ManualPlaceSource *)parseWithInt:(int)name;
+ (ManualPlaceSource *)fromIdWithInt:(int)name;
+ (ManualPlaceSource *)fromId:(int)val;

+ (void)load;
-(eManualPlaceSource) ordinal;

 +(ManualPlaceSource*) USER; 
 +(ManualPlaceSource*) CALENDAR; 
 +(ManualPlaceSource*) RECENT; 


@end

 #define ManualPlaceSource_get_USER() [ManualPlaceSource USER] 
 #define ManualPlaceSource_get_CALENDAR() [ManualPlaceSource CALENDAR] 
 #define ManualPlaceSource_get_RECENT() [ManualPlaceSource RECENT] 


#endif
