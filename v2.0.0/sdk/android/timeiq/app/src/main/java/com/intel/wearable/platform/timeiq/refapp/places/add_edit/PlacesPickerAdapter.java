package com.intel.wearable.platform.timeiq.refapp.places.add_edit;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.api.dbobjects.places.semantic.SemanticKey;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.ArrayList;

/**
 * Created by smoradof on 10/21/2015.
 */
/*package*/ class PlacesPickerAdapter extends ArrayAdapter<TSOPlace> {
    private final ArrayList<TSOPlace> mPlaces;

    public PlacesPickerAdapter(Context context, int res, ArrayList<TSOPlace> places) {
        super(context, res, places);
        mPlaces = places;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.item_place, null);
        }

        if (convertView != null) {
            TSOPlace tsoPlace = mPlaces.get(position);
            String placeName = tsoPlace.getName();
            String placeAddress = tsoPlace.getAddress();
            ((TextView) convertView.findViewById(R.id.place_name)).setText(placeName);
            if (placeAddress == null || placeAddress.isEmpty()) {
                convertView.findViewById(R.id.place_address).setVisibility(View.GONE);
            } else {
                convertView.findViewById(R.id.place_address).setVisibility(View.VISIBLE);
                ((TextView) convertView.findViewById(R.id.place_address)).setText(placeAddress);
            }

            ImageView icon = (ImageView) convertView.findViewById(R.id.place_icon);
            SemanticKey semanticKey = tsoPlace.getSemanticKey();
            if(semanticKey.isWork() || semanticKey.isAutodetectedWork()){
                icon.setImageResource(R.drawable.list_work);
            }
            else if(semanticKey.isHome() || semanticKey.isAutodetectedHome()){
                icon.setImageResource(R.drawable.list_home);
            }

        }
        return convertView;
    }

}
