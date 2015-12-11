package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.dbobjects.interfaces.ITSOSyncDbObject;

public interface IMapLocationFragment <T extends ITSOSyncDbObject> {

    public abstract int getTitleResource();
    public abstract TSOPlace getPlaceFromData(T data);

}
