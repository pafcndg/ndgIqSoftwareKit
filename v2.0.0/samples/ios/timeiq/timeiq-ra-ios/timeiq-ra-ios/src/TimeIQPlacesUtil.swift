//
//  TimeIQPlacesUtil.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 09/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation
import TimeIQ

class TimeIQPlacesUtil {
    
    /**
     * Return a List of places currently save at the repository
     * @return a collection of {@link TSOPlace } currently at the repository, null on error
     */
    class func getAllPlaces() -> Array<TSOPlace>
    {
        var placesList = Array<TSOPlace>();
        let placesRepo = TimeIQBGService.sharedInstance.timeIQApi?.getPlacesRepo();
        let resultData = placesRepo?.getAllPlaces();
        
        if (resultData!.isSuccess())
        {
            placesList = resultData?.getData() as! Array<TSOPlace>;
        }
        
        return placesList;
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
    class func savePlace(placeId: PlaceID?, placeType: PlaceType, placeAddress: String,
        placeName: String, placeCoordinate: TSOCoordinate?) ->  (success: Bool, placeId: PlaceID?, message: String) {
            if (placeAddress.isEmpty) {
                return (false, nil, NSLocalizedString("toast_no_valid_address", comment: ""));
            } else if(placeName.isEmpty){
                return (false, nil, NSLocalizedString("toast_no_valid_name", comment: ""));
            } else if(placeCoordinate == nil){
                return (false, nil, NSLocalizedString("toast_no_valid_coordinate", comment: ""));
            } else {
                let place = TSOPlace(NSString: placeName, withNSString: placeAddress, withTSOCoordinate: placeCoordinate);
                // get the semantic key: home work or the old key and null if no semantic key
                let semanticKey: SemanticKey?
                
                switch (placeType) {
                case .PlaceType_OTHER:
                    let placeId = place.getPlaceId();
                    if(placeId != nil){
                        semanticKey = placeId.getSemanticKey();
                    } else {
                        semanticKey = nil;
                    }
                    break;
                case .PlaceType_HOME:
                    semanticKey = SemanticKey_HOME_
                    break;
                case .PlaceType_WORK:
                    semanticKey = SemanticKey_WORK_
                    break;
                }
                
                let placesRepo = TimeIQBGService.sharedInstance.timeIQApi!.getPlacesRepo();
                let placeIDResultData: ResultData?
                if (semanticKey == nil)
                {
                    placeIDResultData = placesRepo.addPlaceWithTSOPlace(place)
                }
                else
                {
                    placeIDResultData = placesRepo.addPlaceWithTSOPlace(place, withSemanticKey: semanticKey)
                }
                return (placeIDResultData!.isSuccess(), placeIDResultData!.getData() as? PlaceID, placeIDResultData!.getMessage())
            }
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
    class func editPlace(placeId: PlaceID?, placeAddress: String, placeName: String, placeCoordinate: TSOCoordinate) ->  (success: Bool, message: String) {
        var success = false
        var errorMsg = ""
        if (placeId != nil) {
            let placesRepo = TimeIQBGService.sharedInstance.timeIQApi!.getPlacesRepo();
            let placeResultData = placesRepo.getPlaceWithPlaceID(placeId);
            if (placeResultData != nil && placeResultData.isSuccess()) {
                var place = placeResultData.getData() as! TSOPlace;
                
                // Create the new place
                let placeBuilder = TSOPlaceBuilder(TSOPlace: place); // create with the original place as data
                let semanticKey = placeId?.getSemanticKey();
                place = placeBuilder.setAddressDataWithNSString(placeAddress).setNameWithNSString(placeName).setGeoDataWithGeographicData(GeographicData(TSOCoordinate: placeCoordinate)).build();
                
                // Remove the old place
                let result = placesRepo.removePlaceWithPlaceID(placeId);
                let couldNotRemoveOld = !result.isSuccess();
                
                // Add the new place
                let placeIDResultData = semanticKey == nil ? placesRepo.addPlaceWithTSOPlace(place) : placesRepo.addPlaceWithTSOPlace(place, withSemanticKey: semanticKey); // TODO - change to only placesRepo.addPlace(place) once Gil fixes a bug in generateSemanticKey
                if (placeIDResultData.isSuccess()) {
                    success = true;
                    if (couldNotRemoveOld) {
                        errorMsg = NSLocalizedString("toast_location_saved_but_old_not_removed", comment: "")
                    } else {
                        errorMsg = NSLocalizedString("toast_location_saved", comment: "")
                    }
                } else {
                    success = false;
                    errorMsg = placeIDResultData.getMessage();
                    errorMsg = errorMsg.isEmpty ? "" : NSLocalizedString("toast_could_not_add_place", comment: "");
                }
            } else {
                success = false;
                errorMsg = NSLocalizedString("toast_place_not_found", comment: "")
            }
        } else {
            success = false;
            errorMsg = NSLocalizedString("toast_place_id_is_null", comment: "")
        }
        
        return (success, errorMsg);
    }
    
    /**
     * Delete a place from the places repository
     * @param placeID target place id to be deleted
     * @return true on success, else false
     */
    class func deletePlace(placeID: PlaceID?) -> Bool{
        var success = false
        if(placeID != nil) {
            let result = TimeIQBGService.sharedInstance.timeIQApi!.getPlacesRepo().removePlaceWithPlaceID(placeID);
            success = result.isSuccess();
        }
        return success;
    }
    
    /**
     * Returns a {@link TSOPlace} from the given place ID
     * @param placeId id for the place
     * @return The TSOPlace data associated with the place ID
     */
    class func getPlace(placeId: PlaceID) -> TSOPlace {
        var place: TSOPlace?
        let placeResultData = TimeIQBGService.sharedInstance.timeIQApi!.getPlacesRepo().getPlaceWithPlaceID(placeId);
        if (placeResultData != nil && placeResultData.isSuccess()){
            place = placeResultData.getData() as? TSOPlace;
        }
        return place!;
    }
}