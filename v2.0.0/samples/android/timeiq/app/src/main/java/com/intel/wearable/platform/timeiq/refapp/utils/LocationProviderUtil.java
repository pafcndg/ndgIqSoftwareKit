package com.intel.wearable.platform.timeiq.refapp.utils;

import android.content.Context;
import android.location.Location;
import android.location.LocationManager;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.result.ResultCode;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;

public class LocationProviderUtil {

    public static ResultData<TSOCoordinate> getCurrentLocation(Context context) {
        ResultData<TSOCoordinate> coordinateResultData;
        LocationManager locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        if(locationManager.isProviderEnabled(LocationManager.PASSIVE_PROVIDER)) {
            Location location = locationManager.getLastKnownLocation(LocationManager.PASSIVE_PROVIDER);
            if (location != null){
                coordinateResultData = new ResultData< >(
                        ResultCode.SUCCESS,
                        new TSOCoordinate(location.getLatitude(), location.getLongitude())
                );
            } else {
                coordinateResultData = new ResultData< >(
                        ResultCode.LOCATION_IS_NULL,
                        null
                );
            }
        } else {
            coordinateResultData = new ResultData< >(
                    ResultCode.GENERAL_ERROR,
                    "Location provider is not enabled in the Android settings",
                    null
            );
        }

        return coordinateResultData;
    }

}
