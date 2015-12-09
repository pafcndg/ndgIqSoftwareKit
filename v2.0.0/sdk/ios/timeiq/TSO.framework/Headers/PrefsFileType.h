#ifndef __JavaBridge_PrefsFileType__
#define __JavaBridge_PrefsFileType__

#import "JB.h"
#import "JBEnum.h"
#include "NSArray+JavaAPI.h"

//@class PrefsFileType;

typedef NS_ENUM(NSInteger, ePrefsFileType)
{
 	PrefsFileType_BuildPrefs , 
 	PrefsFileType_SdkDefaultPrefs , 

};

@interface PrefsFileType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (PrefsFileType *)valueOfWithNSString:(NSString*)name;
+ (PrefsFileType *)valueOf:(NSString*)name;
+ (PrefsFileType *)parseWithInt:(int)name;
+ (PrefsFileType *)fromIdWithInt:(int)name;
+ (PrefsFileType *)fromId:(int)val;

+ (void)load;
-(ePrefsFileType) ordinal;

 +(PrefsFileType*) BuildPrefs; 
 +(PrefsFileType*) SdkDefaultPrefs; 


@end

 #define PrefsFileType_get_BuildPrefs() [PrefsFileType BuildPrefs] 
 #define PrefsFileType_get_SdkDefaultPrefs() [PrefsFileType SdkDefaultPrefs] 


#endif
