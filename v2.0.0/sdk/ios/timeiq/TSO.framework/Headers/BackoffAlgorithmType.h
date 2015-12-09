#ifndef __JavaBridge_BackoffAlgorithmType__
#define __JavaBridge_BackoffAlgorithmType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class BackoffAlgorithmType;

typedef NS_ENUM(NSInteger, eBackoffAlgorithmType)
{
 	BackoffAlgorithmType_TruncatedExponentialBackoff , 

};

@interface BackoffAlgorithmType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (BackoffAlgorithmType *)valueOfWithNSString:(NSString*)name;
+ (BackoffAlgorithmType *)valueOf:(NSString*)name;
+ (BackoffAlgorithmType *)parseWithInt:(int)name;
+ (BackoffAlgorithmType *)fromIdWithInt:(int)name;
+ (BackoffAlgorithmType *)fromId:(int)val;

+ (void)load;
-(eBackoffAlgorithmType) ordinal;

 +(BackoffAlgorithmType*) TruncatedExponentialBackoff; 


@end

 #define BackoffAlgorithmType_get_TruncatedExponentialBackoff() [BackoffAlgorithmType TruncatedExponentialBackoff] 


#endif
