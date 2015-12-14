#ifndef __JavaBridge_NotifyType__
#define __JavaBridge_NotifyType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class NotifyType;

typedef NS_ENUM(NSInteger, eNotifyType)
{
 	NotifyType_None , 
 	NotifyType_ForceNotify , 
 	NotifyType_TimerTimeout , 

};

@interface NotifyType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (NotifyType *)valueOfWithNSString:(NSString*)name;
+ (NotifyType *)valueOf:(NSString*)name;
+ (NotifyType *)parseWithInt:(int)name;
+ (NotifyType *)fromIdWithInt:(int)name;
+ (NotifyType *)fromId:(int)val;

+ (void)load;
-(eNotifyType) ordinal;

 +(NotifyType*) None; 
 +(NotifyType*) ForceNotify; 
 +(NotifyType*) TimerTimeout; 


@end

 #define NotifyType_get_None() [NotifyType None] 
 #define NotifyType_get_ForceNotify() [NotifyType ForceNotify] 
 #define NotifyType_get_TimerTimeout() [NotifyType TimerTimeout] 


#endif
