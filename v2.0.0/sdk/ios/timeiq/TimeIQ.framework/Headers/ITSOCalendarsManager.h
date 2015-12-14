//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/common/calendar/ITSOCalendarsManager.java
//

#ifndef _ITSOCalendarsManager_H_
#define _ITSOCalendarsManager_H_

@class CalendarDetails;
@class TSOCalendarMeeting;
@protocol ICalendar;
@protocol ICalendarListener;
#import "JBHashSet.h"
#import "JBArrayList.h"

#import "JB.h"

/**
 @brief Created by eskoran on 17/06/2015.
 */
@protocol ITSOCalendarsManager < NSObject >

- (ArrayList*)getAllAvailableCalendars;

- (void)setAllAvailableCalendarsWithJavaUtilList:(ArrayList*)allAvailableCalendars;

- (void)setReadCalendarsWithJavaUtilCollection:(Set*)calendars;

- (Set*)getReadCalendars;

- (void)setWriteCalendarWithICalendar:(id<ICalendar>)calendar;

- (id<ICalendar>)getWriteCalendar;

- (CalendarDetails *)getCalendarDetailsWithNSString:(NSString *)calendarId;

- (void)registerForUpdatesWithICalendarListener:(id<ICalendarListener>)listener;

- (ArrayList*)getMeetingsWithLong:(long64)startTimeFrame
                               withLong:(long64)endTimeFrame;

- (ArrayList*)getMeetingsWithJavaUtilList:(ArrayList*)calendarsIds
                                       withLong:(long64)startTimeFrame
                                       withLong:(long64)endTimeFrame;

- (TSOCalendarMeeting *)getMeetingInstanceByIdWithNSString:(NSString *)meetingId;

- (TSOCalendarMeeting *)getMeetingInstanceByIdWithNSString:(NSString *)calendarId
                                              withNSString:(NSString *)meetingId;

- (NSString *)addMeetingWithTSOCalendarMeeting:(TSOCalendarMeeting *)meeting;

- (ArrayList*)addMeetingsWithJavaUtilList:(ArrayList*)meetings;

- (boolean)updateMeetingWithTSOCalendarMeeting:(TSOCalendarMeeting *)meeting;

- (boolean)deleteMeetingWithNSString:(NSString *)meetingId;

- (ArrayList*)deleteMeetingsWithJavaUtilList:(ArrayList*)meetings;

@end

__attribute__((always_inline)) inline void ITSOCalendarsManager_init() {}

#define ComIntelWearablePlatformTimeiqCommonCalendarITSOCalendarsManager ITSOCalendarsManager

#endif // _ITSOCalendarsManager_H_