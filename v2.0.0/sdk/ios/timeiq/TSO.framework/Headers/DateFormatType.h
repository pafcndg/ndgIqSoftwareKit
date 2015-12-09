#ifndef __JavaBridge_DateFormatType__
#define __JavaBridge_DateFormatType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class DateFormatType;

typedef NS_ENUM(NSInteger, eDateFormatType)
{
 	DateFormatType_DATE , 
 	DateFormatType_TIME , 
 	DateFormatType_FULL , 
 	DateFormatType_TRIMMED , 
 	DateFormatType_TIME_SHORT , 
 	DateFormatType_DATE_SHORT , 

};

@interface DateFormatType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (DateFormatType *)valueOfWithNSString:(NSString*)name;
+ (DateFormatType *)valueOf:(NSString*)name;
+ (DateFormatType *)parseWithInt:(int)name;
+ (DateFormatType *)fromIdWithInt:(int)name;
+ (DateFormatType *)fromId:(int)val;

+ (void)load;
-(eDateFormatType) ordinal;

 +(DateFormatType*) DATE; 
 +(DateFormatType*) TIME; 
 +(DateFormatType*) FULL; 
 +(DateFormatType*) TRIMMED; 
 +(DateFormatType*) TIME_SHORT; 
 +(DateFormatType*) DATE_SHORT; 


@end

 #define DateFormatType_get_DATE() [DateFormatType DATE] 
 #define DateFormatType_get_TIME() [DateFormatType TIME] 
 #define DateFormatType_get_FULL() [DateFormatType FULL] 
 #define DateFormatType_get_TRIMMED() [DateFormatType TRIMMED] 
 #define DateFormatType_get_TIME_SHORT() [DateFormatType TIME_SHORT] 
 #define DateFormatType_get_DATE_SHORT() [DateFormatType DATE_SHORT] 


#endif
