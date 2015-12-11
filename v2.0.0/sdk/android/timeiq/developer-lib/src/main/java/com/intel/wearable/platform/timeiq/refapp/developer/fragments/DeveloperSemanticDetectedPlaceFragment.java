package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;

import com.intel.wearable.platform.timeiq.api.dbobjects.places.semantic.SemanticKey;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.dbobjects.places.SemanticDetectedPlace;
import com.intel.wearable.platform.timeiq.dbobjects.places.cluster.Coord;
import com.intel.wearable.platform.timeiq.refapp.developer.R;

public class DeveloperSemanticDetectedPlaceFragment extends DeveloperListWithMapFragment<SemanticDetectedPlace> {
    public DeveloperSemanticDetectedPlaceFragment() {
        super(SemanticDetectedPlace.class);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        ListAdapter adapter = new ArrayAdapter<SemanticDetectedPlace>( getActivity().getApplicationContext(), R.layout.list_view, m_data ) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
//                if (convertView == null) {
//                    convertView = LayoutInflater.from(getContext()).inflate( R.layout.list_view, parent, false);
//                }
                convertView = super.getView(position, convertView, parent);
                SemanticDetectedPlace sdp = getItem(position);
                SemanticKey key = sdp.getSemanticKey();
                int bgColor = Color.WHITE;
                if (key.isAutodetectedHome()) {
                    bgColor = (getResources().getColor(R.color.detected_home_bg));
                } else if (key.isAutodetectedWork()) {
                    bgColor = (getResources().getColor(R.color.detected_work_bg));
                } else if (key.isHome()) {
                    bgColor = (getResources().getColor(R.color.home_bg));
                } else if (key.isWork()) {
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
        return R.string.developer_fragment_semantic_detected_places_title;
    }

    @Override
    public TSOPlace getPlaceFromData(SemanticDetectedPlace semanticDetectedPlace) {
        Coord center = semanticDetectedPlace.getCenter();
        SemanticKey semanticKey = semanticDetectedPlace.getSemanticKey();
        String address = semanticDetectedPlace.getAddress();
        return new TSOPlace(center.getLatitude(), center.getLongitude(), semanticKey.getIdentifier(), address);
    }

}
