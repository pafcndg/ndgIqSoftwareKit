package com.intel.wearable.platform.timeiq.refapp.developer.fragments.StateHistory;

import com.intel.wearable.platform.timeiq.refapp.developer.fragments.DeveloperDaoListFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.state.DeveloperHistoryManager;
import com.intel.wearable.platform.timeiq.userstate.audit.UserStateHistory;

public class DeveloperStateHistoryFragment extends DeveloperDaoListFragment<UserStateHistory> implements DeveloperHistoryManager.OnStateHistoryChangedListener {

    public DeveloperStateHistoryFragment() {
        super(UserStateHistory.class);
        DeveloperHistoryManager.registerForStateHistoryChangedListener(this);
    }

    @Override
    public void onDestroy() {
        DeveloperHistoryManager.unregisterStateForHistoryChangedListener(this);
        super.onDestroy();
    }

    @Override
    public void stateHistoryChanged() {
        refreshView();
    }
}
