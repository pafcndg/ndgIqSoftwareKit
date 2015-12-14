#ifndef __JavaBridge_ChargeMethod__
#define __JavaBridge_ChargeMethod__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class ChargeMethod;

typedef NS_ENUM(NSInteger, eChargeMethod)
{
 	ChargeMethod_AC , 
 	ChargeMethod_USB , 
 	ChargeMethod_NA , 

};

@interface ChargeMethod : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ChargeMethod *)valueOfWithNSString:(NSString*)name;
+ (ChargeMethod *)valueOf:(NSString*)name;
+ (ChargeMethod *)parseWithInt:(int)name;
+ (ChargeMethod *)fromIdWithInt:(int)name;
+ (ChargeMethod *)fromId:(int)val;

+ (void)load;
-(eChargeMethod) ordinal;

 +(ChargeMethod*) AC; 
 +(ChargeMethod*) USB; 
 +(ChargeMethod*) NA; 


@end

 #define ChargeMethod_get_AC() [ChargeMethod AC] 
 #define ChargeMethod_get_USB() [ChargeMethod USB] 
 #define ChargeMethod_get_NA() [ChargeMethod NA] 


#endif
