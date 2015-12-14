#ifndef __JavaBridge_InnerMessageType__
#define __JavaBridge_InnerMessageType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class InnerMessageType;

typedef NS_ENUM(NSInteger, eInnerMessageType)
{
 	InnerMessageType_ADD_PLACE , 

};

@interface InnerMessageType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (InnerMessageType *)valueOfWithNSString:(NSString*)name;
+ (InnerMessageType *)valueOf:(NSString*)name;
+ (InnerMessageType *)parseWithInt:(int)name;
+ (InnerMessageType *)fromIdWithInt:(int)name;
+ (InnerMessageType *)fromId:(int)val;

+ (void)load;
-(eInnerMessageType) ordinal;

 +(InnerMessageType*) ADD_PLACE; 


@end

 #define InnerMessageType_get_ADD_PLACE() [InnerMessageType ADD_PLACE] 


#endif
