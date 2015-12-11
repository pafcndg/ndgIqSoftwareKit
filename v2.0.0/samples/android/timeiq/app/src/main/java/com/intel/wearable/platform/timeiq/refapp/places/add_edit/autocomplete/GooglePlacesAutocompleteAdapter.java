package com.intel.wearable.platform.timeiq.refapp.places.add_edit.autocomplete;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.Filter;
import android.widget.Filterable;

import com.intel.wearable.platform.timeiq.refapp.BuildConfig;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;

/**
 * Created by adura on 04/11/2015.
 */
public class GooglePlacesAutocompleteAdapter extends ArrayAdapter<String> implements Filterable
{
    private static final String TAG = GooglePlacesAutocompleteAdapter.class.getSimpleName();

    private ArrayList<AutocompleteData> resultList;

    private static final String PLACES_API_BASE = "https://maps.googleapis.com/maps/api/place";
    private static final String TYPE_AUTOCOMPLETE = "/autocomplete";
    private static final String OUT_JSON = "/json";
    private static final String API_KEY = BuildConfig.GOOGLE_API_KEY;


    public GooglePlacesAutocompleteAdapter(Context context, int textViewResourceId) {
        super(context, textViewResourceId);
        resultList = new ArrayList<>();
    }


    public AutocompleteData getItemData(int index) {
        return resultList.get(index);
    }


    @Override
    public int getCount() {
        return resultList.size();
    }


    @Override
    public String getItem(int index) {
        AutocompleteData autocompleteData = resultList.get(index);
        if (TextUtils.isEmpty( autocompleteData.mPlaceDescription))
            return "";
        else
            return autocompleteData.mPlaceDescription;
    }


    @Override
    public Filter getFilter() {
        return new Filter() {
            @Override
            protected FilterResults performFiltering(CharSequence constraint) {
                FilterResults filterResults = new FilterResults();
                if (constraint != null) {
                    // Retrieve google autocomplete suggestions.
                    resultList = getAutocomplete(constraint.toString());

                    // Assign the data to the FilterResults
                    filterResults.values = resultList;
                    filterResults.count = resultList.size();
                }
                return filterResults;
            }

            @Override
            protected void publishResults(CharSequence constraint, FilterResults results) {
                if (results != null && results.count > 0) {
                    notifyDataSetChanged();
                } else {
                    notifyDataSetInvalidated();
                }
            }
        };
    }

    private static ArrayList<AutocompleteData> getAutocomplete(String input) {
        ArrayList<AutocompleteData> resultList = new ArrayList<>( );
        StringBuilder jsonResults = new StringBuilder();

        if (input != null && input.length() > 2 ) {
            HttpURLConnection conn = null;
            try {
                //              sb.append("&components=country:MY_COUNTRY_CODE");

                URL url = new URL(PLACES_API_BASE + TYPE_AUTOCOMPLETE + OUT_JSON + "?key=" + API_KEY + "&input=" + URLEncoder.encode(input, "utf8"));
                conn = (HttpURLConnection) url.openConnection();
                InputStreamReader in = new InputStreamReader(conn.getInputStream());

                // Load the results into a StringBuilder
                int read;
                char[] buff = new char[1024];
                while ((read = in.read(buff)) != -1) {
                    jsonResults.append(buff, 0, read);
                }
            } catch (MalformedURLException e) {
                Log.e(TAG, "Error processing Places API URL", e);
                return resultList;
            } catch (IOException e) {
                Log.e(TAG, "Error connecting to Places API", e);
                return resultList;
            } catch (Exception e) {
                Log.e(TAG, "Unknown Error ", e);
                return resultList;
            } finally {
                if (conn != null) {
                    conn.disconnect();
                }
            }
        }

        if (! TextUtils.isEmpty(jsonResults)) {
            try {
                // Create a JSON object hierarchy from the results
                JSONObject jsonObj = new JSONObject(jsonResults.toString());
                JSONArray predictionsJsonArray = jsonObj.getJSONArray("predictions");

                int resultLength = predictionsJsonArray.length();
                resultList = new ArrayList<>(resultLength);

                // Extract the Place descriptions from the results
                for (int i = 0; i < resultLength; i++) {
                    Log.d(TAG, predictionsJsonArray.getJSONObject(i).getString("description"));
                    resultList.add(new AutocompleteData(predictionsJsonArray.getJSONObject(i)));
                }
            } catch (JSONException e) {
                Log.e(TAG, "Cannot process JSON results", e);
            }
        }


        Log.d(TAG, "Auto Complete returns: " + resultList);
        return resultList;
    }

}
