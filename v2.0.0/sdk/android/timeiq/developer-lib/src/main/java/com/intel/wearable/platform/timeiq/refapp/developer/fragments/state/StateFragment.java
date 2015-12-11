package com.intel.wearable.platform.timeiq.refapp.developer.fragments.state;

import android.app.Activity;
import android.content.Intent;
import android.location.Address;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.InflateException;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.dbobjects.places.semantic.SemanticKey;
import com.intel.wearable.platform.timeiq.api.userstate.IUserStateChangeListener;
import com.intel.wearable.platform.timeiq.api.userstate.UserState;
import com.intel.wearable.platform.timeiq.api.userstate.UserStateChanges;
import com.intel.wearable.platform.timeiq.api.userstate.UserStateType;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.IDeviceStateListener;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.IDeviceStateManager;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.BatteryStateInfo;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.ChargeMethod;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.DeviceStateData;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.DeviceStateType;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.LocationServicesStateInfo;
import com.intel.wearable.platform.timeiq.common.devicestatemanager.data.NetworkStateInfo;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.common.protocol.datatypes.location.TSOPosition;
import com.intel.wearable.platform.timeiq.places.locationprovider.ILocationProvider;
import com.intel.wearable.platform.timeiq.places.repo.IPlaceRepoModule;
import com.intel.wearable.platform.timeiq.refapp.developer.R;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.IDeveloperFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.utils.GoogleSearchProvider;
import com.intel.wearable.platform.timeiq.refapp.developer.utils.PlaceToStringUtil;
import com.intel.wearable.platform.timeiq.userstate.IUserStateManagerModule;

import java.util.List;

/**
 * Created by mleib on 19/07/2015.
 */
public class StateFragment extends Fragment implements IUserStateChangeListener, IDeviceStateListener, IDeveloperFragment {

    private static final String TAG = StateFragment.class.getSimpleName();
    private View m_rootView;
    private TextView m_currentMotTextView;
    private TextView m_currentVisitTextView;
    private TextView m_currentBatteryTextView;
    private TextView m_currentNetworkTextView;
    private TextView m_currentGpsTextView;
    private TextView m_currentLocationTextView;
    private TextView m_currentHomeTextView;
    private TextView m_currentWorkTextView;
    private IUserStateManagerModule m_userStateManagerModule;
    private IDeviceStateManager m_deviceStateManager;
    private ILocationProvider mLocationProvider;
    private IPlaceRepoModule m_placesDepot;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        try {
            if (m_rootView != null) {
                ViewGroup parent = (ViewGroup)m_rootView.getParent();
                if (parent != null) {
                    parent.removeView(m_rootView);
                }
            }

            m_rootView = inflater.inflate(R.layout.fragment_state, container, false);
            m_currentMotTextView = (TextView) m_rootView.findViewById(R.id.state_current_mot_text);
            m_currentVisitTextView = (TextView) m_rootView.findViewById(R.id.state_current_visit_text);
            m_currentBatteryTextView = (TextView) m_rootView.findViewById(R.id.state_current_battery_text);
            m_currentNetworkTextView = (TextView) m_rootView.findViewById(R.id.state_current_network_text);
            m_currentGpsTextView = (TextView) m_rootView.findViewById(R.id.state_current_gps_text);
            m_currentLocationTextView = (TextView) m_rootView.findViewById(R.id.state_current_location_text);
            m_currentHomeTextView = (TextView) m_rootView.findViewById(R.id.state_current_home_text);
            m_currentWorkTextView = (TextView) m_rootView.findViewById(R.id.state_current_work_text);

            final Button historyButton = (Button) m_rootView.findViewById(R.id.state_open_history_button);
            historyButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent historyIntent = new Intent(getActivity(), StateHistoryActivity.class);
                    startActivity(historyIntent);
                }
            });

            m_userStateManagerModule = ClassFactory.getInstance().resolve(IUserStateManagerModule.class);
            m_userStateManagerModule.registerForStateChanges(this);
            m_deviceStateManager = ClassFactory.getInstance().resolve(IDeviceStateManager.class);
            m_deviceStateManager.registerListener(this);
            mLocationProvider = ClassFactory.getInstance().resolve(ILocationProvider.class);
            m_placesDepot = ClassFactory.getInstance().resolve(IPlaceRepoModule.class);
        } catch(InflateException ex) { }

        return m_rootView;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        if (m_userStateManagerModule != null) {
            m_userStateManagerModule.unregisterForStateChanges(this);
        }
        if (m_deviceStateManager != null) {
            m_deviceStateManager.unRegisterListener(this);
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        if (isAdded()) {
            refresh();
        }
    }

    public void refresh() {
        ResultData<UserState> userStateResultData = m_userStateManagerModule.getCurrentState();
        if (userStateResultData.isSuccess()) {
            UserState userState = userStateResultData.getData();
            setMotStateToCurrent(userState, R.string.developer_fragment_state_unknown_state);
            setVisitStateToCurrent(userState, R.string.developer_fragment_state_unknown_state);

            setBatteryStateToCurrent(m_deviceStateManager.getChargingMethod(), m_deviceStateManager.getBatteryLevel());
            setNetworkAvailableStateToCurrent(m_deviceStateManager.isNetworkAvailable(), m_deviceStateManager.isNetworkOverWifi());
            setGpsAvailableStateToCurrent(m_deviceStateManager.isGPSAvailable());
            setCurrentLocation();
            setHomeToCurrent();
            setWorkToCurrent();
        } else {
            Toast.makeText(getActivity(), getActivity().getString(R.string.developer_fragment_error_getting_state, userStateResultData.getMessage()), Toast.LENGTH_LONG).show();
        }
    }

    @Override
    public void onStateChanged(final UserState oldState, final UserState newState, final UserStateChanges changes) {
        Activity activity = getActivity();
        if (activity != null) {
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (changes.isChanged(UserStateType.MOT)) {
                        setMotStateToCurrent(newState, R.string.developer_fragment_state_no_state);
                    }

                    if (changes.isChanged(UserStateType.VISIT)) {
                        setVisitStateToCurrent(newState, R.string.developer_fragment_state_no_state);
                    }
                }
            });
        } else {
            if (changes.isChanged(UserStateType.MOT)) {
                setMotStateToCurrent(newState, R.string.developer_fragment_state_no_state);
            }

            if (changes.isChanged(UserStateType.VISIT)) {
                setVisitStateToCurrent(newState, R.string.developer_fragment_state_no_state);
            }
        }
    }

    private void setMotStateToCurrent(UserState newUserState, int resourceIdForError) {
        String newStateText = "";
        if (newUserState != null) {
            newStateText = StateChangeData.getStateStringFromMot(newUserState.getMot());
        }

        setText(m_currentMotTextView, newStateText, resourceIdForError);
    }

    private void setVisitStateToCurrent(UserState newUserState, int resourceIdForError) {
        String newStateText = "";
        if (newUserState != null) {
            newStateText = StateChangeData.getStateStringFromVisit(newUserState.getVisits());
        }

        setText(m_currentVisitTextView, newStateText, resourceIdForError);
    }

    @Override
    public <T extends DeviceStateData> void onDeviceStateChange(T deviceStateData, List<DeviceStateType> deviceStateTypeChanges) {
        if (deviceStateTypeChanges.contains(DeviceStateType.BATTERY_CHARGE)){
            final BatteryStateInfo batteryStateInfo = (BatteryStateInfo) deviceStateData.getData();
            setBatteryChargeState(batteryStateInfo);
        }

        if (deviceStateTypeChanges.contains(DeviceStateType.BATTERY_LEVEL)){
            final BatteryStateInfo batteryStateInfo = (BatteryStateInfo) deviceStateData.getData();
            setBatteryLevelState(batteryStateInfo);
        }

        if (deviceStateTypeChanges.contains(DeviceStateType.NETWORK_AVAILABLE)){
            final NetworkStateInfo networkStateInfo = (NetworkStateInfo) deviceStateData.getData();
            setNetworkAvailableState(networkStateInfo);
        }

        if (deviceStateTypeChanges.contains(DeviceStateType.NETWORK_WIFI_AVAILABLE)){
            final NetworkStateInfo networkStateInfo = (NetworkStateInfo) deviceStateData.getData();
            setNetworkWifiAvailableState(networkStateInfo);
        }

        if (deviceStateTypeChanges.contains(DeviceStateType.LOCATION_SERVICES_GPS_AVAILABLE)){
            final LocationServicesStateInfo locationServicesStateInfo = (LocationServicesStateInfo) deviceStateData.getData();
            setGpsAvailableState(locationServicesStateInfo);
        }
    }

    private void setBatteryChargeState(BatteryStateInfo batteryStateInfo) {
        setBatteryStateToCurrent(batteryStateInfo.getChargeMethod(), batteryStateInfo.getBatteryLevel());
    }

    private void setBatteryLevelState(BatteryStateInfo batteryStateInfo) {
        setBatteryStateToCurrent(batteryStateInfo.getChargeMethod(), batteryStateInfo.getBatteryLevel());
    }

    private void setBatteryStateToCurrent(ChargeMethod chargeMethod, int batteryLevel) {
        final String text = getBatteryStateText(chargeMethod, batteryLevel);
        setText(m_currentBatteryTextView, text);
    }

    private String getBatteryStateText(ChargeMethod chargeMethod, int batteryLevel) {
        String chargeText = getBatteryChargeData(chargeMethod);
        return chargeText + " (" + StateChangeData.getStateStringFromBatteryLevel(batteryLevel) + ")";
    }

    private String getBatteryChargeData(ChargeMethod chargeMethod) {
        String text = StateChangeData.getStateStringFromChargeMethod(chargeMethod);
        text = (text == null || text.isEmpty()) ? getResources().getString(R.string.developer_fragment_state_unknown_state) : text;

        return text;
    }

    private void setNetworkAvailableState(NetworkStateInfo networkStateInfo) {
        setNetworkAvailableStateToCurrent(networkStateInfo.isNetworkAvailable(), networkStateInfo.isNetworkOverWifi());
    }

    private void setNetworkWifiAvailableState(NetworkStateInfo networkStateInfo) {
        setNetworkAvailableStateToCurrent(networkStateInfo.isNetworkAvailable(), networkStateInfo.isNetworkOverWifi());
    }

    private void setNetworkAvailableStateToCurrent(boolean networkAvailable, boolean networkOverWifi) {
        String networkText = getNetworkAvailableData(networkAvailable);
        final String text = networkText + (networkOverWifi ? " (" : " (no ") + "WiFi)";

        setText(m_currentNetworkTextView, text);
    }

    private String getNetworkAvailableData(boolean networkAvailable) {
        return (networkAvailable ? "" : "not ") + "available";
    }

    private void setGpsAvailableState(LocationServicesStateInfo locationServicesStateInfo) {
        final boolean gpsAvailable = locationServicesStateInfo.isGPSAvailable();
        setGpsAvailableStateToCurrent(gpsAvailable);
    }

    private void setGpsAvailableStateToCurrent(final boolean gpsAvailable) {
        String text = getGpsAvailableData(gpsAvailable);
        setText(m_currentGpsTextView, text);
    }

    private String getGpsAvailableData(boolean gpsAvailable) {
        return (gpsAvailable ? "" : "not ") + "available";
    }

    private void setCurrentLocation() {
        String locationString = getString(R.string.developer_fragment_state_unknown_state);
        ResultData<TSOPosition> lastKnownLocationResultData = mLocationProvider.getLastKnownLocation();
        if (lastKnownLocationResultData != null && lastKnownLocationResultData.isSuccess()) {
            TSOPosition lastKnownLocation = lastKnownLocationResultData.getData();
            if (lastKnownLocation != null) {
                TSOCoordinate coordinate = lastKnownLocation.getCoordinate();
                if (coordinate != null) {
                    Address rgcFromLocation = GoogleSearchProvider.getRGCFromLocation(getActivity(), coordinate.getLatitude(), coordinate.getLongitude());
                    if (rgcFromLocation != null) {
                        locationString = GoogleSearchProvider.getAddressString(rgcFromLocation);
                    }
                } else {
                    locationString = lastKnownLocation.toString();
                }
            }
        }

        m_currentLocationTextView.setText(locationString);
    }

    private void setHomeToCurrent() {
        PlaceID homeId = m_placesDepot.getPlaceIdInternal(SemanticKey.HOME);
        if (homeId == null) {
            homeId = m_placesDepot.getPlaceIdInternal(SemanticKey.AUTODETECTED_HOME);
        }
        m_currentHomeTextView.setText(PlaceToStringUtil.getPlaceDescription(homeId, getResources().getString(R.string.developer_fragment_state_unknown_state)));
    }

    private void setWorkToCurrent() {
        PlaceID workId = m_placesDepot.getPlaceIdInternal(SemanticKey.WORK);
        if (workId == null) {
            workId = m_placesDepot.getPlaceIdInternal(SemanticKey.AUTODETECTED_WORK);
        }
        final String text = PlaceToStringUtil.getPlaceDescription(workId, getResources().getString(R.string.developer_fragment_state_unknown_state));
        setText(m_currentWorkTextView, text);
    }

    private void setText(final TextView textView, final String newStateText, final int resourceIdForError) {
        Activity activity = getActivity();
        if (isAdded()) {
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (newStateText != null && !newStateText.trim().isEmpty()) {
                        textView.setText(newStateText);
                    } else {
                        textView.setText(getResources().getString(resourceIdForError));
                    }
                }
            });
        }
    }

    private void setText(final TextView textView, final String text) {
        Activity activity = getActivity();
        if (isAdded()) {
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    textView.setText(text);
                }
            });
        }
    }

    @Override
    public void onFragmentHidden() {
        Log.d(TAG, "onFragmentHidden");
    }

    @Override
    public void onFragmentShown() {
        Log.d(TAG, "onFragmentShown");
        refreshAndSetFloatingButton();
    }

    private void refreshAndSetFloatingButton() {
        refresh();
        Activity activity = getActivity();
        if (activity != null) {
            FloatingActionButton floatingActionButton = (FloatingActionButton) activity.findViewById(R.id.fab);
            if (floatingActionButton != null) {
                floatingActionButton.setVisibility(View.VISIBLE);
                floatingActionButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        refresh();
                    }
                });
            }
        }
    }

}
