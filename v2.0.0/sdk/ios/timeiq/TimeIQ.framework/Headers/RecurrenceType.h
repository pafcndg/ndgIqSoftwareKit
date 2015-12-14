#ifndef __JavaBridge_RecurrenceType__
#define __JavaBridge_RecurrenceType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class RecurrenceType;

typedef NS_ENUM(NSInteger, eRecurrenceType)
{
 	RecurrenceType_DAILY , 
 	RecurrenceType_WEEKLY , 
 	RecurrenceType_MONTHLY , 
 	RecurrenceType_YEARLY , 

};

@interface RecurrenceType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (RecurrenceType *)valueOfWithNSString:(NSString*)name;
+ (RecurrenceType *)valueOf:(NSString*)name;
+ (RecurrenceType *)parseWithInt:(int)name;
+ (RecurrenceType *)fromIdWithInt:(int)name;
+ (RecurrenceType *)fromId:(int)val;

+ (void)load;
-(eRecurrenceType) ordinal;

 +(RecurrenceType*) DAILY; 
 +(RecurrenceType*) WEEKLY; 
 +(RecurrenceType*) MONTHLY; 
 +(RecurrenceType*) YEARLY; 


@end

 #define RecurrenceType_get_DAILY() [RecurrenceType DAILY] 
 #define RecurrenceType_get_WEEKLY() [RecurrenceType WEEKLY] 
 #define RecurrenceType_get_MONTHLY() [RecurrenceType MONTHLY] 
 #define RecurrenceType_get_YEARLY() [RecurrenceType YEARLY] 


#endif
