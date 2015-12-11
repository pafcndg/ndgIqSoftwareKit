package com.intel.wearable.platform.timeiq.refapp.developer.fragments.state;

import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;

import com.crittercism.app.Crittercism;
import com.crittercism.app.CrittercismConfig;
import com.intel.wearable.platform.timeiq.api.common.auth.TSOUserInfo;
import com.intel.wearable.platform.timeiq.api.userstate.IUserStateChangeListener;
import com.intel.wearable.platform.timeiq.api.userstate.UserState;
import com.intel.wearable.platform.timeiq.api.userstate.UserStateChanges;
import com.intel.wearable.platform.timeiq.api.userstate.UserStateType;
import com.intel.wearable.platform.timeiq.common.core.auth.IAuthorizationManager;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.IDeviceStateListener;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.IDeviceStateManager;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.BatteryStateInfo;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.ChargeMethod;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.DeviceStateData;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.DeviceStateType;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.LocationServicesStateInfo;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.NetworkStateInfo;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.common.logger.placeslogger.LOG_LEVEL;
import com.intel.wearable.platform.timeiq.common.logger.placeslogger.TSOLoggerPlaces;
import com.intel.wearable.platform.timeiq.common.preferences.IUserPrefs;
import com.intel.wearable.platform.timeiq.refapp.developer.BuildConfig;
import com.intel.wearable.platform.timeiq.refapp.developer.R;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.StateHistory.UserStateHistoryCollector;
import com.intel.wearable.platform.timeiq.userstate.IUserStateManagerModule;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.List;
import java.util.Map;

public class DeveloperHistoryManager implements IUserStateChangeListener, IDeviceStateListener {

    /*package*/ interface OnHistoryChangedListener {
        public void historyChanged();
    }

    public interface OnStateHistoryChangedListener {
        public void stateHistoryChanged();
    }

    private static final String TAG = DeveloperHistoryManager.class.getSimpleName();
    /*package*/ static final String HISTORY = "StateFragment.HISTORY";
    private final Context mContext;
    private IUserPrefs m_userPrefs;
    private History m_history;
    private IUserStateManagerModule m_userStateManagerInternal;
    private IDeviceStateManager m_deviceStateManager;
    private static OnHistoryChangedListener m_onHistoryChangedListener;
    private static OnStateHistoryChangedListener m_onStateHistoryChangedListener;
    private UserStateHistoryCollector m_userStateHistoryCollector;

    public DeveloperHistoryManager(Context context) {
        super();
        mContext = context;
        Log.d(TAG, "onCreate");
        m_userStateManagerInternal = ClassFactory.getInstance().resolve(IUserStateManagerModule.class);
        m_userStateManagerInternal.registerForStateChanges(this);
        m_deviceStateManager = ClassFactory.getInstance().resolve(IDeviceStateManager.class);
        m_deviceStateManager.registerListener(this);
        loadHistory();
        addLogsFromPreviousLaunch();    // TODO - do we want this in the release? If yes - move to the BG service
        initCrittercism();              // TODO - do we want this in the release? If yes - move to MainActivity --> OnCreate
        m_userStateHistoryCollector = new UserStateHistoryCollector(this);
        m_userStateHistoryCollector.start();
    }

    private void loadHistory() {
        m_userPrefs = ClassFactory.getInstance().resolve(IUserPrefs.class);
        m_history = new History();
        if (m_userPrefs.contains(HISTORY)) {
            final Map map = m_userPrefs.getMap(HISTORY);
            m_history.initObjectFromMap(map);
        }
    }

    private void addLogsFromPreviousLaunch() {
        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                StringBuilder log = new StringBuilder();

                try {
                    Process process = Runtime.getRuntime().exec("logcat -d -v time");
                    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(process.getInputStream()));

                    String line;
                    while ((line = bufferedReader.readLine()) != null) {
                        log.append(line);
                        log.append("\n");
                    }
                } catch (IOException e) {
                }

                TSOLoggerPlaces.get().addToLog(LOG_LEVEL.DEBUG, TAG, "************************************************************************");
                TSOLoggerPlaces.get().addToLog(LOG_LEVEL.DEBUG, TAG, log.toString());
                TSOLoggerPlaces.get().addToLog(LOG_LEVEL.DEBUG, TAG, "************************************************************************");

                return  null;
            }
        }.execute();
    }

    private void initCrittercism() {
        CrittercismConfig crittercismConfig = new CrittercismConfig();
        crittercismConfig.setLogcatReportingEnabled(true);
        crittercismConfig.setCustomVersionName(BuildConfig.BUILD_DATE_SHORT);
        Crittercism.initialize(mContext, "565c3d71d224ac0a00ed4456", crittercismConfig);    // TODO - do we need to move the key into a properties file?

        JSONObject metadata = new JSONObject();
        IAuthorizationManager authorizationManager = ClassFactory.getInstance().resolve(IAuthorizationManager.class); // TODO - remove or find another way to do this

        TSOUserInfo userId = authorizationManager.getUserInfo();
        if ((userId == null) || (userId.getIdentifier() == null)) {
            try {
                metadata.put("user_id", "(not set)");
            } catch (JSONException e) {
                e.printStackTrace();
            }
        } else {
            Crittercism.setUsername(userId.getIdentifier() + " " + userId.getUserName());
        }

        Crittercism.setMetadata(metadata);
    }

    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        if (m_userStateManagerInternal != null) {
            m_userStateManagerInternal.unregisterForStateChanges(this);
        }

        if (m_deviceStateManager != null) {
            m_deviceStateManager.unRegisterListener(this);
        }

        if (m_userStateHistoryCollector != null) {
            m_userStateHistoryCollector.stop();
        }
    }

    @Override
    public void onStateChanged(final UserState oldState, final UserState newState, final UserStateChanges changes) {
        if (changes.isChanged(UserStateType.MOT)) {
            setMotStateToCurrent(oldState, newState, true);
        }

        if (changes.isChanged(UserStateType.VISIT)) {
            setVisitStateToCurrent(oldState, newState, true);
        }
    }

    private void setMotStateToCurrent(UserState oldUserState, UserState newUserState, boolean addToHistory) {
        String time = "";
        StateType type = StateType.MOT;
        String oldState = "";
        String newStateText = "";

        if (oldUserState != null) {
            time = StateChangeData.getStateTime(oldUserState.getMot());
        } else if (newUserState != null) {
            time = StateChangeData.getStateTime(newUserState.getMot());
        }

        if (oldUserState != null) {
            oldState = StateChangeData.getStateStringFromMot(oldUserState.getMot());
        }

        if (newUserState != null) {
            newStateText = StateChangeData.getStateStringFromMot(newUserState.getMot());
        }

        StateChangeData stateChangeData = new StateChangeData(time, type, oldState, newStateText);
        if (addToHistory) {
            addToHistory(stateChangeData);
        }
    }

    private void setVisitStateToCurrent(UserState oldUserState, UserState newUserState, boolean addToHistory) {
        String time = "";
        StateType type = StateType.ViP;
        String oldState = "";
        String newStateText = "";

        if (oldUserState != null) {
            time = StateChangeData.getStateTime(oldUserState.getVisits());
        } else if (newUserState != null) {
            time = StateChangeData.getStateTime(newUserState.getVisits());
        }

        if (oldUserState != null) {
            oldState = StateChangeData.getStateStringFromVisit(oldUserState.getVisits());
        }

        if (newUserState != null) {
            newStateText = StateChangeData.getStateStringFromVisit(newUserState.getVisits());
        }

        StateChangeData stateChangeData = new StateChangeData(time, type, oldState, newStateText);
        if (addToHistory) {
            addToHistory(stateChangeData);
        }
    }

    @Override
    public <T extends DeviceStateData> void onDeviceStateChange(T deviceStateData, List<DeviceStateType> deviceStateTypeChanges) {
        if (deviceStateTypeChanges.contains(DeviceStateType.BATTERY_CHARGE)){
            final BatteryStateInfo batteryStateInfo = (BatteryStateInfo) deviceStateData.getData();
            setBatteryChargeState(StateChangeData.getStateTime(deviceStateData), batteryStateInfo);
        }

        if (deviceStateTypeChanges.contains(DeviceStateType.NETWORK_AVAILABLE)){
            final NetworkStateInfo networkStateInfo = (NetworkStateInfo) deviceStateData.getData();
            setNetworkAvailableState(StateChangeData.getStateTime(deviceStateData), networkStateInfo);
        }

        if (deviceStateTypeChanges.contains(DeviceStateType.NETWORK_WIFI_AVAILABLE)){
            final NetworkStateInfo networkStateInfo = (NetworkStateInfo) deviceStateData.getData();
            setNetworkWifiAvailableState(StateChangeData.getStateTime(deviceStateData), networkStateInfo);
        }

        if (deviceStateTypeChanges.contains(DeviceStateType.LOCATION_SERVICES_GPS_AVAILABLE)){
            final LocationServicesStateInfo locationServicesStateInfo = (LocationServicesStateInfo) deviceStateData.getData();
            setGpsAvailableState(StateChangeData.getStateTime(deviceStateData), locationServicesStateInfo);
        }
    }

    private void setBatteryChargeState(String time, BatteryStateInfo batteryStateInfo) {
        String state = getBatteryStateText(batteryStateInfo.getChargeMethod(), batteryStateInfo.getBatteryLevel());
        StateChangeData stateChangeData = new StateChangeData(time, StateType.BatteryCharge, state);
        addToHistory(stateChangeData);
    }

    private String getBatteryStateText(ChargeMethod chargeMethod, int batteryLevel) {
        String chargeText = getBatteryChargeData(chargeMethod);
        return chargeText + " (" + StateChangeData.getStateStringFromBatteryLevel(batteryLevel) + ")";
    }

    private String getBatteryChargeData(ChargeMethod chargeMethod) {
        String text = StateChangeData.getStateStringFromChargeMethod(chargeMethod);
        text = (text == null || text.isEmpty()) ? mContext.getResources().getString(R.string.developer_fragment_state_unknown_state) : text;

        return text;
    }

    private void setNetworkAvailableState(String time, NetworkStateInfo networkStateInfo) {
        StateChangeData stateChangeData = new StateChangeData(time, StateType.NetworkAvailable, "" + networkStateInfo.isNetworkAvailable());
        addToHistory(stateChangeData);
    }

    private void setNetworkWifiAvailableState(String time, NetworkStateInfo networkStateInfo) {
        StateChangeData stateChangeData = new StateChangeData(time, StateType.NetworkWifiAvailable, "" + networkStateInfo.isNetworkOverWifi());
        addToHistory(stateChangeData);
    }

    private void setGpsAvailableState(String time, LocationServicesStateInfo locationServicesStateInfo) {
        final boolean gpsAvailable = locationServicesStateInfo.isGPSAvailable();
        StateChangeData stateChangeData = new StateChangeData(time, StateType.GpsAvailable, getGpsAvailableData(gpsAvailable));
        addToHistory(stateChangeData);
    }

    private String getGpsAvailableData(boolean gpsAvailable) {
        return (gpsAvailable ? "" : "not ") + "available";
    }

    /*package*/ static void registerForHistoryChangedListener(OnHistoryChangedListener onHistoryChangedListener) {
        m_onHistoryChangedListener = onHistoryChangedListener;
    }

    /*package*/ static void unregisterForHistoryChangedListener(OnHistoryChangedListener onHistoryChangedListener) {
        if (m_onHistoryChangedListener == onHistoryChangedListener) {
            m_onHistoryChangedListener = null;
        }
    }

    private void addToHistory(StateChangeData stateChangeData) {
        m_history.add(stateChangeData);
        m_userPrefs.setMap(HISTORY, m_history.objectToMap());
        if (m_onHistoryChangedListener != null) {
            m_onHistoryChangedListener.historyChanged();
        }
    }

    public static void registerForStateHistoryChangedListener(OnStateHistoryChangedListener onStateHistoryChangedListener) {
        m_onStateHistoryChangedListener = onStateHistoryChangedListener;
    }

    public static void unregisterStateForHistoryChangedListener(OnStateHistoryChangedListener onStateHistoryChangedListener) {
        if (m_onStateHistoryChangedListener == onStateHistoryChangedListener) {
            m_onStateHistoryChangedListener = null;
        }
    }

    public void dataHasChanged() {
        if (m_onStateHistoryChangedListener != null) {
            m_onStateHistoryChangedListener.stateHistoryChanged();
        }
    }

}
