package com.intel.wearable.platform.timeiq.refapp.apitoolbox;

import android.content.Context;
import android.location.Address;
import android.text.TextUtils;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.common.result.Result;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.api.dbobjects.places.semantic.SemanticKey;
import com.intel.wearable.platform.timeiq.api.places.IPlaceRepo;
import com.intel.wearable.platform.timeiq.api.places.TSOPlaceBuilder;
import com.intel.wearable.platform.timeiq.api.places.datatypes.GeographicData;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.TimeIQBGService;
import com.intel.wearable.platform.timeiq.refapp.googleAnalytics.GoogleAnalyticsTrackers;
import com.intel.wearable.platform.timeiq.refapp.places.PlaceType;
import com.intel.wearable.platform.timeiq.refapp.places.PlacesWrapper;
import com.intel.wearable.platform.timeiq.refapp.utils.RgcProviderUtil;

import java.util.ArrayList;
import java.util.Collection;

/**
 * Created by smoradof on 10/28/2015.
 */
public final class TimeIQPlacesUtils {


    /**
     * Return a List of places currently save at the repository
     * @return a collection of {@link TSOPlace } currently at the repository, null on error
     */
    public static Collection<TSOPlace> getAllPlaces(){
        Collection<TSOPlace> placesList = null;
        IPlaceRepo placesRepo = TimeIQBGService.mTimeIQApi.getPlacesRepo();
        ResultData<Collection<TSOPlace>> placesListResult = placesRepo.getAllPlaces();
        if(placesListResult.isSuccess()) {
            placesList = placesListResult.getData();
        }
        return placesList;
    }

    /**
     *
     * @return a list of {@link PlacesWrapper}, which contains all current places at the repository.
     */
    public static ArrayList<PlacesWrapper> getAllPlacesIncludingHomeAndWork(){
        ArrayList<PlacesWrapper> placesWrappers = new ArrayList<>();
        IPlaceRepo placesRepo = TimeIQBGService.mTimeIQApi.getPlacesRepo();

        TSOPlace workPlace = null;
        ResultData<PlaceID> workResult = placesRepo.getPlaceId(SemanticKey.WORK);
        if(workResult.isSuccess()){
            ResultData<TSOPlace>  res = placesRepo.getPlace(workResult.getData());
            if (res.isSuccess()) {
                workPlace = res.getData();
            }
        }
        if (workPlace == null) {
            workResult = placesRepo.getPlaceId(SemanticKey.AUTODETECTED_WORK);
            if(workResult.isSuccess()){
                ResultData<TSOPlace>  res = placesRepo.getPlace(workResult.getData());
                if (res.isSuccess()) {
                    workPlace = res.getData();
                }
            }
        }
        PlacesWrapper work = new PlacesWrapper();
        work.setPlace(workPlace);
        work.setPlaceType(PlaceType.WORK);
        placesWrappers.add(0, work);

        TSOPlace homePlace = null;
        ResultData<PlaceID> homeResult = placesRepo.getPlaceId(SemanticKey.HOME);
        if(homeResult.isSuccess()){
            ResultData<TSOPlace> res = placesRepo.getPlace(homeResult.getData());
            if (res.isSuccess()) {
                homePlace = res.getData();
            }
        }
        if (homePlace == null) {
            homeResult = placesRepo.getPlaceId(SemanticKey.AUTODETECTED_HOME);
            if(homeResult.isSuccess()){
                ResultData<TSOPlace> res = placesRepo.getPlace(homeResult.getData());
                if (res.isSuccess()) {
                    homePlace = res.getData();
                }
            }
        }
        PlacesWrapper home = new PlacesWrapper();
        home.setPlace(homePlace);
        home.setPlaceType(PlaceType.HOME);
        placesWrappers.add(0, home);

        ResultData<Collection<TSOPlace>> placesListResult = placesRepo.getAllPlaces();
        if(placesListResult.isSuccess()) {
            Collection<TSOPlace> placesList = placesListResult.getData();
            for (TSOPlace place : placesList) {
                if (place != null) {
                    SemanticKey semanticKey = place.getSemanticKey();
                    if (semanticKey != null && !semanticKey.isHome() && !semanticKey.isWork()) {
                        PlacesWrapper placesWrapper = new PlacesWrapper();
                        placesWrapper.setPlace(place);
                        PlaceType placeType = PlaceType.OTHER;
                        if(semanticKey.isAutodetectedHome() || semanticKey.isHome()){
                            placeType = PlaceType.HOME;
                        }
                        else if(semanticKey.isAutodetectedWork() || semanticKey.isWork()){
                            placeType = PlaceType.WORK;
                        }
                        placesWrapper.setPlaceType(placeType);
                        placesWrappers.add(placesWrapper);
                    }
                }
            }
        }
        return placesWrappers;
    }

    /**
     * Save place to the repository
     * @param context           the Context to use for accessing resources
     * @param placeType         the type of the place({@link PlaceType#HOME} / {@link PlaceType#WORK} or {@link PlaceType#OTHER})
     * @param placeAddress      place address
     * @param placeName         place name
     * @param placeCoordinate   place Coordinate
     * @return                  ResultObject with success or the error message on failure.
     *                          The {@link PlaceID} returned is either the {@link PlaceID} of the new place.
     */
    public static ResultObject<PlaceID> savePlace(Context context, PlaceType placeType, String placeAddress, String placeName, TSOCoordinate placeCoordinate) {
        ResultObject<PlaceID> resultData;
        if (TextUtils.isEmpty(placeAddress)) {
            resultData = new ResultObject<>(false, context.getString(R.string.toast_no_valid_address), null);
        } else if(TextUtils.isEmpty(placeName)){
            resultData = new ResultObject< >(false, context.getString(R.string.toast_no_valid_name), null);
        } else if(placeCoordinate == null){
            resultData = new ResultObject< >(false, context.getString(R.string.toast_no_valid_coordinate), null);
        } else {
            TSOPlace place = new TSOPlace(placeName, placeAddress, placeCoordinate);
            // get the semantic key: home work or the old key and null if no semantic key
            SemanticKey semanticKey;
            if(placeType.equals(PlaceType.OTHER)) {
                PlaceID placeId = place.getPlaceId();
                if(placeId != null){
                    semanticKey = placeId.getSemanticKey();
                } else {
                    semanticKey = null;
                }
            } else{
                semanticKey = placeType.equals(PlaceType.HOME) ? SemanticKey.HOME : SemanticKey.WORK;
            }

            IPlaceRepo placesRepo = TimeIQBGService.mTimeIQApi.getPlacesRepo();
            ResultData<PlaceID> placeIDResultData = semanticKey == null ? placesRepo.addPlace(place) : placesRepo.addPlace(place, semanticKey);
            resultData = new ResultObject<>(placeIDResultData.isSuccess(), placeIDResultData.getMessage(), placeIDResultData.getData());
        }

        return resultData;
    }

    /**
     * Edit and existing place
     * @param context           the Context to use for accessing resources
     * @param placeId           the place ID of the {@link PlaceID} to be edited
     * @param placeAddress      place address
     * @param placeName         place name
     * @param placeCoordinate   place Coordinate
     * @return                  ResultObject with success or the error message on failure.
     */
    public static ResultObject<Boolean> editPlace(Context context, PlaceID placeId, String placeAddress, String placeName, TSOCoordinate placeCoordinate) {
        boolean success;
        String errorMsg;
        if (placeId != null) {
            IPlaceRepo placesRepo = TimeIQBGService.mTimeIQApi.getPlacesRepo();
            ResultData<TSOPlace> placeResultData = placesRepo.getPlace(placeId);
            if (placeResultData != null && placeResultData.isSuccess()) {
                TSOPlace place = placeResultData.getData();

            // Create the new place
                TSOPlaceBuilder placeBuilder = new TSOPlaceBuilder(place); // create with the original place as data
                SemanticKey semanticKey = placeId.getSemanticKey();
                place = placeBuilder.setAddressData(placeAddress).setName(placeName).setGeoData(new GeographicData(placeCoordinate)).build();

            // Remove the old place
                Result result = placesRepo.removePlace(placeId);
                boolean couldNotRemoveOld = !result.isSuccess();

            // Add the new place
                ResultData<PlaceID> placeIDResultData = semanticKey == null ? placesRepo.addPlace(place) : placesRepo.addPlace(place, semanticKey); // TODO - change to only placesRepo.addPlace(place) once Gil fixes a bug in generateSemanticKey
                if (placeIDResultData.isSuccess()) {
                    success = true;
                    if (couldNotRemoveOld) {
                        errorMsg = context.getString(R.string.toast_location_saved_but_old_not_removed);
                    } else {
                        errorMsg = context.getString(R.string.toast_location_saved);
                    }
                } else {
                    success = false;
                    errorMsg = placeIDResultData.getMessage();
                    errorMsg = context.getString(R.string.toast_could_not_add_place, errorMsg != null ? errorMsg : "");
                }
            } else {
                success = false;
                errorMsg = context.getString(R.string.toast_place_not_found);
            }
        } else {
            success = false;
            errorMsg = context.getString(R.string.toast_place_id_is_null);
        }

        return new ResultObject<>(success, errorMsg, success);
    }

    /**
     * Delete a place from the places repository
     * @param placeID target place id to be deleted
     * @return true on success, else false
     */
    public static boolean deletePlace(PlaceID placeID){
        boolean success = false;
        if(placeID != null) {
            IPlaceRepo placesRepo = TimeIQBGService.mTimeIQApi.getPlacesRepo();
            Result result = placesRepo.removePlace(placeID);
            success = result.isSuccess();
        }
        return success;
    }

    /**
     * Returns a {@link TSOPlace} from the given place ID
     * @param placeId id for the place
     * @return The TSOPlace data associated with the place ID
     */
    public static TSOPlace getPlace(PlaceID placeId){
        TSOPlace place = null;
        IPlaceRepo placesRepo = TimeIQBGService.mTimeIQApi.getPlacesRepo();
        ResultData<TSOPlace> placeResultData = placesRepo.getPlace(placeId);
        if(placeResultData != null && placeResultData.isSuccess()){
            place = placeResultData.getData();
        }
        return place;
    }

    /**
     * Two options for auto detection: work and home
     */
    public enum PlaceToAutoDetect {
        WORK,
        HOME
    }

    /**
     * Try and set the auto detected place if no place exists already
     * @param placeToAutoDetect    the type of place to set ({@link PlaceToAutoDetect#WORK} or {@link PlaceToAutoDetect#HOME})
     * @return                     true if there was no place set and the auto detected place was available and set
     */
    public static boolean setPlaceFromAutoDetectedIfNotSet(Context context, PlaceToAutoDetect placeToAutoDetect) {
        boolean autoDetectedWasSet = false;
        if (placeToAutoDetect != null) {
            SemanticKey setPlaceKey = null, detectedPlaceKey = null;
            PlaceType placeType = null;
            String name = null;
            switch (placeToAutoDetect) {
                case WORK:
                    setPlaceKey = SemanticKey.WORK;
                    detectedPlaceKey = SemanticKey.AUTODETECTED_WORK;
                    placeType = PlaceType.WORK;
                    name = context.getString(R.string.work);
                    break;
                case HOME:
                    setPlaceKey = SemanticKey.HOME;
                    detectedPlaceKey = SemanticKey.AUTODETECTED_HOME;
                    placeType = PlaceType.HOME;
                    name = context.getString(R.string.home);
                    break;
            }

            TSOPlace place = TimeIQPlacesUtils.getPlaceFromAutoDetectedIfNotSet(setPlaceKey, detectedPlaceKey);
            if (place != null) {
                String addressString = place.getAddress();
                if (addressString == null) {
                    TSOCoordinate coordinate = place.getCoordinate();
                    Address address = RgcProviderUtil.getRGCFromLocation(context, coordinate.getLatitude(), coordinate.getLongitude());
                    if (address != null) {
                        addressString = RgcProviderUtil.getAddressString(address);
                        GoogleAnalyticsTrackers.getInstance().trackEvent(R.string.google_analytics_place, R.string.google_analytics_add_auto_detected, placeToAutoDetect.name());
                        autoDetectedWasSet = savePlace(context, placeType, addressString, name, place.getCoordinate()).isSuccess();
                    }
                }
            }
        }

        return autoDetectedWasSet;
    }

    private static TSOPlace getPlaceFromAutoDetectedIfNotSet(SemanticKey setPlaceKey, SemanticKey detectedPlaceKey) {
        TSOPlace autoDetectedPlace = null;
        IPlaceRepo placesRepo = TimeIQBGService.mTimeIQApi.getPlacesRepo();
        ResultData<PlaceID> placeIDResultData = placesRepo.getPlaceId(setPlaceKey);
        if (!placeIDResultData.isSuccess()) {
            placeIDResultData = placesRepo.getPlaceId(detectedPlaceKey);
            if (placeIDResultData.isSuccess()) {
                ResultData<TSOPlace> placeResultData = placesRepo.getPlace(placeIDResultData.getData());
                if (placeResultData.isSuccess()) {
                    autoDetectedPlace = placeResultData.getData();
                }
            }
        }

        return autoDetectedPlace;
    }

}
