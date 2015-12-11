package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;

import com.intel.wearable.platform.timeiq.api.dbobjects.places.semantic.SemanticKey;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.dbobjects.places.ManualPlace;
import com.intel.wearable.platform.timeiq.dbobjects.places.cluster.Coord;
import com.intel.wearable.platform.timeiq.refapp.developer.R;

public class DeveloperManualPlaceFragment extends DeveloperListWithMapFragment<ManualPlace> {
    public DeveloperManualPlaceFragment() {
        super(ManualPlace.class);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        ListAdapter adapter = new ArrayAdapter<ManualPlace>(getActivity().getApplicationContext(), R.layout.list_view, m_data) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
//                if (convertView == null) {
//                    convertView = LayoutInflater.from(getContext()).inflate( R.layout.list_view, parent, false);
//                }
                convertView = super.getView(position, convertView, parent);
                ManualPlace mp = getItem(position);
                int bgColor = Color.WHITE;
                if (mp.isHome()) {
                    bgColor = (getResources().getColor(R.color.home_bg));
                } else if (mp.isWork()) {
                    bgColor = (getResources().getColor(R.color.work_bg));
                }

                convertView.setBackgroundColor(bgColor);
                return convertView;
            }
        };

        setListAdapter(adapter);
    }

    @Override
    public int getTitleResource() {
        return R.string.developer_fragment_manual_places_title;
    }

    @Override
    public TSOPlace getPlaceFromData(ManualPlace manualPlace) {
        Coord center = manualPlace.getCenter();
        SemanticKey semanticKey = manualPlace.getSemanticKey();
        String address = manualPlace.getAddress();
        return new TSOPlace(center.getLatitude(), center.getLongitude(), semanticKey.getIdentifier(), address);
    }

}
