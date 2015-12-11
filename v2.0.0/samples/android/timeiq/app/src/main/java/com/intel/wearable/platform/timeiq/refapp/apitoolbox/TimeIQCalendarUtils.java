package com.intel.wearable.platform.timeiq.refapp.apitoolbox;

import com.intel.wearable.platform.timeiq.api.common.calendar.CalendarDetails;
import com.intel.wearable.platform.timeiq.api.common.calendar.ICalendarDetailsProvider;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.TimeIQBGService;

import java.util.List;

public final class TimeIQCalendarUtils {

    /**
     * Gets the available calendars on the user's device, these calendars can be used as readable or writable calendars.
     * @return List of available calendars and their details {@link CalendarDetails} upon success or a corresponding error upon failure.
     */
    public static ResultObject<List<CalendarDetails>> getAvailableCalendars() {
        ICalendarDetailsProvider calendarsDetailsProvider = TimeIQBGService.mTimeIQApi.getCalendarsDetailsProvider();
        ResultData<List<CalendarDetails>> availableCalendarsResultData = calendarsDetailsProvider.getAvailableCalendars();
        return new ResultObject<>(availableCalendarsResultData.isSuccess(), availableCalendarsResultData.getMessage(), availableCalendarsResultData.getData());
    }

    /**
     * Sets the defined list of calendars the SDK is able to read from on the user's device
     * @param calendarIds - A List of string values representing the read calendar ids.
     */
    public static void setReadCalendars(List<String> calendarIds) {
        ICalendarDetailsProvider calendarsDetailsProvider = TimeIQBGService.mTimeIQApi.getCalendarsDetailsProvider();
        calendarsDetailsProvider.setReadCalendars(calendarIds);
    }

    /**
     * Gets the defined read calendars for the SDK
     * @return ResultObject<List<CalendarDetails>> The list of the defined read calendar details
     * {@link CalendarDetails} from which the SDK can read events and other relevant information
     */
    public static ResultObject<List<CalendarDetails>> getReadCalendars() {
        ICalendarDetailsProvider calendarsDetailsProvider = TimeIQBGService.mTimeIQApi.getCalendarsDetailsProvider();
        ResultData<List<CalendarDetails>> readCalendarsResultData = calendarsDetailsProvider.getReadCalendars();
        return new ResultObject<>(readCalendarsResultData.isSuccess(), readCalendarsResultData.getMessage(), readCalendarsResultData.getData());
    }

    /**
     * Sets the write calendar id as defined above.
     * @param calendarId - String that represents the calendar id as defined on the device
     */
    public static void setWriteCalendar(String calendarId) {
        ICalendarDetailsProvider calendarsDetailsProvider = TimeIQBGService.mTimeIQApi.getCalendarsDetailsProvider();
        calendarsDetailsProvider.setWriteCalendar(calendarId);
    }

    /**
     * Gets the defined write calendar - the calendar to which the SDK can write events and other
     * relevant information.
     * @return ResultObject<CalendarDetails> The write calendar details {@link CalendarDetails}
     * if it was defined or a corresponding error code.
     */
    public static ResultObject<CalendarDetails> getWriteCalendar() {
        ICalendarDetailsProvider calendarsDetailsProvider = TimeIQBGService.mTimeIQApi.getCalendarsDetailsProvider();
        ResultData<CalendarDetails> writeCalendarResultData = calendarsDetailsProvider.getWriteCalendar();
        return new ResultObject<>(writeCalendarResultData.isSuccess(), writeCalendarResultData.getMessage(), writeCalendarResultData.getData());
    }

    /**
     * Is there a read calendar defined
     * @return true if there's at least one read calendar defined or false otherwise
     */
    public static boolean isThereAnyReadableCalendarsSet() {
        ResultObject<List<CalendarDetails>> selectedCalendarsResultData = TimeIQCalendarUtils.getReadCalendars();
        return selectedCalendarsResultData != null && selectedCalendarsResultData.isSuccess() && selectedCalendarsResultData.getData() != null && !selectedCalendarsResultData.getData().isEmpty();
    }

}
