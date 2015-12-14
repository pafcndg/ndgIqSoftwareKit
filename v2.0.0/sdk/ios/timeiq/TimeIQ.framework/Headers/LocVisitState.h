#ifndef __JavaBridge_LocVisitState__
#define __JavaBridge_LocVisitState__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class LocVisitState;

typedef NS_ENUM(NSInteger, eLocVisitState)
{
 	LocVisitState_Unknown , 
 	LocVisitState_VisitStart , 
 	LocVisitState_VisitOngoing , 
 	LocVisitState_VisitEnd , 

};

@interface LocVisitState : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (LocVisitState *)valueOfWithNSString:(NSString*)name;
+ (LocVisitState *)valueOf:(NSString*)name;
+ (LocVisitState *)parseWithInt:(int)name;
+ (LocVisitState *)fromIdWithInt:(int)name;
+ (LocVisitState *)fromId:(int)val;

+ (void)load;
-(eLocVisitState) ordinal;

 +(LocVisitState*) Unknown; 
 +(LocVisitState*) VisitStart; 
 +(LocVisitState*) VisitOngoing; 
 +(LocVisitState*) VisitEnd; 


@end

 #define LocVisitState_get_Unknown() [LocVisitState Unknown] 
 #define LocVisitState_get_VisitStart() [LocVisitState VisitStart] 
 #define LocVisitState_get_VisitOngoing() [LocVisitState VisitOngoing] 
 #define LocVisitState_get_VisitEnd() [LocVisitState VisitEnd] 


#endif
