#ifndef __JavaBridge_PrivacyType__
#define __JavaBridge_PrivacyType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class PrivacyType;

typedef NS_ENUM(NSInteger, ePrivacyType)
{
 	PrivacyType_PRIVATE , 
 	PrivacyType_CONFIDENTIAL , 
 	PrivacyType_PUBLIC , 
 	PrivacyType_DEFAULT , 

};

@interface PrivacyType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (PrivacyType *)valueOfWithNSString:(NSString*)name;
+ (PrivacyType *)valueOf:(NSString*)name;
+ (PrivacyType *)parseWithInt:(int)name;
+ (PrivacyType *)fromIdWithInt:(int)name;
+ (PrivacyType *)fromId:(int)val;

+ (void)load;
-(ePrivacyType) ordinal;

 +(PrivacyType*) PRIVATE; 
 +(PrivacyType*) CONFIDENTIAL; 
 +(PrivacyType*) PUBLIC; 
 +(PrivacyType*) DEFAULT; 


@end

 #define PrivacyType_get_PRIVATE() [PrivacyType PRIVATE] 
 #define PrivacyType_get_CONFIDENTIAL() [PrivacyType CONFIDENTIAL] 
 #define PrivacyType_get_PUBLIC() [PrivacyType PUBLIC] 
 #define PrivacyType_get_DEFAULT() [PrivacyType DEFAULT] 


#endif
