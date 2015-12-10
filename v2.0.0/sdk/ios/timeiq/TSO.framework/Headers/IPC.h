#ifndef __JavaBridge_IPC__
#define __JavaBridge_IPC__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class IPC;

typedef NS_ENUM(NSInteger, eIPC)
{
 	IPC_Far , 
 	IPC_Low , 
 	IPC_Medium , 
 	IPC_High , 

};

@interface IPC : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (IPC *)valueOfWithNSString:(NSString*)name;
+ (IPC *)valueOf:(NSString*)name;
+ (IPC *)parseWithInt:(int)name;
+ (IPC *)fromIdWithInt:(int)name;
+ (IPC *)fromId:(int)val;

+ (void)load;
-(eIPC) ordinal;

 +(IPC*) Far; 
 +(IPC*) Low; 
 +(IPC*) Medium; 
 +(IPC*) High; 


@end

 #define IPC_get_Far() [IPC Far] 
 #define IPC_get_Low() [IPC Low] 
 #define IPC_get_Medium() [IPC Medium] 
 #define IPC_get_High() [IPC High] 


#endif
