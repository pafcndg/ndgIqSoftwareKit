#ifndef __JavaBridge_DataBaseControl__
#define __JavaBridge_DataBaseControl__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class DataBaseControl;

typedef NS_ENUM(NSInteger, eDataBaseControl)
{
 	DataBaseControl_MASTER , 
 	DataBaseControl_SLAVE , 
 	DataBaseControl_NA , 

};

@interface DataBaseControl : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (DataBaseControl *)valueOfWithNSString:(NSString*)name;
+ (DataBaseControl *)valueOf:(NSString*)name;
+ (DataBaseControl *)parseWithInt:(int)name;
+ (DataBaseControl *)fromIdWithInt:(int)name;
+ (DataBaseControl *)fromId:(int)val;

+ (void)load;
-(eDataBaseControl) ordinal;

 +(DataBaseControl*) MASTER; 
 +(DataBaseControl*) SLAVE; 
 +(DataBaseControl*) NA; 


@end

 #define DataBaseControl_get_MASTER() [DataBaseControl MASTER] 
 #define DataBaseControl_get_SLAVE() [DataBaseControl SLAVE] 
 #define DataBaseControl_get_NA() [DataBaseControl NA] 


#endif
