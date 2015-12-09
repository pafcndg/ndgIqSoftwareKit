#ifndef __JavaBridge_VipStateType__
#define __JavaBridge_VipStateType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class VipStateType;

typedef NS_ENUM(NSInteger, eVipStateType)
{
 	VipStateType_Unkown , 
 	VipStateType_StartSuspend , 
 	VipStateType_Start , 
 	VipStateType_InProgress , 
 	VipStateType_EndSuspend , 
 	VipStateType_End , 

};

@interface VipStateType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (VipStateType *)valueOfWithNSString:(NSString*)name;
+ (VipStateType *)valueOf:(NSString*)name;
+ (VipStateType *)parseWithInt:(int)name;
+ (VipStateType *)fromIdWithInt:(int)name;
+ (VipStateType *)fromId:(int)val;

+ (void)load;
-(eVipStateType) ordinal;

 +(VipStateType*) Unkown; 
 +(VipStateType*) StartSuspend; 
 +(VipStateType*) Start; 
 +(VipStateType*) InProgress; 
 +(VipStateType*) EndSuspend; 
 +(VipStateType*) End; 


@end

 #define VipStateType_get_Unkown() [VipStateType Unkown] 
 #define VipStateType_get_StartSuspend() [VipStateType StartSuspend] 
 #define VipStateType_get_Start() [VipStateType Start] 
 #define VipStateType_get_InProgress() [VipStateType InProgress] 
 #define VipStateType_get_EndSuspend() [VipStateType EndSuspend] 
 #define VipStateType_get_End() [VipStateType End] 


#endif
