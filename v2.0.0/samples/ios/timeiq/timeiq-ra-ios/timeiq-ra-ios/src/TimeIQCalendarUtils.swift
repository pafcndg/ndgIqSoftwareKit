//
//  TimeIQCalendarUtils.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 01/12/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import TSO

class TimeIQCalendarUtils {
    
    /**
     * Gets the available calendars on the user's device, these calendars can be used as readable or writable calendars.
     * @return List of available calendars and their details {@link CalendarDetails} upon success or a corresponding error upon failure.
     */
    class func getAvailableCalendars() -> ArrayList? {
        let calendarsDetailsProvider = TimeIQBGService.sharedInstance.timeIQApi!.getCalendarsDetailsProvider();
        let availableCalendarsResultData = calendarsDetailsProvider.getAvailableCalendars();
        return availableCalendarsResultData.getData() as? ArrayList;
    }
    
    /**
     * Sets the defined list of calendars the SDK is able to read from on the user's device
     * @param calendarIds - A List of string values representing the read calendar ids.
     */
    class func setReadCalendars(calendarIds: ArrayList) {
        let calendarsDetailsProvider = TimeIQBGService.sharedInstance.timeIQApi!.getCalendarsDetailsProvider();
        calendarsDetailsProvider.setReadCalendarsWithJavaUtilList(calendarIds);
    }
    
    /**
     * Gets the defined read calendars for the SDK
     * @return ResultObject<List<CalendarDetails>> The list of the defined read calendar details
     * {@link CalendarDetails} from which the SDK can read events and other relevant information
     */
    class func getReadCalendars() -> ArrayList? {
        let calendarsDetailsProvider = TimeIQBGService.sharedInstance.timeIQApi!.getCalendarsDetailsProvider();
        let readCalendarsResultData = calendarsDetailsProvider.getReadCalendars();
        print(readCalendarsResultData.getMessage())
        return readCalendarsResultData.getData() as? ArrayList;
    }
    
    /**
     * Sets the write calendar id as defined above.
     * @param calendarId - String that represents the calendar id as defined on the device
     */
    class func setWriteCalendar(calendarId: String) {
        let calendarsDetailsProvider = TimeIQBGService.sharedInstance.timeIQApi!.getCalendarsDetailsProvider();
        let result = calendarsDetailsProvider.setWriteCalendarWithNSString(calendarId);
        print(result.getMessage())
    }
    
    /**
     * Gets the defined write calendar - the calendar to which the SDK can write events and other
     * relevant information.
     * @return ResultObject<CalendarDetails> The write calendar details {@link CalendarDetails}
     * if it was defined or a corresponding error code.
     */
    class func getWriteCalendar() -> CalendarDetails? {
        let calendarsDetailsProvider = TimeIQBGService.sharedInstance.timeIQApi!.getCalendarsDetailsProvider();
        let writeCalendarResultData = calendarsDetailsProvider.getWriteCalendar();
        return writeCalendarResultData.getData() as? CalendarDetails;
    }
    
    /**
     * Is there a read calendar defined
     * @return true if there's at least one read calendar defined or false otherwise
     */
    class func isThereAnyReadableCalendarsSet() -> Bool {
        let selectedCalendars = TimeIQCalendarUtils.getReadCalendars();
        return selectedCalendars != nil;
    }
}
