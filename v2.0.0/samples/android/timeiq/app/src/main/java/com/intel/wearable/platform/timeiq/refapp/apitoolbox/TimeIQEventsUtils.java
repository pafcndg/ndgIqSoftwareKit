package com.intel.wearable.platform.timeiq.refapp.apitoolbox;

import android.content.Context;
import android.os.AsyncTask;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.protocol.enums.TransportType;
import com.intel.wearable.platform.timeiq.api.common.result.Result;
import com.intel.wearable.platform.timeiq.api.common.result.ResultCode;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.events.BeEvent;
import com.intel.wearable.platform.timeiq.api.events.IEvent;
import com.intel.wearable.platform.timeiq.api.events.IEventsEngine;
import com.intel.wearable.platform.timeiq.api.events.TSOEventType;
import com.intel.wearable.platform.timeiq.api.events.TSOEventsResponse;
import com.intel.wearable.platform.timeiq.api.events.UserActionType;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.api.route.EtaRouteData;
import com.intel.wearable.platform.timeiq.api.route.IRouteProvider;
import com.intel.wearable.platform.timeiq.api.route.TtlRouteData;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.TimeIQBGService;
import com.intel.wearable.platform.timeiq.refapp.calendar.ActivityReadableCalendar;
import com.intel.wearable.platform.timeiq.refapp.calendar.ActivityWritableCalendar;
import com.intel.wearable.platform.timeiq.refapp.utils.LocationProviderUtil;
import com.intel.wearable.platform.timeiq.refapp.utils.TextUtil;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.TimeUnit;

/**
 * Created by smoradof on 10/28/2015.
 */
public final class TimeIQEventsUtils {

    private static final long TIME_DELAY = TimeUnit.MINUTES.toMillis(5);

    /**
     * @param eventId the event ID to be deleted
     * @param context App context
     * @return error message on failure, or null on success
     */
    public static String deleteEvent(Context context, String eventId) {
        String errorMsg = null;
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        Result result = eventsEngine.deleteEvent(eventId);

        if(!result.isSuccess()){
            errorMsg = result.getMessage();
            if(errorMsg == null){
                errorMsg = context.getString(R.string.toast_unknown_error);
            }
        }
        return errorMsg;
    }

    /**
     * @param eventId                the event ID to be deleted
     * @param context                App context
     * @param applyToAllInstances    deletes all recurrences
     * @return error message on failure, or null on success
     */
    public static String deleteEvent(Context context, String eventId, boolean applyToAllInstances) {
        String errorMsg = null;
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        Result result = eventsEngine.deleteEvent(eventId, applyToAllInstances);

        if(!result.isSuccess()){
            errorMsg = result.getMessage();
            if(errorMsg == null){
                errorMsg = context.getString(R.string.toast_unknown_error);
            }
        }
        return errorMsg;
    }

    /**
     * Returns an event with the given ID
     * @param eventId    the ID of the event
     * @return {@link IEvent} or null if no event found
     */
    public static IEvent getEventById(String eventId) {
        IEvent event = null;
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        ResultData<IEvent> eventResultData = eventsEngine.getEvent(eventId);
        if (eventResultData != null && eventResultData.isSuccess()) {
            event = eventResultData.getData();
        }

        return event;
    }

    /**
     * Get all the {@link TSOEventType#CALENDAR} events between the given times from the readable calendars (defined by the dialog in {@link ActivityReadableCalendar})
     * @param startTimeFrame    start time in milliseconds
     * @param endTimeFrame      end time in milliseconds
     * @return a list of {@link IEvent}, or an empty list if none exist
     */
    public static List<IEvent> getEventsBetweenTimes(long startTimeFrame, long endTimeFrame) {
        List<IEvent> eventList = new ArrayList<>();
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        ResultData<TSOEventsResponse> resultData = eventsEngine.getEventsByDates(TSOEventType.CALENDAR, startTimeFrame, endTimeFrame);
        if (resultData != null && resultData.getData() != null) {
            eventList.addAll(resultData.getData().getEventsList());
        }

        return eventList;
    }

    /**
     * Get all upcoming {@link TSOEventType#CALENDAR} events
     * @return a list of {@link IEvent}, null if no resultData
     */
    public static List<IEvent> getUpcomingEvents(){
        List<IEvent> eventList = null;
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        ResultData<TSOEventsResponse> resultData = eventsEngine.getCurrentEvents(TSOEventType.CALENDAR);

        if (resultData != null && resultData.getData() != null)
        {
            TSOEventsResponse eventsResponse = resultData.getData();
            eventList= eventsResponse.getEventsList();
        }
        return eventList;
    }


    /**
     * Get all Be events for the next 365 days
     * @return a list of {@link BeEvent}, null if no result
     */
    public static List<BeEvent> getBeEventsForTheNextYear(){
        List<BeEvent> beEventList = null;
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        long from = System.currentTimeMillis();
        long to = from + TimeUnit.DAYS.toMillis(365);
        ResultData<TSOEventsResponse> resultData = eventsEngine.getEventsByDates(TSOEventType.BE, from, to);
        if (resultData != null && resultData.getData() != null) {
            TSOEventsResponse eventsResponse = resultData.getData();
            beEventList = new ArrayList<>();
            List<IEvent> eventsList = eventsResponse.getEventsList();

            for (IEvent event : eventsList) {
                beEventList.add((BeEvent) event);
            }

        }
        return beEventList;
    }


    /**
     * @param context App context
     * @param beEvent The event to add
     * @return error message on failure, or null on success
     */
    public static String addBeEvent(Context context, BeEvent beEvent){
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        Result result = eventsEngine.addEvent(beEvent);
        String errorMsg = null;
        if(!result.isSuccess()){
            errorMsg = result.getMessage();
            if(errorMsg == null){
                errorMsg = context.getString(R.string.toast_unknown_error);
            }
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
    public static void getTtlOrEtaString(IEvent event, Object extraData, Context context, ITtlListener listener){
        if (event != null && listener != null) {
            ResultData<TSOCoordinate> userCoordinateResultData = LocationProviderUtil.getCurrentLocation(context); // origin
            if (userCoordinateResultData != null && userCoordinateResultData.isSuccess()) {
                TSOPlace destinationPlace = event.getLocation();
                if (destinationPlace != null) {
                    TSOCoordinate destinationCoordinate = destinationPlace.getCoordinate();
                    if (destinationCoordinate != null) {
                        new AsyncTaskGetTtl(listener, context, extraData, userCoordinateResultData.getData(), destinationCoordinate, event.getArrivalTime(), event.getPreferredTransportType()).execute();
                    }
                }
            } else if (userCoordinateResultData != null) {
                listener.onTtlOrEtaStringReceived(extraData, context.getString(R.string.ttl_error_cannot_find_location_error, userCoordinateResultData.getMessage()));
            } else {
                listener.onTtlOrEtaStringReceived(extraData, context.getString(R.string.ttl_error_cannot_find_location_null));
            }
        } else if (listener != null) {
            listener.onTtlOrEtaStringReceived(extraData, context.getString(R.string.ttl_error_event_is_null));
        }
    }


    /**
     * A listener to which the {@link #getTtlOrEtaString(IEvent, Object, Context, ITtlListener)} result be given to
     */
    public interface ITtlListener {
        /**
         * When {@link #getTtlOrEtaString(IEvent, Object, Context, ITtlListener)} finishes it's work, this will be triggered
         * @param extraData         the extra data given in {@link #getTtlOrEtaString(IEvent, Object, Context, ITtlListener)}
         * @param ttlOrEtaString    the string that represents the TTL or ETA for the given event
         */
        void onTtlOrEtaStringReceived(Object extraData, String ttlOrEtaString);
    }


    private static class AsyncTaskGetTtl extends AsyncTask<Void, Void, ResultData<TtlRouteData>> {
        final private ITtlListener mListener;
        final private Context mContext;
        final private Object mExtraData;
        final private TSOCoordinate mOrigin;
        final private TSOCoordinate mDestination;
        final private long mArrivalTime;
        final private TransportType mPreferredTransportType;

        private AsyncTaskGetTtl(ITtlListener listener, Context context, Object extraData, TSOCoordinate origin, TSOCoordinate destination, long arrivalTime, TransportType preferredTransportType) {
            mListener = listener;
            mContext = context;
            mExtraData = extraData;
            mOrigin = origin;
            mDestination = destination;
            mArrivalTime = arrivalTime;
            mPreferredTransportType = preferredTransportType;
        }

        @Override
        protected ResultData<TtlRouteData> doInBackground(Void... params) {
            IRouteProvider routeProvider = TimeIQBGService.mTimeIQApi.getRouteProvider();
            return routeProvider.getTTL(mOrigin, mDestination, mArrivalTime, mPreferredTransportType);
        }

        @Override
        protected void onPostExecute(ResultData<TtlRouteData> ttlRouteDataResultData) {
            super.onPostExecute(ttlRouteDataResultData);
            if (ttlRouteDataResultData != null && ttlRouteDataResultData.isSuccess()) { // TTL is valid
                TtlRouteData data = ttlRouteDataResultData.getData();
                long arrivalTime = data.getDepartureTime();
                String arrivalTimeAsString = TextUtil.getHourAsString(arrivalTime);
                String ttlOrEtaString = TextUtil.getRouteString(mContext, data, TextUtil.LateType.NOT_LATE, arrivalTimeAsString);
                mListener.onTtlOrEtaStringReceived(mExtraData, ttlOrEtaString);
            } else if (ttlRouteDataResultData != null) { // TTL had some kind of error, try and see what it is
                ResultCode resultCode = ttlRouteDataResultData.getResultCode();
                switch (resultCode) {
                    case TTL_CANNOT_REACH_DESTINATION_IN_TIME: // there is insufficient time to reach the destination in time
                    // Get ETA
                        new AsyncTaskGetEta(mListener, mContext, mExtraData, mOrigin, mDestination, mArrivalTime, mPreferredTransportType, TextUtil.LateType.LATE).execute();
                        break;
                    case TTL_IS_OVERDUE: // the arrival time has already passed
                        new AsyncTaskGetEta(mListener, mContext, mExtraData, mOrigin, mDestination, mArrivalTime, mPreferredTransportType, TextUtil.LateType.OVERDUE).execute();
                        break;
                    default: // some other error occurred --> send ETA with late
                        new AsyncTaskGetEta(mListener, mContext, mExtraData, mOrigin, mDestination, mArrivalTime, mPreferredTransportType, TextUtil.LateType.LATE).execute();
                        break;
                }
            } else {
                mListener.onTtlOrEtaStringReceived(mExtraData, mContext.getString(R.string.ttl_error_null_ttl));
            }
        }
    }

    private static class AsyncTaskGetEta extends AsyncTask<Void, Void, ResultData<EtaRouteData>> {

        final private ITtlListener mListener;
        final private Context mContext;
        final private Object mExtraData;
        final private TSOCoordinate mOrigin;
        final private TSOCoordinate mDestination;
        final private long mArrivalTime;
        final private TransportType mPreferredTransportType;
        private final TextUtil.LateType mLateType;

        private AsyncTaskGetEta(ITtlListener listener, Context context, Object extraData,
                                TSOCoordinate origin, TSOCoordinate destination, long arrivalTime,
                                TransportType preferredTransportType, TextUtil.LateType lateType) {
            mListener = listener;
            mContext = context;
            mExtraData = extraData;
            mOrigin = origin;
            mDestination = destination;
            mArrivalTime = arrivalTime;
            mPreferredTransportType = preferredTransportType;
            mLateType = lateType;
        }

        @Override
        protected ResultData<EtaRouteData> doInBackground(Void... params) {
            IRouteProvider routeProvider = TimeIQBGService.mTimeIQApi.getRouteProvider();
            final Calendar calendar = Calendar.getInstance();
            long currentTime = calendar.getTimeInMillis();
            return routeProvider.getETA(mOrigin, mDestination, currentTime, mPreferredTransportType);
        }

        @Override
        protected void onPostExecute(ResultData<EtaRouteData> etaRouteDataResultData) {
            super.onPostExecute(etaRouteDataResultData);
            if (etaRouteDataResultData != null && etaRouteDataResultData.isSuccess()) { // ETA is valid
                EtaRouteData data = etaRouteDataResultData.getData();
                String lateTimeAsString = TextUtil.getHourAsString(data.getArrivalTime());
                String ttlOrEtaString = TextUtil.getRouteString(mContext, data, mLateType, lateTimeAsString);
                mListener.onTtlOrEtaStringReceived(mExtraData, ttlOrEtaString);
            } else if (etaRouteDataResultData != null) { // TTL had some kind of error, try and see what it is
                mListener.onTtlOrEtaStringReceived(mExtraData, mContext.getString(R.string.eta_error_general, etaRouteDataResultData.getMessage()));
            } else {
                mListener.onTtlOrEtaStringReceived(mExtraData, mContext.getString(R.string.eta_error_null));
            }
        }
    }




    /**
     * Create a Be event, set its alertOnEventEnd to <code>true</code> in order to be notified when the event ends
     * @param context The Application Context
     * @param selectedPlace target place
     * @param beEventTime   target time
     * @param addToCalendar whether or not to add the event to the calender (defined by the dialog in {@link ActivityWritableCalendar})
     * @return a new {@link BeEvent} object
     */
    public static BeEvent createBeEvent(Context context, TSOPlace selectedPlace, long beEventTime, boolean addToCalendar) {
        if (selectedPlace != null && beEventTime > 0) {
            String subject = context.getString(R.string.be_at_place, selectedPlace.getName());
            String description = context.getString(R.string.be_description);
            return new BeEvent.BeEventBuilder(selectedPlace, beEventTime)
                    .subject(subject)
                    .description(description)
                    .alertOnEventEnd(true)
                    .addToCalendar(addToCalendar)
                    .build();
        }

        return null;
    }

    /**
     * Create a Be event from another Be event
     * @param context The Application Context
     * @param beEvent       the Be event to build upon
     * @param selectedPlace target place
     * @param beEventTime   target time
     * @return a new {@link BeEvent} object
     */
    public static BeEvent createBeEvent(Context context, BeEvent beEvent, TSOPlace selectedPlace, long beEventTime) {
        if (beEvent != null && selectedPlace != null && beEventTime > 0) {
            String subject = context.getString(R.string.be_at_place, selectedPlace.getName());
            String description = context.getString(R.string.be_description);
            return new BeEvent.BeEventBuilder(beEvent)
                    .location(selectedPlace)
                    .arrivalTime(beEventTime)
                    .subject(subject)
                    .description(description)
                    .build();
        }

        return null;
    }

    /**
     * Update an existing event
     * @param context    The Application Context
     * @param event      The event to be updated
     * @return error message on failure, or null on success
     */
    public static String updateEvent(Context context, IEvent event) {
        String errorMsg = null;
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        Result result = eventsEngine.updateEvent(event);

        if(!result.isSuccess()){
            errorMsg = result.getMessage();
            if(errorMsg == null){
                errorMsg = context.getString(R.string.toast_unknown_error);
            }
        }
        return errorMsg;
    }

    /**
     * Snooze the event and get a reminder after five minutes
     * @param eventId    the event ID
     * @return           true on success and false otherwise
     */
    public static boolean snoozeEventForFiveMinutes(String eventId) {
        boolean success = false;
        Set<UserActionType> userActionOptions = getUserActionOptions(eventId);
        if (userActionOptions != null && userActionOptions.contains(UserActionType.TIME_SNOOZE)) {

            IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
            Result result = eventsEngine.chosenUserAction(eventId, UserActionType.TIME_SNOOZE, TIME_DELAY);
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
    public static boolean chooseUserAction(String eventId, UserActionType userActionType) {
        boolean success = false;
        Set<UserActionType> userActionOptions = getUserActionOptions(eventId);
        if (userActionOptions != null && userActionOptions.contains(userActionType)) {

            IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
            Result result = eventsEngine.chosenUserAction(eventId, userActionType);
            success = result.isSuccess();
        }

        return success;
    }

    private static Set<UserActionType> getUserActionOptions(String eventId) {
        Set<UserActionType> userActionOptions = new HashSet<>();
        IEventsEngine eventsEngine = TimeIQBGService.mTimeIQApi.getEventsEngine();
        ResultData<Set<UserActionType>> userActionOptionsResultData = eventsEngine.getUserActionOptions(eventId);
        if (userActionOptionsResultData.isSuccess()) {
            userActionOptions = userActionOptionsResultData.getData();
        }

        return userActionOptions;
    }

}
