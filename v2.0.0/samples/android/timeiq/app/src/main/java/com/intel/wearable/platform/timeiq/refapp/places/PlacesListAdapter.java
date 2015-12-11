package com.intel.wearable.platform.timeiq.refapp.places;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.ArrayList;

/**
 * Created by smoradof on 10/12/2015.
 */
/*package*/ class PlacesListAdapter extends ArrayAdapter<PlacesWrapper> {

    private final ArrayList<PlacesWrapper> mPlacesArray;

    public PlacesListAdapter(Context context, int resource, ArrayList<PlacesWrapper> placesArray) {
        super(context, resource, placesArray);
        mPlacesArray = placesArray;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.item_place, null);
        }

        if (convertView != null) {
            convertView.setEnabled(true);
            PlacesWrapper placesWrapper = mPlacesArray.get(position);
            String placeAddress = placesWrapper.getPlaceAddress();
            String placeName = placesWrapper.getPlaceName(getContext());

            ((TextView) convertView.findViewById(R.id.place_name)).setText(placeName);
            if (placeAddress == null || placeAddress.isEmpty()) {
                convertView.findViewById(R.id.place_address).setVisibility(View.GONE);
            } else {
                convertView.findViewById(R.id.place_address).setVisibility(View.VISIBLE);
                ((TextView) convertView.findViewById(R.id.place_address)).setText(placeAddress);
            }


            int iconSrc = -1;
            switch (placesWrapper.getPlaceType()){
                case HOME:
                    iconSrc = R.drawable.list_home;
                    break;
                case WORK:
                    iconSrc = R.drawable.list_work;
                    break;
                case OTHER:
                    iconSrc = R.drawable.list_save_place;
                    break;
            }

            ((ImageView) convertView.findViewById(R.id.place_icon)).setImageResource(iconSrc);
        }

        return convertView;
    }
}
