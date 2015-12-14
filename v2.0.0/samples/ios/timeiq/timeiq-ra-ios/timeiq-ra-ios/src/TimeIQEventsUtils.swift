//
//  TimeIQEventsUtils.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 01/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import TimeIQ

private let TIME_DELAY: long64 = TimeUnit.MINUTES().toMillisWithLong(5);

class TimeIQEventsUtils {
    
    class func getUpcomingEvents() -> ArrayList {
        var eventList = ArrayList()
        
        let resultData = TimeIQBGService.sharedInstance.timeIQApi?.getEventsEngine().getCurrentEventsWithTSOEventType(TSOEventType.CALENDAR());
        
        if (resultData != nil && resultData!.getData() != nil)
        {
            let eventsResponse = resultData!.getData();
            eventList = eventsResponse.getEventsList();
        }
        return eventList;
    }
    
    /**
     * Get all Be events for the next 365 days
     * @return a list of {@link BeEvent}, null if no result
     */
    class func getBeEventsForTheNextYear() -> Array<AnyObject>
    {
        var beEventList = Array<BeEvent>()
        let eventsManager = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine();
        let from = System.currentTimeMillis();
        let to = from + TimeUnit.DAYS().toMillisWithLong(365);
        let resultData = eventsManager.getEventsByDatesWithTSOEventType(TSOEventType.BE(), withLong: from, withLong: to);
        if (resultData != nil && resultData.getData() != nil)
        {
            let eventsResponse = resultData.getData();
            let eventsList = eventsResponse.getEventsList();
            
            for event in eventsList
            {
                beEventList.append(event as! BeEvent)
            }
            
        }
        return beEventList;
    }
    
    /**
    * Get all the {@link TSOEventType#CALENDAR} events between the given times from the readable calendars (defined by the dialog in {@link ActivityReadableCalendar})
    * @param startTimeFrame    start time in milliseconds
    * @param endTimeFrame      end time in milliseconds
    * @return a list of {@link IEvent}, or an empty list if none exist
    */
    class func getEventsBetweenTimes(startTimeFrame: long64, endTimeFrame: long64) -> ArrayList {
        let eventList = ArrayList()
        let eventsEngine = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine();
        let resultData = eventsEngine.getEventsByDatesWithTSOEventType(TSOEventType.CALENDAR(), withLong: startTimeFrame, withLong: endTimeFrame)
        if (resultData != nil && resultData.getData() != nil) {
            eventList.addAllWithJavaUtilCollection(resultData.getData().getEventsList() as [AnyObject]);
        }
        
        return eventList;
    }
    
    /**
     * Returns an event with the given ID
     * @param eventId    the ID of the event
     * @return {@link IEvent} or null if no event found
     */
    class func getEventById(eventId: String) -> IEvent?
    {
        let event: IEvent?
        
        let eventResultData = TimeIQBGService.sharedInstance.timeIQApi?.getEventsEngine().getEventWithNSString(eventId);
        if (eventResultData != nil && eventResultData!.isSuccess()) {
            event = eventResultData!.getData() as? IEvent
            return event
        }
        
        return nil;
    }
    
     /**
     * @param eventId the event ID to be deleted
     * @return error message on failure, or null on success
     */
    class func deleteEvent(eventId: String) -> Bool
    {
        let result = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine().deleteEventWithNSString(eventId);
        
        return result.isSuccess();
    }
    
    /**
     * @param context App context
     * @param beEvent The event to add
     * @return error message on failure, or null on success
     */
    class func addBeEvent(beEvent: BeEvent) -> (success: Bool, message: String) {
        let result = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine().addEventWithIEvent(beEvent);
        var errorMsg = ""
        if(!result.isSuccess()){
            errorMsg = result.getMessage();
        }
        return (result.isSuccess(), errorMsg);
    }
    
    /**
     * Update an existing event
     * @param context    The Application Context
     * @param event      The event to be updated
     * @return error message on failure, or null on success
     */
    class func updateEvent(event: IEvent) -> String {
        var errorMsg = ""
        let result = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine().updateEventWithIEvent(event);
        
        if(!result.isSuccess()){
            errorMsg = result.getMessage();
        }
        return errorMsg;
    }
    
    /**
     * Get a string that represents the TTL or ETA for the given event. <BR>
     * ETA will be given if there was an error with the TTL (see {@link AsyncTaskGetTtl#onPostExecute(ResultData)}) <BR>
     * The result will be given to the listener provided ({@link ITtlListener#onTtlOrEtaStringReceived(Object, String)})
     * @param event        the event that the TTL is provided for
     * @param extraData    an extra data that will be returned to the listener
     * @param context      the Android context
     * @param listener     the listener to which the resulting string will be given to
     */
    class func getTtlOrEtaString(event: IEvent, callback: (String)->())// -> String
    {
        let destinationPlace = event.getLocation();
        if (destinationPlace != nil) {
            let destinationCoordinate = destinationPlace.getCoordinate();
            if (destinationCoordinate != nil) {
                
                //    AsyncTaskGetTtl(listener, context, extraData, userCoordinateResultData.getData(), destinationCoordinate, event.getArrivalTime(), event.getPreferredTransportType()).execute();
                
                let currentLocation = TSOCoordinate(double: LocationApi.currentLocation().coordinate.latitude, withDouble: LocationApi.currentLocation().coordinate.longitude);
                let routeProvider = TimeIQBGService.sharedInstance.timeIQApi!.getRouteProvider();
                let ttlRouteDataResultData = routeProvider.getTTLWithTSOCoordinate(currentLocation, withTSOCoordinate: destinationCoordinate, withLong: event.getArrivalTime(), withTransportType: event.getPreferredTransportType());
                
                if (ttlRouteDataResultData != nil && ttlRouteDataResultData.isSuccess()) { // TTL is valid
                    let data = ttlRouteDataResultData.getData() as! TtlRouteData;
                    let arrivalTime = data.getDepartureTime();
                    let arrivalTimeAsString = TextFormatterUtil.getHourAsString(arrivalTime);
                    let ttlOrEtaString = TextFormatterUtil.getRouteString(data, lateType: .NOT_LATE, etaOrTtlString: arrivalTimeAsString);
                    callback(ttlOrEtaString!)
                    return
//                    return ttlOrEtaString!
                } else if (ttlRouteDataResultData != nil) { // TTL had some kind of error, try and see what it is
                    let resultCode = ttlRouteDataResultData.getResultCode();
                    switch (resultCode) {
                    case ResultCode.TTL_CANNOT_REACH_DESTINATION_IN_TIME(): // there is insufficient time to reach the destination in time
                        // Get ETA
//                        new AsyncTaskGetEta(mListener, mContext, mExtraData, mOrigin, mDestination, mArrivalTime, mPreferredTransportType, TextUtil.LateType.LATE).execute();
                        break;
                    case ResultCode.TTL_IS_OVERDUE(): // the arrival time has already passed
//                        new AsyncTaskGetEta(mListener, mContext, mExtraData, mOrigin, mDestination, mArrivalTime, mPreferredTransportType, TextUtil.LateType.OVERDUE).execute();
                        break;
                    default: // some other error occurred --> send ETA with late
//                        new AsyncTaskGetEta(mListener, mContext, mExtraData, mOrigin, mDestination, mArrivalTime, mPreferredTransportType, TextUtil.LateType.LATE).execute();
                        break;
                    }
                } else {
//                    mListener.onTtlOrEtaStringReceived(mExtraData, mContext.getString(R.string.ttl_error_null_ttl));
                }
            }
        }
        callback(NSLocalizedString("ttl_error_null_ttl", comment: ""));
//        return NSLocalizedString("ttl_error_null_ttl", comment: "");
    }
    
    /**
     * Snooze the event and get a reminder after five minutes
     * @param eventId    the event ID
     * @return           true on success and false otherwise
     */
    class func snoozeEventWithMinutes(eventId: String, snoozeTime : long64) -> Bool {
        var success = false;
        let userActionOptions = getUserActionOptions(eventId);
        if (userActionOptions.containsObject(UserActionType.TIME_SNOOZE())) {
            let eventsEngine = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine();
            let result = eventsEngine.chosenUserActionWithNSString(eventId, withUserActionType: UserActionType.TIME_SNOOZE(), withLong: snoozeTime);
            success = result.isSuccess();
        }
        
        return success;
    }
    
    /**
     * Snooze the event and get a reminder after five minutes
     * @param eventId    the event ID
     * @return           true on success and false otherwise
     */
    class func snoozeEventForFiveMinutes(eventId: String) -> Bool {
        var success = false;
        let userActionOptions = getUserActionOptions(eventId);
        if (userActionOptions.containsObject(UserActionType.TIME_SNOOZE())) {
            let eventsEngine = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine();
            let result = eventsEngine.chosenUserActionWithNSString(eventId, withUserActionType: UserActionType.TIME_SNOOZE(), withLong: TIME_DELAY);
            success = result.isSuccess();
        }
        
        return success;
    }
    
    /**
     * Choose how to act when a message about the event has been received
     * @param eventId           the event ID
     * @param userActionType    the action type to set
     * @return                  true on success and false otherwise
     */
    class func chooseUserAction(eventId: String, userActionType: UserActionType) -> Bool {
        var success = false;
        let userActionOptions = getUserActionOptions(eventId);
        if (userActionOptions.containsObject(userActionType)) {
            
            let eventsEngine = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine();
            let result = eventsEngine.chosenUserActionWithNSString(eventId, withUserActionType: userActionType);
            success = result.isSuccess();
        }
        
        return success;
    }
    
    class func getUserActionOptions(eventId: String) -> HashSet {
        var userActionOptions:HashSet = HashSet()
        let eventsEngine = TimeIQBGService.sharedInstance.timeIQApi!.getEventsEngine();
        let userActionOptionsResultData = eventsEngine.getUserActionOptionsWithNSString(eventId);
        if (userActionOptionsResultData.isSuccess()) {
            userActionOptions = userActionOptionsResultData.getData() as! HashSet;
        }
        
        return userActionOptions;
    }
}
