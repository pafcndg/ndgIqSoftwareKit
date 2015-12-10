#ifndef __JavaBridge_State__
#define __JavaBridge_State__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class State;

typedef NS_ENUM(NSInteger, eState)
{
 	State_START , 
 	State_END , 

};

@interface State : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (State *)valueOfWithNSString:(NSString*)name;
+ (State *)valueOf:(NSString*)name;
+ (State *)parseWithInt:(int)name;
+ (State *)fromIdWithInt:(int)name;
+ (State *)fromId:(int)val;

+ (void)load;
-(eState) ordinal;

 +(State*) START; 
 +(State*) END; 


@end

 #define State_get_START() [State START] 
 #define State_get_END() [State END] 


#endif
