package com.intel.wearable.platform.timeiq.refapp.developer.utils;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.SourceDataType;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.dbobjects.places.semantic.SemanticKey;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.places.repo.IPlaceRepoModule;

/**
 * Created by mleib on 22/07/2015.
 */
public abstract class PlaceToStringUtil {

    /**
     * @return the place description if matches a known type or the default name supplied if no such match is found
     */
    public static String getPlaceDescription(PlaceID placeId, String defaultName) {
        String placeDescription = defaultName;
        if (placeId != null) {
            IPlaceRepoModule placesRepo = ClassFactory.getInstance().resolve(IPlaceRepoModule.class);
            ResultData<TSOPlace> res = placesRepo.getPlace(placeId);
            if (res.isSuccess()) {
                placeDescription = getPlaceDescription(res.getData(), defaultName);
            }
        }
        return placeDescription;
    }

    public static String getPlaceDescription(TSOPlace place, String defaultName) {
        String placeDescription = null;
        //try to get description from semantic data
        String name = place.getName();
        if (name != null && name.trim().length() > 0) {
            placeDescription = String.format("%s (%s)", name, place.getPlaceId().getSourceDataType().toString()); // name;
        } else {
            final SemanticKey semanticKey = place.getSemanticKey();
            if (semanticKey != null) {
                if (semanticKey.equals(SemanticKey.HOME)) {
                    placeDescription = String.format("HOME (%s - %s)", place.getPlaceId().getSourceDataType().toString(), place.getPlaceId().getIdentifier()); //"HOME";
                } else if (semanticKey.equals(SemanticKey.WORK)) {
                    placeDescription = String.format("WORK (%s - %s)", place.getPlaceId().getSourceDataType().toString(), place.getPlaceId().getIdentifier()); //"WORK";
                } else {
                    placeDescription = String.format("SEMANTIC (%s - %s)", place.getPlaceId().getSourceDataType().toString(), place.getPlaceId().getIdentifier());

                }
            }
        }
        if (placeDescription == null) {
            if (isDetectedSourceDataType(place)) {
                placeDescription = String.format("DETECTED (%s - %s)",place.getPlaceId().getSourceDataType().toString(), place.getPlaceId().getIdentifier());
            }
            else {
                placeDescription = String.format("MANUAL (%s - %s)",place.getPlaceId().getSourceDataType().toString(), place.getPlaceId().getIdentifier());
            }
        }

        if (placeDescription == null) {
            placeDescription = defaultName;
        }

        return placeDescription;
    }

    private static boolean isDetectedSourceDataType(TSOPlace place) {
        boolean detected = false;
        if (place != null) {
            PlaceID placeID = place.getPlaceId();
            if (placeID != null) {
                detected = (placeID.getSourceDataType() == SourceDataType.DETECTED) || (placeID.getSourceDataType() == SourceDataType.SEMANTIC_DETECTED);
            }
        }
        return detected;
    }

}
