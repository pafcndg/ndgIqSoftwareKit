#ifndef __JavaBridge_DBOperation__
#define __JavaBridge_DBOperation__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class DBOperation;

typedef NS_ENUM(NSInteger, eDBOperation)
{
 	DBOperation_CREATE , 
 	DBOperation_READ , 
 	DBOperation_UPDATE , 
 	DBOperation_DELETE , 
 	DBOperation_DELETE_ALL , 

};

@interface DBOperation : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (DBOperation *)valueOfWithNSString:(NSString*)name;
+ (DBOperation *)valueOf:(NSString*)name;
+ (DBOperation *)parseWithInt:(int)name;
+ (DBOperation *)fromIdWithInt:(int)name;
+ (DBOperation *)fromId:(int)val;

+ (void)load;
-(eDBOperation) ordinal;

 +(DBOperation*) CREATE; 
 +(DBOperation*) READ; 
 +(DBOperation*) UPDATE; 
 +(DBOperation*) DELETE; 
 +(DBOperation*) DELETE_ALL; 


@end

 #define DBOperation_get_CREATE() [DBOperation CREATE] 
 #define DBOperation_get_READ() [DBOperation READ] 
 #define DBOperation_get_UPDATE() [DBOperation UPDATE] 
 #define DBOperation_get_DELETE() [DBOperation DELETE] 
 #define DBOperation_get_DELETE_ALL() [DBOperation DELETE_ALL] 


#endif
