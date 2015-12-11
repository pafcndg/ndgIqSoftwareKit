package com.intel.wearable.platform.timeiq.refapp.places;

import android.app.Dialog;
import android.content.Intent;
import android.content.res.ColorStateList;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.gson.Gson;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQPlacesUtils;
import com.intel.wearable.platform.timeiq.refapp.main.IMainFragment;
import com.intel.wearable.platform.timeiq.refapp.places.add_edit.ActivityAddOrEditPlace;

import java.util.ArrayList;

/**
 * Created by adura on 22/10/2015.
 */


/**
 * Fragment holding the Places list
 */
public class PlacesFragment extends Fragment implements IMainFragment {

    private static final String TAG = PlacesFragment.class.getSimpleName();
    private static final int GOOGLE_PLAY_SERVICES_AVAILABLE_REQUEST_CODE = 13;

    private ArrayList<PlacesWrapper> mPlacesArray;
    private PlacesListAdapter mPlacesListAdapter;

    public PlacesFragment() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_places, container, false);

        mPlacesArray = new ArrayList<>();

        mPlacesListAdapter = new PlacesListAdapter(getActivity(), R.id.placesListView, mPlacesArray);

        ListView placesListView = (ListView) rootView.findViewById(R.id.placesListView);
        placesListView.setAdapter(mPlacesListAdapter);

        placesListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                int resultCode = GooglePlayServicesUtil.isGooglePlayServicesAvailable(getActivity());
                if (resultCode != ConnectionResult.SUCCESS) {
                    openGoogleServicesUpdateDialog(resultCode);
                } else {
                    PlacesWrapper tsoPlaceWrapper = (PlacesWrapper) parent.getItemAtPosition(position);

                    FragmentActivity activity = getActivity();
                    String placeName = tsoPlaceWrapper.getPlaceName(activity);
                    String placeAddress = tsoPlaceWrapper.getPlaceAddress();
                    PlaceType placeType = tsoPlaceWrapper.getPlaceType();
                    TSOPlace place = tsoPlaceWrapper.getPlace();
                    PlaceID placeId = null;
                    TSOCoordinate coordinate = null;
                    if(place != null){
                        placeId = place.getPlaceId();
                        coordinate = tsoPlaceWrapper.getPlace().getCoordinate();
                    }

                    Intent intentActivity = new Intent(activity, ActivityAddOrEditPlace.class);

                    intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_TITLE, getString(R.string.places_title_edit));
                    intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_NAME, placeName);
                    intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_ADDRESS_NAME, placeAddress);
                    if (coordinate != null) {
                        intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_ADDRESS_LAT, coordinate.getLatitude());
                        intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_ADDRESS_LON, coordinate.getLongitude());
                    }

                    intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_IS_EDIT, true);
                    intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_TYPE, placeType.ordinal());
                    if(placeId != null){
                        String jsonString = new Gson().toJson(placeId);
                        intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_PLACED_ID, jsonString);
                    }

                    getActivity().startActivity(intentActivity);
                }
            }
        });

        return rootView;
    }


    /**
     * On Google Services error open the Google Services Update Dialog
     * @param resultCode the given error code return by google
     */
    private void openGoogleServicesUpdateDialog(int resultCode) {
        Dialog dialog = GooglePlayServicesUtil.getErrorDialog(resultCode, getActivity(), GOOGLE_PLAY_SERVICES_AVAILABLE_REQUEST_CODE);
        if (dialog != null) {
            dialog.show();
        } else {
            Toast.makeText(getActivity(), getString(R.string.google_api_client_error_general), Toast.LENGTH_LONG).show();
        }
    }

    @Override
      public void onResume() {
        super.onResume();
        if (isAdded()) {
            refreshUiPlacesList();
        }
    }

    /**
     * Update the places list from the TimeIQ's placesRepository:
     */
    private void refreshUiPlacesList() {
        mPlacesArray.clear();
        ArrayList<PlacesWrapper> allPlacesIncludingHomeAndWork = TimeIQPlacesUtils.getAllPlacesIncludingHomeAndWork();
        if(allPlacesIncludingHomeAndWork != null && !allPlacesIncludingHomeAndWork.isEmpty()) {
            mPlacesArray.addAll(allPlacesIncludingHomeAndWork);
        }

        mPlacesListAdapter.notifyDataSetChanged();
    }

    @Override
    public void onFragmentShown() {
        Log.d(TAG, "onFragmentShown");
        refreshUiPlacesList();
        FragmentActivity activity = getActivity();
        if (activity != null) {
            FloatingActionButton fab = (FloatingActionButton) activity.findViewById(R.id.fab);
            if (fab != null) {
                fab.setImageResource(R.drawable.e_add);
                int color = getResources().getColor(R.color.places_fab_bg_color);
                fab.setBackgroundTintList(ColorStateList.valueOf(color));
            }
        }
    }

    @Override
    public void onFloatingButtonPressed() {
        Log.d("RefApp", "PlacesFragment: onFloatingButtonPressed");
        int resultCode = GooglePlayServicesUtil.isGooglePlayServicesAvailable(getActivity());
        if (resultCode != ConnectionResult.SUCCESS) {
            openGoogleServicesUpdateDialog(resultCode);
        } else {
            FragmentActivity activity = getActivity();
            Intent intentActivity = new Intent(activity, ActivityAddOrEditPlace.class);

            intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_TITLE, getString(R.string.places_title_add));
            intentActivity.putExtra(ActivityAddOrEditPlace.PLACE_KEY_IS_EDIT, false);

            getActivity().startActivity(intentActivity);
        }
    }
}
