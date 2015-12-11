package com.intel.wearable.platform.timeiq.refapp.developer.fragments.state;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.intel.wearable.platform.timeiq.refapp.developer.R;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.SimpleListFragment;

/**
 * Created by mleib on 21/07/2015.
 */
public class StateLogListFragment extends SimpleListFragment<StateChangeData> {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = super.onCreateView(inflater, container, savedInstanceState);

        StateLogListArrayAdapter stateLogListArrayAdapter = new StateLogListArrayAdapter(getActivity().getApplicationContext(), R.layout.list_view, m_entryList);
        setAdapter(stateLogListArrayAdapter);

        return view;
    }

}
