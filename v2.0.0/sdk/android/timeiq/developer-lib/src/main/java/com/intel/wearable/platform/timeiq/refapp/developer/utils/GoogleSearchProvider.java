package com.intel.wearable.platform.timeiq.refapp.developer.utils;


import android.content.Context;
import android.location.Address;
import android.location.Geocoder;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.result.ResultCode;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.developer.BuildConfig;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.net.URLEncoder;
import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

/**
 * Created by smoradof on 8/31/2014.
 */
public class GoogleSearchProvider {

    private static final String API_KEY = BuildConfig.GOOGLE_API_KEY;
    //public static final String TAG = CommonFactoryInitializer.TAG + GoogleSearchProvider.class.getName();


    public static ResultData<List<TSOPlace>> searchAddress(String address) {
        if (address == null || address.trim().length() <= 0) {
            return new ResultData<List<TSOPlace>>(ResultCode.GENERAL_ILLEGAL_PARAMETER_VALUE, null);
        }

        StringBuilder defaultQuery = new StringBuilder().
                /*append("https://maps.googleapis.com/maps/api/place/queryautocomplete/json?&key=").
                append(API_KEY).
                append("&input=").
                append(URLEncoder.encode(input));*/

                append("https://maps.googleapis.com/maps/api/geocode/json?address=\"").
                append(URLEncoder.encode(address)).
                append("\"&rankby=distance&sensor=true&key=").
                append(API_KEY);

        return baseSearch(defaultQuery.toString());
    }

    public static ResultData<List<TSOPlace>> searchNearby(String input, TSOPlace nearbyLocation, int searchRadius) {
        if (input == null || input.trim().length() <= 0 || nearbyLocation == null || searchRadius <= 0) {
            return new ResultData<List<TSOPlace>>(ResultCode.GENERAL_ILLEGAL_PARAMETER_VALUE, null);
        }

        StringBuilder defaultQuery = new StringBuilder().
                append("https://maps.googleapis.com/maps/api/place/nearbysearch/json?location=").
                append(nearbyLocation.getCoordinate().getLatitude()).
                append(",").
                append(nearbyLocation.getCoordinate().getLongitude()).
                append("&radius=").
                append(searchRadius).
                append("&name=\"").
                append(URLEncoder.encode(input)).
                append("\"&sensor=true&key=").
                append(API_KEY);

        return baseSearch(defaultQuery.toString());
    }

    private static ResultData<List<TSOPlace>> baseSearch(String url) {
        ResultData<List<TSOPlace>> resolvedLoc;
        try
        {
            Map<String,String> headers = new HashMap<String, String>();
            headers.put("Accept-Charset", "UTF-8");
            headers.put("Content-Type", "application/x-www-form-urlencoded;charset=utf-8");
            ResultData<String> response = ExternalHttpProviderTemp.httpGet(url, headers, Charset.forName("UTF8"));
            if (response != null && response.isSuccess() && response.getData().length() > 0)
            {
                JSONObject json = new JSONObject(response.getData());
                String jsonStatus = json.getString("status");
                if (jsonStatus.compareToIgnoreCase("OK") == 0)
                {
                    JSONArray results = json.getJSONArray("results");
                    int nNumOfResult = results.length();
                    if (nNumOfResult > 0)
                    {
                        List<TSOPlace> locations = new Vector<TSOPlace>();
                        for (int i = 0; i < nNumOfResult ; i++) {
                            JSONObject currentResult = results.getJSONObject(i);
                        // Get location
                            JSONObject jsonGeometry = currentResult.getJSONObject("geometry");
                            JSONObject jsonLocation = jsonGeometry.getJSONObject("location");
                        // Get name
                            JSONArray addressComponents = null;
                            String name = null;
                            if (currentResult.has("address_components")) {
                                addressComponents = currentResult.getJSONArray("address_components");
                                name = addressComponents.getJSONObject(0).getString("short_name");
                            } else if (currentResult.has("name")) {
                                name = currentResult.getString("name");
                            }
                        // get address
                            String formattedAddress = null;
                            if (currentResult.has("formatted_address")) {
                                formattedAddress = currentResult.getString("formatted_address");
                            } else if (currentResult.has("vicinity")) {
                                formattedAddress = currentResult.getString("vicinity");
                            }
                        // Create TSOPlace
                            TSOPlace locationAddress = new TSOPlace(jsonLocation.getDouble("lat"), jsonLocation.getDouble("lng"), name, formattedAddress);
                            locations.add(locationAddress);
                        }

                        resolvedLoc = new ResultData<List<TSOPlace>>(ResultCode.SUCCESS, locations);
                    } else {
                        resolvedLoc = new ResultData<List<TSOPlace>>(ResultCode.SEARCH_NO_RESULTS, null);
                    }
                } else if (jsonStatus.compareToIgnoreCase("ZERO_RESULTS") == 0) {
                    resolvedLoc = new ResultData<List<TSOPlace>>(ResultCode.SEARCH_NO_RESULTS, null);
                } else {
                    resolvedLoc = new ResultData<List<TSOPlace>>(ResultCode.GENERAL_SERVER_ERROR, null);
                }
            } else {
                resolvedLoc = new ResultData<List<TSOPlace>>(ResultCode.GENERAL_SERVER_ERROR, null);
            }
        } catch (Throwable t) {
            t.printStackTrace();
            System.out.println(t.toString());
            resolvedLoc = new ResultData<List<TSOPlace>>(ResultCode.GENERAL_EXCEPTION_WAS_THROWN, null);
        }

        return resolvedLoc;
    }

    /**
     * @param isWalking    walking or driving
     */
    public static ResultData<EtaData> getEtaData(TSOCoordinate origin, TSOCoordinate destination, boolean isWalking) {
        if (origin == null || destination == null) {
            return new ResultData<EtaData>(ResultCode.GENERAL_ILLEGAL_PARAMETER_VALUE, null);
        }

        String isWalkingText = isWalking ? "&mode=walking" : "";

        StringBuilder url = new StringBuilder().
                append("https://maps.googleapis.com/maps/api/distancematrix/json?origins=").
                append(origin.getLatitude()).append(",").append(origin.getLongitude()).
                append("&destinations=").
                append(destination.getLatitude()).append(",").append(destination.getLongitude()).
                append(isWalkingText).
                append("&sensor=true&key=").
                append(API_KEY);

        ResultData<EtaData> result;

        try
        {
            Map<String,String> headers = new HashMap<String, String>();
            headers.put("Accept-Charset", "UTF-8");
            headers.put("Content-Type", "application/x-www-form-urlencoded;charset=utf-8");
            ResultData<String> response = ExternalHttpProviderTemp.httpGet(url.toString(), headers, Charset.forName("UTF8"));
            if (response != null && response.isSuccess() && response.getData().length() > 0) {
                JSONObject json = new JSONObject(response.getData());
                String jsonStatus = json.getString("status");
                if (jsonStatus.compareToIgnoreCase("OK") == 0) {
                    JSONObject rows = json.getJSONArray("rows").getJSONObject(0);
                    JSONObject elements = rows.getJSONArray("elements").getJSONObject(0);
                    String status = elements.getString("status");
                    if ("ZERO_RESULTS".equals(status)) {
                        result = new ResultData<EtaData>(ResultCode.GENERAL_SERVER_ERROR, "No results", null);
                    } else {
                        JSONObject distance = elements.getJSONObject("distance");
                        String distanceText = distance.getString("text");
                        JSONObject duration = elements.getJSONObject("duration");
                        String durationText = duration.getString("text");

                        if (distanceText != null && !distanceText.trim().isEmpty() && durationText != null && !durationText.trim().isEmpty()) {
                            result = new ResultData<EtaData>(ResultCode.SUCCESS, new EtaData(distanceText, durationText));
                        } else {
                            result = new ResultData<EtaData>(ResultCode.GENERAL_NULL_ERROR, "Null data", null);
                        }
                    }
                } else {
                    result = new ResultData<EtaData>(ResultCode.GENERAL_SERVER_ERROR, "Status not OK", null);
                }
            } else {
                result = new ResultData<EtaData>(ResultCode.GENERAL_SERVER_ERROR, "Response null or failed", null);
            }
        } catch (Throwable t) {
            t.printStackTrace();
            System.out.println(t.toString());
            result = new ResultData<EtaData>(ResultCode.GENERAL_EXCEPTION_WAS_THROWN, null);
        }

        return result;
    }

    public static Address getRGCFromLocation(Context context, double lat, double lon) {
        Geocoder geoCoder = new Geocoder(context);
        List<Address> matches = null;
        try {
            matches = geoCoder.getFromLocation(lat, lon, 1);
        } catch (IOException e) {
            e.printStackTrace();
        }

        Address bestMatch = ((matches == null || matches.isEmpty()) ? null : matches.get(0));

        return bestMatch;
    }

    public static String getAddressString(Address address) {
        StringBuilder stringBuilder = new StringBuilder();
        if (address != null) {
            for (int i = 0; i <= address.getMaxAddressLineIndex(); i++) {
                String line = address.getAddressLine(i);
                if (line != null) {
                    if (i > 0) {
                        stringBuilder.append(", ");
                    }
                    stringBuilder.append(line);
                }
            }
        }

        return stringBuilder.toString();
    }

}
