#ifndef __JavaBridge_RetCode__
#define __JavaBridge_RetCode__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class RetCode;

typedef NS_ENUM(NSInteger, eRetCode)
{
 	RetCode_SUCCESS , 
 	RetCode_FAILED , 
 	RetCode_RETCODE_NA , 
 	RetCode_ERROR , 
 	RetCode_EMPTY_REQUEST , 
 	RetCode_ALREADY_EXISTS , 
 	RetCode_NO_SUCH_ELEMENT , 
 	RetCode_OBJECTS_LIST_EMPTY , 
 	RetCode_MALFORMED_JSON , 
 	RetCode_NOT_MASTER , 
 	RetCode_NOT_SLAVE , 
 	RetCode_NONE_SYNCED , 
 	RetCode_FULLY_SYNCED , 
 	RetCode_PARTIALLY_SYNCED , 
 	RetCode_PENDING_SYNC , 

};

@interface RetCode : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (RetCode *)valueOfWithNSString:(NSString*)name;
+ (RetCode *)valueOf:(NSString*)name;
+ (RetCode *)parseWithInt:(int)name;
+ (RetCode *)fromIdWithInt:(int)name;
+ (RetCode *)fromId:(int)val;

+ (void)load;
-(eRetCode) ordinal;

 +(RetCode*) SUCCESS; 
 +(RetCode*) FAILED; 
 +(RetCode*) RETCODE_NA; 
 +(RetCode*) ERROR; 
 +(RetCode*) EMPTY_REQUEST; 
 +(RetCode*) ALREADY_EXISTS; 
 +(RetCode*) NO_SUCH_ELEMENT; 
 +(RetCode*) OBJECTS_LIST_EMPTY; 
 +(RetCode*) MALFORMED_JSON; 
 +(RetCode*) NOT_MASTER; 
 +(RetCode*) NOT_SLAVE; 
 +(RetCode*) NONE_SYNCED; 
 +(RetCode*) FULLY_SYNCED; 
 +(RetCode*) PARTIALLY_SYNCED; 
 +(RetCode*) PENDING_SYNC; 


@end

 #define RetCode_get_SUCCESS() [RetCode SUCCESS] 
 #define RetCode_get_FAILED() [RetCode FAILED] 
 #define RetCode_get_RETCODE_NA() [RetCode RETCODE_NA] 
 #define RetCode_get_ERROR() [RetCode ERROR] 
 #define RetCode_get_EMPTY_REQUEST() [RetCode EMPTY_REQUEST] 
 #define RetCode_get_ALREADY_EXISTS() [RetCode ALREADY_EXISTS] 
 #define RetCode_get_NO_SUCH_ELEMENT() [RetCode NO_SUCH_ELEMENT] 
 #define RetCode_get_OBJECTS_LIST_EMPTY() [RetCode OBJECTS_LIST_EMPTY] 
 #define RetCode_get_MALFORMED_JSON() [RetCode MALFORMED_JSON] 
 #define RetCode_get_NOT_MASTER() [RetCode NOT_MASTER] 
 #define RetCode_get_NOT_SLAVE() [RetCode NOT_SLAVE] 
 #define RetCode_get_NONE_SYNCED() [RetCode NONE_SYNCED] 
 #define RetCode_get_FULLY_SYNCED() [RetCode FULLY_SYNCED] 
 #define RetCode_get_PARTIALLY_SYNCED() [RetCode PARTIALLY_SYNCED] 
 #define RetCode_get_PENDING_SYNC() [RetCode PENDING_SYNC] 


#endif
