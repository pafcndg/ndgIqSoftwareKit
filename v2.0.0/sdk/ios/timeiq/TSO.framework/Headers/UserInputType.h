#ifndef __JavaBridge_UserInputType__
#define __JavaBridge_UserInputType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class UserInputType;

typedef NS_ENUM(NSInteger, eUserInputType)
{
 	UserInputType_WIDGET_ACTIVITY_STATIONARY , 
 	UserInputType_WIDGET_ACTIVITY_CYCLING , 
 	UserInputType_WIDGET_ACTIVITY_RUNNING , 
 	UserInputType_WIDGET_ACTIVITY_WALKING , 
 	UserInputType_WIDGET_ACTIVITY_ON_TRAIN , 
 	UserInputType_WIDGET_ACTIVITY_ON_BUS , 
 	UserInputType_WIDGET_ACTIVITY_ON_CAR , 
 	UserInputType_WIDGET_VISIT_START , 
 	UserInputType_WIDGET_VISIT_END , 
 	UserInputType_WIDGET_CLEAR_FEEDBACK_LIST , 
 	UserInputType_WIDGET_TEST_START , 
 	UserInputType_WIDGET_TEST_END , 

};

@interface UserInputType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (UserInputType *)valueOfWithNSString:(NSString*)name;
+ (UserInputType *)valueOf:(NSString*)name;
+ (UserInputType *)parseWithInt:(int)name;
+ (UserInputType *)fromIdWithInt:(int)name;
+ (UserInputType *)fromId:(int)val;

+ (void)load;
-(eUserInputType) ordinal;

 +(UserInputType*) WIDGET_ACTIVITY_STATIONARY; 
 +(UserInputType*) WIDGET_ACTIVITY_CYCLING; 
 +(UserInputType*) WIDGET_ACTIVITY_RUNNING; 
 +(UserInputType*) WIDGET_ACTIVITY_WALKING; 
 +(UserInputType*) WIDGET_ACTIVITY_ON_TRAIN; 
 +(UserInputType*) WIDGET_ACTIVITY_ON_BUS; 
 +(UserInputType*) WIDGET_ACTIVITY_ON_CAR; 
 +(UserInputType*) WIDGET_VISIT_START; 
 +(UserInputType*) WIDGET_VISIT_END; 
 +(UserInputType*) WIDGET_CLEAR_FEEDBACK_LIST; 
 +(UserInputType*) WIDGET_TEST_START; 
 +(UserInputType*) WIDGET_TEST_END; 


@end

 #define UserInputType_get_WIDGET_ACTIVITY_STATIONARY() [UserInputType WIDGET_ACTIVITY_STATIONARY] 
 #define UserInputType_get_WIDGET_ACTIVITY_CYCLING() [UserInputType WIDGET_ACTIVITY_CYCLING] 
 #define UserInputType_get_WIDGET_ACTIVITY_RUNNING() [UserInputType WIDGET_ACTIVITY_RUNNING] 
 #define UserInputType_get_WIDGET_ACTIVITY_WALKING() [UserInputType WIDGET_ACTIVITY_WALKING] 
 #define UserInputType_get_WIDGET_ACTIVITY_ON_TRAIN() [UserInputType WIDGET_ACTIVITY_ON_TRAIN] 
 #define UserInputType_get_WIDGET_ACTIVITY_ON_BUS() [UserInputType WIDGET_ACTIVITY_ON_BUS] 
 #define UserInputType_get_WIDGET_ACTIVITY_ON_CAR() [UserInputType WIDGET_ACTIVITY_ON_CAR] 
 #define UserInputType_get_WIDGET_VISIT_START() [UserInputType WIDGET_VISIT_START] 
 #define UserInputType_get_WIDGET_VISIT_END() [UserInputType WIDGET_VISIT_END] 
 #define UserInputType_get_WIDGET_CLEAR_FEEDBACK_LIST() [UserInputType WIDGET_CLEAR_FEEDBACK_LIST] 
 #define UserInputType_get_WIDGET_TEST_START() [UserInputType WIDGET_TEST_START] 
 #define UserInputType_get_WIDGET_TEST_END() [UserInputType WIDGET_TEST_END] 


#endif
