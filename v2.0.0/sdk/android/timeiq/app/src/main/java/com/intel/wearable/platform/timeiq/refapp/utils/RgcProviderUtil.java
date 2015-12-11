package com.intel.wearable.platform.timeiq.refapp.utils;

import android.content.Context;
import android.location.Address;
import android.location.Geocoder;

import java.io.IOException;
import java.util.List;

public abstract class RgcProviderUtil {

    public static Address getRGCFromLocation(Context context, double lat, double lon) {
        Geocoder geoCoder = new Geocoder(context);
        List<Address> matches = null;
        try {
            matches = geoCoder.getFromLocation(lat, lon, 1);
        } catch (IOException e) {
            e.printStackTrace();
        }

        return (matches == null || matches.isEmpty()) ? null : matches.get(0);
    }

    public static String getAddressString(Address address) {
        String addressString = null;
        if (address != null) {
            StringBuilder stringBuilder = new StringBuilder();
            for (int i = 0; i <= address.getMaxAddressLineIndex(); i++) {
                String line = address.getAddressLine(i);
                if (line != null) {
                    if (i > 0) {
                        stringBuilder.append(", ");
                    }
                    stringBuilder.append(line);
                }
            }

            addressString = stringBuilder.toString();
        }

        return addressString;
    }

}
