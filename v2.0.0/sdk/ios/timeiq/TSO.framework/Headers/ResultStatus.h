#ifndef __JavaBridge_ResultStatus__
#define __JavaBridge_ResultStatus__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class ResultStatus;

typedef NS_ENUM(NSInteger, eResultStatus)
{
 	ResultStatus_OK , 
 	ResultStatus_REQUEST_FILTERED , 
 	ResultStatus_ERR_REQUEST_ID_EMPTY , 
 	ResultStatus_ERR_USER_ID_EMPTY , 
 	ResultStatus_ERR_FLOW_TYPE_EMPTY , 
 	ResultStatus_ERR_CALEDAR_FIELD_EMPTY , 
 	ResultStatus_ERR_LOCATION_EMPTY , 
 	ResultStatus_ERR_EXPRESSION_DIFFER_FROM_LOCATION_FIELD , 
 	ResultStatus_ERR_UNSUPPORTED_FLOW_TYPE , 
 	ResultStatus_ERR_SERVER_ERROR , 

};

@interface ResultStatus : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (ResultStatus *)valueOfWithNSString:(NSString*)name;
+ (ResultStatus *)valueOf:(NSString*)name;
+ (ResultStatus *)parseWithInt:(int)name;
+ (ResultStatus *)fromIdWithInt:(int)name;
+ (ResultStatus *)fromId:(int)val;

+ (void)load;
-(eResultStatus) ordinal;

 +(ResultStatus*) OK; 
 +(ResultStatus*) REQUEST_FILTERED; 
 +(ResultStatus*) ERR_REQUEST_ID_EMPTY; 
 +(ResultStatus*) ERR_USER_ID_EMPTY; 
 +(ResultStatus*) ERR_FLOW_TYPE_EMPTY; 
 +(ResultStatus*) ERR_CALEDAR_FIELD_EMPTY; 
 +(ResultStatus*) ERR_LOCATION_EMPTY; 
 +(ResultStatus*) ERR_EXPRESSION_DIFFER_FROM_LOCATION_FIELD; 
 +(ResultStatus*) ERR_UNSUPPORTED_FLOW_TYPE; 
 +(ResultStatus*) ERR_SERVER_ERROR; 


@end

 #define ResultStatus_get_OK() [ResultStatus OK] 
 #define ResultStatus_get_REQUEST_FILTERED() [ResultStatus REQUEST_FILTERED] 
 #define ResultStatus_get_ERR_REQUEST_ID_EMPTY() [ResultStatus ERR_REQUEST_ID_EMPTY] 
 #define ResultStatus_get_ERR_USER_ID_EMPTY() [ResultStatus ERR_USER_ID_EMPTY] 
 #define ResultStatus_get_ERR_FLOW_TYPE_EMPTY() [ResultStatus ERR_FLOW_TYPE_EMPTY] 
 #define ResultStatus_get_ERR_CALEDAR_FIELD_EMPTY() [ResultStatus ERR_CALEDAR_FIELD_EMPTY] 
 #define ResultStatus_get_ERR_LOCATION_EMPTY() [ResultStatus ERR_LOCATION_EMPTY] 
 #define ResultStatus_get_ERR_EXPRESSION_DIFFER_FROM_LOCATION_FIELD() [ResultStatus ERR_EXPRESSION_DIFFER_FROM_LOCATION_FIELD] 
 #define ResultStatus_get_ERR_UNSUPPORTED_FLOW_TYPE() [ResultStatus ERR_UNSUPPORTED_FLOW_TYPE] 
 #define ResultStatus_get_ERR_SERVER_ERROR() [ResultStatus ERR_SERVER_ERROR] 


#endif
