package com.intel.wearable.platform.timeiq.refapp.places.add_edit;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import com.google.gson.Gson;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQPlacesUtils;
import com.intel.wearable.platform.timeiq.refapp.reminders.add_edit.FragmentEditBe;

import java.util.ArrayList;
import java.util.Collection;

public class FragmentPlacePicker extends Fragment {

    private ListView mPlacesListView;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_place_picker, container, false);
        final Activity activity = getActivity();

        final ArrayList<TSOPlace> placesArray = new ArrayList<>();
        Collection<TSOPlace> allPlaces = TimeIQPlacesUtils.getAllPlaces();
        if(allPlaces != null && !allPlaces.isEmpty()){
            placesArray.addAll(allPlaces);
        }

        PlacesPickerAdapter placesListAdapter = new PlacesPickerAdapter(activity, R.id.placePickerListView, placesArray);
        mPlacesListView = (ListView) rootView.findViewById(R.id.placePickerListView);
        mPlacesListView.setAdapter(placesListAdapter);

        mPlacesListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                TSOPlace place = placesArray.get(position);
                Intent intent = activity.getIntent();
                if (place != null) {
                    PlaceID placeId = place.getPlaceId();
                    if (placeId != null) {
                        String jsonString = new Gson().toJson(placeId);
                        intent.putExtra(FragmentEditBe.SELECT_PLACE_ID_DATA_KEY, jsonString);
                        activity.setResult(Activity.RESULT_OK, intent);
                        activity.finish();
                    }
                }
            }
        });

        TextView emptyListMsg = (TextView) rootView.findViewById(R.id.emptyListText);
        if (placesArray.isEmpty()) {
            emptyListMsg.setVisibility(View.VISIBLE);
        } else {
            emptyListMsg.setVisibility(View.GONE);
        }

        return rootView;
    }

    public void setListVisibility(boolean isVisible) {
        mPlacesListView.setVisibility(isVisible ? View.VISIBLE : View.GONE);
    }

}
