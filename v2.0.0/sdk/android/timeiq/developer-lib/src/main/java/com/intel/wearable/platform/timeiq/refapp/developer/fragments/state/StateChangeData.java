package com.intel.wearable.platform.timeiq.refapp.developer.fragments.state;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.common.protocol.enums.MotType;
import com.intel.wearable.platform.timeiq.api.common.protocol.interfaces.IMappable;
import com.intel.wearable.platform.timeiq.api.userstate.UserStateData;
import com.intel.wearable.platform.timeiq.api.userstate.VisitedPlaces;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.ChargeMethod;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.DeviceStateData;
import com.intel.wearable.platform.timeiq.common.utils.time.DateFormatType;
import com.intel.wearable.platform.timeiq.common.utils.time.PlacesTimeFormatUtil;
import com.intel.wearable.platform.timeiq.dbobjects.abstracts.ATSOBaseDBObject;
import com.intel.wearable.platform.timeiq.dbobjects.abstracts.ATSOSyncDbObject;
import com.intel.wearable.platform.timeiq.refapp.developer.utils.PlaceToStringUtil;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by mleib on 21/07/2015.
 */
/*package*/ class StateChangeData implements IMappable {

    private String m_userId;
    private static final String USER_ID_FIELD = ATSOSyncDbObject.USER_ID_FIELD;
    private String m_id;
    private static final String ID_FIELD = ATSOBaseDBObject.OBJECT_ID_FIELD;
    /*package*/ String time;
    private static final String TIME_ID_FIELD = "time";
    /*package*/ StateType type;
    private static final String TYPE_FIELD = "type";
    /*package*/ String oldState;
    private static final String OLD_STATE_FIELD = "oldState";
    /*package*/ String newState;
    private static final String NEW_STATE_FIELD = "newState";

    public StateChangeData() { }

    /*package*/ StateChangeData(String time, StateType type, String oldState, String newState) {
        this.time = time;
        this.type = type;
        this.oldState = oldState;
        this.newState = newState;
    }

    /*package*/ StateChangeData(String time, StateType type, String oldState) {
        this.time = time;
        this.type = type;
        this.oldState = oldState;
        this.newState = null;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        StateChangeData that = (StateChangeData) o;

        if (time != null ? !time.equals(that.time) : that.time != null) return false;
        if (type != that.type) return false;
        if (oldState != null ? !oldState.equals(that.oldState) : that.oldState != null)
            return false;
        return !(newState != null ? !newState.equals(that.newState) : that.newState != null);

    }

    @Override
    public int hashCode() {
        int result = time != null ? time.hashCode() : 0;
        result = 31 * result + (type != null ? type.hashCode() : 0);
        result = 31 * result + (oldState != null ? oldState.hashCode() : 0);
        result = 31 * result + (newState != null ? newState.hashCode() : 0);
        return result;
    }

    @Override
    public void initObjectFromMap(Map<String, Object> stringObjectMap) {
        if(stringObjectMap != null) {
            if(stringObjectMap.containsKey(USER_ID_FIELD)) {
                this.m_userId = (String)stringObjectMap.get(USER_ID_FIELD);
            }

            if(stringObjectMap.containsKey(ID_FIELD)) {
                this.m_id = (String)stringObjectMap.get(ID_FIELD);
            }

            if(stringObjectMap.containsKey(TIME_ID_FIELD)) {
                this.time = (String)stringObjectMap.get(TIME_ID_FIELD);
            }

            if(stringObjectMap.containsKey(TYPE_FIELD)) {
                this.type = StateType.valueOf((String) stringObjectMap.get(TYPE_FIELD));
            }

            if(stringObjectMap.containsKey(OLD_STATE_FIELD)) {
                this.oldState = (String)stringObjectMap.get(OLD_STATE_FIELD);
            }

            if(stringObjectMap.containsKey(NEW_STATE_FIELD)) {
                this.newState = (String)stringObjectMap.get(NEW_STATE_FIELD);
            }
        }
    }

    @Override
    public Map<String, Object> objectToMap() {
        Map<String, Object> objectMap = new HashMap<>();

        if(m_userId != null) { objectMap.put(USER_ID_FIELD, m_userId); }
        if(m_id != null) { objectMap.put(ID_FIELD, m_id); }
        if(time != null) { objectMap.put(TIME_ID_FIELD, time); }
        if(type != null) { objectMap.put(TYPE_FIELD, type.toString()); }
        if(oldState != null) { objectMap.put(OLD_STATE_FIELD, oldState); }
        if(newState != null) { objectMap.put(NEW_STATE_FIELD, newState); }

        return objectMap;
    }

    /*package*/ static <T> String getStateTime(UserStateData<T> userStateData) {
        String res = "";

        if (userStateData != null) {
            res = getDateTimeString(userStateData.getTimeStamp());
        }

        return res;
    }

    /*package*/ static <T> String getStateTime(DeviceStateData<T> deviceStateData) {
        String res = "";

        if (deviceStateData != null) {
            res = getDateTimeString(deviceStateData.getTimeStamp());
        }

        return res;
    }

    private static String getDateTimeString(long time) {
        return PlacesTimeFormatUtil.convertTimeStampToDateString(time, DateFormatType.DATE_SHORT);
    }

    /*package*/ static String getStateStringFromMot(UserStateData<MotType> stateMot) {
        String res = "";

        if (stateMot != null) {
            MotType motType = stateMot.getData();
            if (motType != null) {
                res = motType.name();
            }
        }

        return res;
    }

    /*package*/ static String getStateStringFromVisit(UserStateData<VisitedPlaces> stateVisits) {
        String res = "";

        if (stateVisits != null) {
            VisitedPlaces visitedPlaces = stateVisits.getData();
            if (visitedPlaces != null) {
                res = getListAsString(visitedPlaces);
            }
        }

        return res;
    }

    private static String getListAsString(VisitedPlaces visitedPlaces) {
        StringBuilder stringBuilder = new StringBuilder("");
        String placeDescription;
        if (visitedPlaces != null) {
            int i = 0;
            for (PlaceID visitedPlace : visitedPlaces) {
                placeDescription = PlaceToStringUtil.getPlaceDescription(visitedPlace, visitedPlace.getIdentifier());
                if (i > 0) {
                    stringBuilder.append("\n");
                }

                stringBuilder.append(placeDescription);
                ++i;
            }
        }

        return stringBuilder.toString();
    }

    /*package*/ static String getStateStringFromChargeMethod(ChargeMethod chargeMethod) {
        String res = "";

        if (chargeMethod != null) {
            res = chargeMethod.name();
        }

        return res;
    }

    /*package*/ static String getStateStringFromBatteryLevel(int batteryLevel) {
        String res = "";

        res = batteryLevel + "%";

        return res;
    }

//    private void setArrivedAndLeftVisits(UserState oldState, UserState newState) {
//        UserStateData<VisitedPlaces> oldVisits = oldState == null ? null : oldState.getVisits();
//        UserStateData<VisitedPlaces> newVisits = newState == null ? null : newState.getVisits();
//        VisitedPlaces newVisitedPlaces = newVisits == null ? null : newVisits.getData();
//        VisitedPlaces oldVisitedPlaces = oldVisits == null ? null : oldVisits.getData();
//        VisitedPlaces arrivedPlacesList = new VisitedPlaces();
//        VisitedPlaces leftPlacesList = new VisitedPlaces();
//
//        if(newVisitedPlaces != null) {
//            newVisitedPlaces.getMutualDiff(oldVisitedPlaces, arrivedPlacesList, leftPlacesList);
//        } else if(oldVisitedPlaces != null) {
//            leftPlacesList.addAll(oldVisitedPlaces);
//        }
//
//        arrivedPlaces = getListAsString(arrivedPlacesList);
//        leftPlaces = getListAsString(leftPlacesList);
//    }

}
