package com.intel.wearable.platform.timeiq.refapp.places.add_edit.autocomplete;

import com.intel.wearable.platform.timeiq.api.resolver.ResolvedLocation;

public interface IResolvedLocationListener {

    void onSearchTermChanged(String text);
    void onPlacePicked(String name);
    void onResolvedLocation(ResolvedLocation resolvedLocation);
    void onResolvedLocationError(String error);

}
