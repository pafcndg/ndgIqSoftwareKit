package com.intel.wearable.platform.timeiq.refapp.developer.fragments.state;

import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.view.View;
import android.widget.CheckBox;

import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.common.preferences.IUserPrefs;
import com.intel.wearable.platform.timeiq.refapp.developer.R;

import java.util.ArrayList;
import java.util.Map;


public class StateHistoryActivity extends FragmentActivity implements DeveloperHistoryManager.OnHistoryChangedListener {

    private StateLogListFragment m_stateLogListFragment;
    private IUserPrefs m_userPrefs;
    private CheckBox m_motCheckBox, m_vipCheckBox, m_batteryChargeCheckBox, /*m_batteryLevelCheckBox,*/ m_networkCheckBox, m_gpsCheckBox;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_state_history);
        m_motCheckBox = (CheckBox) findViewById(R.id.history_filter_mot_checkbox);
        m_vipCheckBox = (CheckBox) findViewById(R.id.history_filter_vip_checkbox);
        m_batteryChargeCheckBox = (CheckBox) findViewById(R.id.history_filter_battery_charge_checkbox);
//        m_batteryLevelCheckBox = (CheckBox) findViewById(R.id.history_filter_battery_level_checkbox);
        m_networkCheckBox = (CheckBox) findViewById(R.id.history_filter_network_checkbox);
        m_gpsCheckBox = (CheckBox) findViewById(R.id.history_filter_gps_checkbox);
        m_stateLogListFragment = (StateLogListFragment) getFragmentManager().findFragmentById(R.id.state_log);
        m_stateLogListFragment.setTitle(getResources().getString(R.string.developer_fragment_state_history_button_text));
        m_userPrefs = ClassFactory.getInstance().resolve(IUserPrefs.class);

        DeveloperHistoryManager.registerForHistoryChangedListener(this);

        refresh();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        DeveloperHistoryManager.unregisterForHistoryChangedListener(this);
    }

    public void refresh(View view) {
        refresh();
    }

    private void refresh() {
        History history = new History();
        if (m_userPrefs.contains(DeveloperHistoryManager.HISTORY)) {
            final Map map = m_userPrefs.getMap(DeveloperHistoryManager.HISTORY);
            history.initObjectFromMap(map);
        }

        m_stateLogListFragment.clearList();

        final ArrayList<StateChangeData> stateChangeDataList = history.getHistory();
        StateChangeData stateChangeData;
        for (int index = stateChangeDataList.size() - 1 ; index >= 0  ; --index) {
            boolean add;
            stateChangeData = stateChangeDataList.get(index);
            switch (stateChangeData.type) {
                case MOT:
                    add = m_motCheckBox.isChecked();
                    break;
                case ViP:
                    add = m_vipCheckBox.isChecked();
                    break;
                case BatteryCharge:
                    add = m_batteryChargeCheckBox.isChecked();
                    break;
//                case BatteryLevel:
//                    add = m_batteryLevelCheckBox.isChecked();
//                    break;
                case NetworkAvailable:
                    add = m_networkCheckBox.isChecked();
                    break;
                case NetworkWifiAvailable:
                    add = m_networkCheckBox.isChecked();
                    break;
                case GpsAvailable:
                    add = m_gpsCheckBox.isChecked();
                    break;
                default:
                    add = false;
                    break;
            }

            if (add) {
                m_stateLogListFragment.addToList(stateChangeData);
            }
        }
    }

    @Override
    public void historyChanged() {
        refresh();
    }
}
