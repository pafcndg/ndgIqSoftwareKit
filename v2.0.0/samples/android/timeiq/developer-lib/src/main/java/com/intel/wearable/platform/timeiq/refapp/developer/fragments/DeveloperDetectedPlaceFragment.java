package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;

import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.dbobjects.places.cluster.Coord;
import com.intel.wearable.platform.timeiq.dbobjects.places.cluster.DetectedPlace;
import com.intel.wearable.platform.timeiq.dbobjects.places.cluster.SemanticTag;
import com.intel.wearable.platform.timeiq.refapp.developer.R;

public class DeveloperDetectedPlaceFragment extends DeveloperListWithMapFragment<DetectedPlace> {
    public DeveloperDetectedPlaceFragment() {
        super(DetectedPlace.class);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        ListAdapter adapter = new ArrayAdapter<DetectedPlace>( getActivity().getApplicationContext(), R.layout.list_view, m_data ) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                convertView = super.getView(position, convertView, parent);
                DetectedPlace dp = getItem(position);
                int bgColor = Color.WHITE;
                if (dp.getSemanticTag().equals(SemanticTag.PLACE_SEMATIC_HOME)) {
                    bgColor = (getResources().getColor(R.color.detected_home_bg));
                } else if (dp.getSemanticTag().equals(SemanticTag.PLACE_SEMATIC_WORK)) {
                    bgColor = (getResources().getColor(R.color.detected_work_bg));
                }

                convertView.setBackgroundColor(bgColor);
                return convertView;
            }
        };

        setListAdapter(adapter);
    }

    @Override
    public int getTitleResource() {
        return R.string.developer_fragment_detected_places_title;
    }

    @Override
    public TSOPlace getPlaceFromData(DetectedPlace detectedPlace) {
        Coord center = detectedPlace.getCenter();
        SemanticTag semanticTag = detectedPlace.getSemanticTag();
        String address = detectedPlace.getAddress();
        return new TSOPlace(center.getLatitude(), center.getLongitude(), semanticTag.name(), address);
    }

}
