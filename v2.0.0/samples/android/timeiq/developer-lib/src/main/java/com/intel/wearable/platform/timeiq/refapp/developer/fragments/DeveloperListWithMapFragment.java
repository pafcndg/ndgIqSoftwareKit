package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.widget.ListView;

import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.common.externallibs.IJSONUtils;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.dbobjects.interfaces.ITSOSyncDbObject;
import com.intel.wearable.platform.timeiq.refapp.developer.R;
import com.intel.wearable.platform.timeiq.refapp.developer.activities.MapActivity;

import java.util.ArrayList;
import java.util.Map;

/**
 * Created by mleib on 15/11/2015.
 */
public abstract class DeveloperListWithMapFragment<T extends ITSOSyncDbObject> extends DeveloperDaoListFragment<T> implements IMapLocationFragment<T> {

    private IJSONUtils mJsonUtils;

    protected DeveloperListWithMapFragment(Class<T> typeParameterClass) {
        super(typeParameterClass);
        mJsonUtils = ClassFactory.getInstance().resolve(IJSONUtils.class);
    }

    @Override
    public void onListItemClick(ListView listView, View view, int position, long id) {
        super.onListItemClick(listView, view, position, id);
        Context context = getActivity();
        Intent mapActivity = new Intent(context, MapActivity.class);

        String title = getString(R.string.developer_map_title_prefix, getString(getTitleResource()).toLowerCase());
        mapActivity.putExtra(MapActivity.INPUT_STRING_TITLE, title);

        TSOPlace selectedPlace = getPlaceFromData(m_data.get(position));
        Map<String, Object> selectedPlaceStringObjectMap = selectedPlace.objectToMap();
        String selectedPlaceAsString = mJsonUtils.toJson(selectedPlaceStringObjectMap);
        mapActivity.putExtra(MapActivity.INPUT_STRING_SELECTED_LOCATION, selectedPlaceAsString);

        ArrayList<TSOPlace> allLocations = getAllLocations();
        String allLocationsAsString = mJsonUtils.toJson(allLocations);
        mapActivity.putExtra(MapActivity.INPUT_STRING_LOCATIONS_JSON, allLocationsAsString);

        context.startActivity(mapActivity);
    }

    private ArrayList<TSOPlace> getAllLocations() {
        ArrayList<TSOPlace> allLocations = new ArrayList<TSOPlace>(m_data.size());

        for (T data : m_data) {
            allLocations.add(getPlaceFromData(data));
        }

        return allLocations;
    }

}
