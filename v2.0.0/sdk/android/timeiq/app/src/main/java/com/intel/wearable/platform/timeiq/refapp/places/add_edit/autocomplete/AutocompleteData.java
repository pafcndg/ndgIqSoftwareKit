package com.intel.wearable.platform.timeiq.refapp.places.add_edit.autocomplete;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by adura on 05/11/2015.
 */
public class AutocompleteData {
    public String mGooglePlacesId;
    public String mPlaceDescription;

    AutocompleteData(JSONObject jsonObject) {
        try {
            mGooglePlacesId = jsonObject.getString("place_id");
            mPlaceDescription = jsonObject.getString("description");

        } catch (JSONException e) {
            mGooglePlacesId = "NA";
            mPlaceDescription = "NA";
            e.printStackTrace();
        }
    }
}
