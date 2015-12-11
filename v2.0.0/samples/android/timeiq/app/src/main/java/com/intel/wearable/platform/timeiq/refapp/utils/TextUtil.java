package com.intel.wearable.platform.timeiq.refapp.utils;

import android.content.Context;
import android.content.res.Resources;
import android.util.Log;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.common.protocol.enums.MotType;
import com.intel.wearable.platform.timeiq.api.dbobjects.places.semantic.SemanticKey;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.reminders.doReminder.DoReminder;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.callReminder.CallReminder;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.notificationReminder.NotificationReminder;
import com.intel.wearable.platform.timeiq.api.reminders.snooze.PlaceSnoozeOption;
import com.intel.wearable.platform.timeiq.api.reminders.snooze.SnoozeOption;
import com.intel.wearable.platform.timeiq.api.reminders.snooze.TimeDelaySnoozeOption;
import com.intel.wearable.platform.timeiq.api.reminders.snooze.TimeRangeSnoozeOption;
import com.intel.wearable.platform.timeiq.api.route.IRouteData;
import com.intel.wearable.platform.timeiq.api.route.RouteDataType;
import com.intel.wearable.platform.timeiq.api.triggers.ITrigger;
import com.intel.wearable.platform.timeiq.api.triggers.TriggerType;
import com.intel.wearable.platform.timeiq.api.triggers.place.PlaceTrigger;
import com.intel.wearable.platform.timeiq.api.triggers.place.PlaceTriggerType;
import com.intel.wearable.platform.timeiq.api.triggers.time.TimeTrigger;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQPlacesUtils;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.concurrent.TimeUnit;

/**
 * Created by smoradof on 10/15/2015.
 */
public class TextUtil {

    private static final String TAG = "TextUtil";
    private static final DateFormat REMINDER_AT_DATE_FORMAT = new SimpleDateFormat("HH:mm  dd/MM/yy", Locale.US);
    private static final DateFormat HOUR_DATE_FORMAT = new SimpleDateFormat("h:mma", Locale.US);

    public static String getReminderText(Context ctx, IReminder reminder){
        String reminderText = "";
        switch (reminder.getReminderType()){

            case CALL:
                reminderText = ctx.getString(R.string.reminder_call_text, ((CallReminder)reminder).getContactInfo().getName());
                break;
            case NOTIFY:
                reminderText = ctx.getString(R.string.reminder_notify_text, ((NotificationReminder)reminder).getContactInfo().getName());
                break;
            case DO:
                reminderText = ctx.getString(R.string.reminder_do_text, ((DoReminder) reminder).getDoAction());
                break;
        }
        return reminderText;
    }

    public static String getTriggerText(Context ctx, ITrigger trigger) {
        String triggerText = "";
        switch (trigger.getTriggerType()) {

            case CHARGE:
                /*ChargeTrigger chargeTrigger = (ChargeTrigger) trigger;
                String chargeType = chargeTrigger.getChargeTriggerType().name();
                int chargePercent = chargeTrigger.getPercent();*/
                triggerText = ctx.getString(R.string.reminder_type_battery, "15%");
                break;
            case MOT:
                //MotTrigger motTrigger = (MotTrigger) trigger;
                triggerText = ctx.getString(R.string.reminder_type_next_drive);
                break;
            case PLACE:
                PlaceTrigger placeTrigger = (PlaceTrigger) trigger;
                /*triggerText += ctx.getString(R.string.place_trigger, placeTrigger.getPlaceTriggerType().name().toLowerCase(), placeTrigger.getPlaceId().getSemanticKey().getIdentifier());*/
                PlaceID placeId = placeTrigger.getPlaceId();
                SemanticKey semanticKey = placeId.getSemanticKey();
                String identifier = semanticKey.getIdentifier();
                if(placeTrigger.getPlaceTriggerType().equals(PlaceTriggerType.ARRIVE)) {
                    triggerText = ctx.getString(R.string.reminder_type_arrive_to, identifier);
                }
                else{
                    triggerText = ctx.getString(R.string.reminder_type_leave_from, identifier);
                }
                break;
            case TIME:
                triggerText = getReminderDateString(ctx, ((TimeTrigger) trigger).getTriggerTime());
                break;
        }
        return triggerText;
    }

    public static String getReminderNotifyText(Context ctx, IReminder reminder) {
        String reminderText;
        switch (reminder.getReminderType()){

            case CALL:
                CallReminder callReminder = (CallReminder) reminder;
                reminderText = ctx.getString(R.string.reminder_call_text, callReminder.getContactInfo().getName());
                break;
            case NOTIFY:
                NotificationReminder notificationReminder = (NotificationReminder) reminder;
                reminderText = ctx.getString(
                        R.string.reminder_notification,
                        notificationReminder.getContactInfo().getName()
                );
                break;
            case DO:
                DoReminder doReminder = (DoReminder) reminder;
                reminderText = ctx.getString(R.string.reminder_do, doReminder.getDoAction());
                break;
            default:
                Log.e(TAG, "unsupported reminder type");
                reminderText = "";
        }
        return reminderText;
    }

    public static String getReminderNotifySubText(Context ctx, IReminder reminder) {
        String reminderSubText;
        ITrigger trigger = reminder.getTrigger();
        TriggerType triggerType = trigger.getTriggerType();
        switch (triggerType) {
            case CHARGE:
            // Only one type of charge trigger is currently available
                reminderSubText = ctx.getString(R.string.reminder_subtext_battery_above);
                break;
            case MOT:
            // Only one type of MOT trigger is currently available
                reminderSubText = ctx.getString(R.string.reminder_subtext_driving);
                break;
            case PLACE:
                PlaceTrigger placeTrigger = (PlaceTrigger) trigger;
                TSOPlace place = TimeIQPlacesUtils.getPlace(placeTrigger.getPlaceId());
                if (place != null) {
                    PlaceTriggerType placeTriggerType = placeTrigger.getPlaceTriggerType();
                    switch (placeTriggerType) {
                        case ARRIVE:
                            reminderSubText = ctx.getString(R.string.reminder_subtext_arrived, place.getName());
                            break;
                        case LEAVE:
                            reminderSubText = ctx.getString(R.string.reminder_subtext_left, place.getName());
                            break;
                        default:
                            Log.e(TAG, "unsupported place trigger type: " + placeTriggerType.name());
                            reminderSubText = "";
                            break;
                    }
                } else {
                    Log.e(TAG, "place is null");
                    reminderSubText = "";
                }
                break;
            case TIME:
                TimeTrigger timeTrigger = (TimeTrigger) trigger;
                long triggerTime = timeTrigger.getTriggerTime();
                reminderSubText = ctx.getString(R.string.reminder_subtext_time, TextUtil.getHourAsString(triggerTime));
                break;
            default:
                Log.e(TAG, "unsupported trigger type: " + triggerType.name());
                reminderSubText = "";
        }
        return reminderSubText;
    }

    public static String getReminderDateString( Context context, long time)
    {
        Date currDate = new Date( time );
        Resources res = context.getResources();

        return (String.format(res.getString(R.string.reminder_time_at), REMINDER_AT_DATE_FORMAT.format( currDate )));
    }

    /**
     *
     * @param hourInMillis
     * @return String representing the time as SimpleDateFormat "h:mma" with one letter day time.
     *  For example: 12:08p
     */
    public static String getHourAsString(long hourInMillis) {
        return HOUR_DATE_FORMAT.format(new Date(hourInMillis))
                .replace("M", "")
                .toLowerCase();
    }

    public static String getTimeTillAsString(Context context, long timeToInMillis) {
        String result;

        long minutes;
        long hours = TimeUnit.MILLISECONDS.toHours(timeToInMillis);
        Resources res = context.getResources();
        if (hours == 0) {
            minutes = TimeUnit.MILLISECONDS.toMinutes(timeToInMillis);
            result = res.getString(R.string.ttl_min, minutes);
        } else {
            minutes = TimeUnit.MILLISECONDS.toMinutes(timeToInMillis - TimeUnit.HOURS.toMillis(hours));
            result = res.getString(R.string.ttl_hour_and_min, hours, minutes);
        }

        return result;
    }

    public static String getSnoozeOptionDescription(Context context, SnoozeOption snoozeOption) {
        String desc;
        switch (snoozeOption.getType()) {
            case WHEN_CHARGING:
                desc = context.getString(R.string.snooze_option_when_charging);
                break;
            case FROM_CAR:
                desc = context.getString(R.string.snooze_option_from_the_car);
                break;
            case NEXT_DRIVE:
                desc = context.getString(R.string.snooze_option_next_drive);
                break;
            case FROM_PLACE:
                PlaceSnoozeOption placeSnoozeOption = (PlaceSnoozeOption) snoozeOption;
                PlaceID placeId = placeSnoozeOption.getPlaceId();
                TSOPlace place = TimeIQPlacesUtils.getPlace(placeId);
                if (place != null) {
                    desc = context.getString(R.string.snooze_option_from_place, place.getName());
                } else {
                    desc = null;
                    Log.e(TAG, "No place found for placeId: " + placeId.toString());
                }
                break;
            case DEFINE_HOME:
                desc = context.getString(R.string.snooze_option_define_home);
                break;
            case DEFINE_WORK:
                desc = context.getString(R.string.snooze_option_define_work);
                break;
            case NEXT_TIME_AT_CURRENT_PLACE:
                placeSnoozeOption = (PlaceSnoozeOption) snoozeOption;
                placeId = placeSnoozeOption.getPlaceId();
                place = TimeIQPlacesUtils.getPlace(placeId);
                if (place != null) {
                    desc = context.getString(R.string.snooze_option_next_time_at_current_place, place.getName());
                } else {
                    desc = null;
                    Log.e(TAG, "No place found for placeId: " + placeId.toString());
                }
                break;
            case LEAVE_CURRENT_PLACE:
                placeSnoozeOption = (PlaceSnoozeOption) snoozeOption;
                placeId = placeSnoozeOption.getPlaceId();
                place = TimeIQPlacesUtils.getPlace(placeId);
                if (place != null) {
                    desc = context.getString(R.string.snooze_option_leave_current_place, place.getName());
                } else {
                    desc = null;
                    Log.e(TAG, "No place found for placeId: " + placeId.toString());
                }
                break;
            case IN_X_MIN:
                TimeDelaySnoozeOption timeDelaySnoozeOption = (TimeDelaySnoozeOption) snoozeOption;
                desc = context.getString(R.string.snooze_option_in_x_min, timeDelaySnoozeOption.getDelayMinutes());
                break;
            case TIME_RANGE:
                TimeRangeSnoozeOption timeRangeSnoozeOption = (TimeRangeSnoozeOption) snoozeOption;
                switch (timeRangeSnoozeOption.getTimeRange()) {
                    case THIS_MORNING:
                        desc = context.getString(R.string.snooze_option_time_range_this_morning);
                        break;
                    case TODAY:
                        desc = context.getString(R.string.snooze_option_time_range_later_today);
                        break;
                    case THIS_EVENING:
                        desc = context.getString(R.string.snooze_option_time_range_in_the_evening);
                        break;
                    case THIS_NIGHT:
                        desc = context.getString(R.string.snooze_option_time_range_later_tonight);
                        break;
                    case TOMORROW_MORNING:
                        desc = context.getString(R.string.snooze_option_time_range_tomorrow_morning);
                        break;
                    default:
                        desc = null;
                        Log.e(TAG, "Missing time range description for " + timeRangeSnoozeOption.getTimeRange());
                }
                break;
            default:
                desc = null;
                Log.e(TAG, "Missing description for " + snoozeOption.getType());
        }

        return desc;
    }

    /**
     *
     * @param context Context - the Context to get resources from
     * @param motType Input MotType
     * @return Mot Type as string
     */
    public static String getMotString(Context context, MotType motType) {
        String motString = "";
        switch (motType) {
            case CAR:
                motString = context.getString(R.string.ttl_mot_type_string_car);
                break;
            case WALK:
                motString = context.getString(R.string.ttl_mot_type_string_walk);
                break;
            case PUBLIC_TRANSPORT:  // Currently no supported
            case STATIONARY:        //  Cannot occur
                break;
        }

        return motString;
    }


    public enum LateType {
        NOT_LATE,
        LATE,
        OVERDUE
    }

    public static String getRouteString(Context context, IRouteData data, LateType lateType, String etaOrTtlString) {
        String routeString = null;
        String motString = TextUtil.getMotString(context, data.getMainMotType());
        RouteDataType routeDataType = data.getRouteDataType();
        long routeDuration = data.getRouteDuration();
        String routeDurationAsString = TextUtil.getTimeTillAsString(context, routeDuration);

        switch (routeDataType){
            case AT_DESTINATION:
                routeString = context.getString(R.string.route_at_destination);
                break;
            case AT_DESTINATION_WHILE_DRIVING:
                routeString = context.getString(R.string.route_at_destination);
                break;
            case NEAR_DESTINATION:
                routeString = context.getString(R.string.route_near_destination, routeDurationAsString, motString);
                break;
            case DESTINATION_CLOSE_BY:
                routeString = context.getString(R.string.route_near_destination, routeDurationAsString, motString);
                break;
            case DESTINATION_CLOSE_BY_WHILE_DRIVING:
                routeString = context.getString(R.string.route_near_destination, routeDurationAsString, motString);
                break;
            case WALK:
            case DRIVE:
                switch (lateType){
                    case NOT_LATE:
                        routeString = context.getString(R.string.route_ok_not_late, etaOrTtlString, routeDurationAsString, motString);
                        break;
                    case LATE:
                    case OVERDUE:
                        routeString = context.getString(R.string.route_ok_late, etaOrTtlString, routeDurationAsString, motString);
                        break;
                }
                break;
            case DRIVE_WHILE_DRIVING:
                switch (lateType){
                    case NOT_LATE:
                        routeString = context.getString(R.string.route_ok_while_driving_not_late, routeDurationAsString, motString);
                        break;
                    case LATE:
                    case OVERDUE:
                        routeString = context.getString(R.string.route_ok_while_driving_late, etaOrTtlString, routeDurationAsString, motString);
                        break;
                }
                break;
            case TOO_FAR_FOR_WALKING:
                routeString = context.getString(R.string.route_too_far_for_walking);
                break;
            case TOO_FAR_FOR_DRIVING:
                routeString = context.getString(R.string.route_too_far_for_driving);
                break;
        }

        return routeString;
    }

}
