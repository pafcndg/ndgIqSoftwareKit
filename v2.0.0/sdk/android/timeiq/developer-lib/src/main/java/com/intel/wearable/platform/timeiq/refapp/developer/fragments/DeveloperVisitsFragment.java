package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.dbobjects.places.visit.DBVisit;
import com.intel.wearable.platform.timeiq.refapp.developer.R;

public class DeveloperVisitsFragment extends DeveloperListWithMapFragment<DBVisit> {
    public DeveloperVisitsFragment() {
        super(DBVisit.class);
    }

    @Override
    public int getTitleResource() {
        return R.string.developer_fragment_visits_title;
    }

    @Override
    public TSOPlace getPlaceFromData(DBVisit dbVisit) {
        TSOCoordinate centerCoordinate = dbVisit.getCenterCoordinate();
        String start = DBVisit.convertTimeStampToDateString(dbVisit.getStartTime());
        String end = DBVisit.convertTimeStampToDateString(dbVisit.getEndTime());
        return new TSOPlace(centerCoordinate.getLatitude(), centerCoordinate.getLongitude(), start, end);
    }

}
