#ifndef __JavaBridge_MeetingResolverType__
#define __JavaBridge_MeetingResolverType__

#import "JB.h"
#import "JBEnum.h"
#import "NSArray+JavaAPI.h"

//@class MeetingResolverType;

typedef NS_ENUM(NSInteger, eMeetingResolverType)
{
 	MeetingResolverType_RESOLVE_MEETING_INITAL , 
 	MeetingResolverType_RESOLVE_MEETING_CHANGE_LOCATION_NAME , 
 	MeetingResolverType_RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SELECTED , 
 	MeetingResolverType_RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SEARCH_SELECTED , 
 	MeetingResolverType_RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SEARCH_BACK , 
 	MeetingResolverType_RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SELECTED , 
 	MeetingResolverType_RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SEARCH_SELECTED , 
 	MeetingResolverType_RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SEARCH_BACK , 

};

@interface MeetingResolverType : Enum

+ (Array*)values;
+ (Array*) getEnumConstants;
+ (MeetingResolverType *)valueOfWithNSString:(NSString*)name;
+ (MeetingResolverType *)valueOf:(NSString*)name;
+ (MeetingResolverType *)parseWithInt:(int)name;
+ (MeetingResolverType *)fromIdWithInt:(int)name;
+ (MeetingResolverType *)fromId:(int)val;

+ (void)load;
-(eMeetingResolverType) ordinal;

 +(MeetingResolverType*) RESOLVE_MEETING_INITAL; 
 +(MeetingResolverType*) RESOLVE_MEETING_CHANGE_LOCATION_NAME; 
 +(MeetingResolverType*) RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SELECTED; 
 +(MeetingResolverType*) RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SEARCH_SELECTED; 
 +(MeetingResolverType*) RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SEARCH_BACK; 
 +(MeetingResolverType*) RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SELECTED; 
 +(MeetingResolverType*) RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SEARCH_SELECTED; 
 +(MeetingResolverType*) RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SEARCH_BACK; 


@end

 #define MeetingResolverType_get_RESOLVE_MEETING_INITAL() [MeetingResolverType RESOLVE_MEETING_INITAL] 
 #define MeetingResolverType_get_RESOLVE_MEETING_CHANGE_LOCATION_NAME() [MeetingResolverType RESOLVE_MEETING_CHANGE_LOCATION_NAME] 
 #define MeetingResolverType_get_RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SELECTED() [MeetingResolverType RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SELECTED] 
 #define MeetingResolverType_get_RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SEARCH_SELECTED() [MeetingResolverType RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SEARCH_SELECTED] 
 #define MeetingResolverType_get_RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SEARCH_BACK() [MeetingResolverType RESOLVE_MEETING_WN_SINGLE_PLAUSIBLE_SEARCH_BACK] 
 #define MeetingResolverType_get_RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SELECTED() [MeetingResolverType RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SELECTED] 
 #define MeetingResolverType_get_RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SEARCH_SELECTED() [MeetingResolverType RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SEARCH_SELECTED] 
 #define MeetingResolverType_get_RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SEARCH_BACK() [MeetingResolverType RESOLVE_MEETING_WN_MULTIPLE_PLAUSIBLE_SEARCH_BACK] 


#endif
