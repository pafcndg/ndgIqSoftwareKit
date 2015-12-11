package com.intel.wearable.platform.timeiq.refapp.places;


import android.content.Context;

import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.R;

/**
 * Created by smoradof on 10/18/2015.
 */
public class PlacesWrapper {

    private TSOPlace place;

    private PlaceType placeType = PlaceType.OTHER;

    public TSOPlace getPlace() {
        return place;
    }

    public void setPlace(TSOPlace place) {
        this.place = place;
    }



    public String getPlaceName(Context context){
        String placeName = null;

        switch (placeType) {
            case HOME:
                placeName = context.getString(R.string.home);
                break;
            case WORK:
                placeName = context.getString(R.string.work);
                break;
            case OTHER:
                if(place != null ){
                    placeName = place.getName();
                }
                break;
        }


        return placeName;
    }

    public String getPlaceAddress(){
        if(place != null){
            return place.getAddress();
        }
        return null;
    }

    public PlaceType getPlaceType() {
        return placeType;
    }

    public void setPlaceType(PlaceType placeType) {
        this.placeType = placeType;
    }
}
