#ifndef __JavaBridge_ShowAsType__
#define __JavaBridge_ShowAsType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ShowAsType;

typedef NS_ENUM(NSInteger, eShowAsType)
{
 	ShowAsType_BUSY , 
 	ShowAsType_AVAILABLE , 
 	ShowAsType_TENTATIVE , 
 	ShowAsType_OUT_OF_OFFICE , 

};

@interface ShowAsType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ShowAsType *)valueOfWithNSString:(NSString*)name;
+ (ShowAsType *)valueOf:(NSString*)name;
+ (ShowAsType *)parseWithInt:(int)name;
+ (ShowAsType *)fromIdWithInt:(int)name;
+ (ShowAsType *)fromId:(int)val;

+ (void)load;
-(eShowAsType) ordinal;

 +(ShowAsType*) BUSY; 
 +(ShowAsType*) AVAILABLE; 
 +(ShowAsType*) TENTATIVE; 
 +(ShowAsType*) OUT_OF_OFFICE; 


@end

 #define ShowAsType_get_BUSY() [ShowAsType BUSY] 
 #define ShowAsType_get_AVAILABLE() [ShowAsType AVAILABLE] 
 #define ShowAsType_get_TENTATIVE() [ShowAsType TENTATIVE] 
 #define ShowAsType_get_OUT_OF_OFFICE() [ShowAsType OUT_OF_OFFICE] 


#endif
