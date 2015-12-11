package com.intel.wearable.platform.timeiq.refapp.developer.activities;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.view.View;
import android.view.ViewTreeObserver;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.LatLngBounds;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.gson.reflect.TypeToken;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.common.externallibs.IJSONUtils;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.refapp.developer.R;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MapActivity extends FragmentActivity {

    public static final String INPUT_STRING_TITLE = "INPUT_STRING_TITLE";
    public static final String INPUT_STRING_SELECTED_LOCATION = "INPUT_STRING_SELECTED_LOCATION";
    public static final String INPUT_STRING_LOCATIONS_JSON = "INPUT_STRING_LOCATIONS_JSON";
    private static final float SELECTED_LOCATION_COLOR = BitmapDescriptorFactory.HUE_GREEN;
    private static final int MAP_PADDING = 200; // offset from edges of the map in pixels
    private static final float DEFAULT_ZOOM_LEVEL = 12.0f;

    private GoogleMap mMap;
    private IJSONUtils mJsonUtils;
    private TSOPlace mSelectedPlace;
    protected HashMap<Marker, TSOPlace> mMarkerLocationHash = new HashMap<Marker, TSOPlace>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.layout_map);
        init();
    }

    private void init() {
        mJsonUtils = ClassFactory.getInstance().resolve(IJSONUtils.class);
        initMap();
        getInputAndInit();
    }

    private void getInputAndInit() {
        Intent intent = getIntent();
        Bundle extras;
        if (intent != null) {
            extras = intent.getExtras();
            if (extras != null) {
                getAndSetTitle(extras);
                getAndSetSelectedLocation(extras);
                getAndSetLocations(extras);
            }
        }
    }

    private void getAndSetTitle(Bundle extras) {
        String title = extras.getString(INPUT_STRING_TITLE, null);
        TextView titleTextView = (TextView) findViewById(R.id.map_title);
        titleTextView.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                zoomMapToMarkers();
            }
        });

        if (title != null) {
            titleTextView.setVisibility(View.VISIBLE);
            titleTextView.setText(title);
        } else {
            titleTextView.setVisibility(View.GONE);
        }
    }

    private void getAndSetSelectedLocation(Bundle extras) {
        String selectedPlaceAsString = extras.getString(INPUT_STRING_SELECTED_LOCATION, null);
        mSelectedPlace = mJsonUtils.fromJson(selectedPlaceAsString, TSOPlace.class);
        if (mSelectedPlace == null) {
            Toast.makeText(getApplicationContext(), getString(R.string.developer_map_error_no_selected_location), Toast.LENGTH_LONG).show();
        }
    }

    private void getAndSetLocations(Bundle extras) {
        String locationsJson = extras.getString(INPUT_STRING_LOCATIONS_JSON, null);
        Type type = new TypeToken<ArrayList<TSOPlace>>() { }.getType();
        ArrayList<TSOPlace> allLocations = mJsonUtils.fromJson(locationsJson, type);
        if (locationsJson != null) {
            if (mSelectedPlace != null) {
                if (allLocations.contains(mSelectedPlace)) {
                    int selectedIndex = allLocations.indexOf(mSelectedPlace);
                    addLocationsToMap(allLocations, selectedIndex);
                } else {
                    Toast.makeText(getApplicationContext(), getString(R.string.developer_map_error_locations_do_not_contain_selected_location), Toast.LENGTH_LONG).show();
                }
            }
        } else {
            Toast.makeText(getApplicationContext(), getString(R.string.developer_map_error_no_locations), Toast.LENGTH_LONG).show();
        }
    }

    private void initMap() {
        if (mMap == null) {
            mMap = ((SupportMapFragment) getSupportFragmentManager().findFragmentById(R.id.map_map_fragment)).getMap();
//            mMap.setOnInfoWindowClickListener(new GoogleMap.OnInfoWindowClickListener() {
//                @Override
//                public void onInfoWindowClick(Marker marker) {
//                }
//            });
//
//            mMap.setOnMarkerClickListener(new GoogleMap.OnMarkerClickListener() {
//                @Override
//                public boolean onMarkerClick(Marker marker) {
//                    return false;
//                }
//            });
//
//            mMap.setOnMapClickListener(new GoogleMap.OnMapClickListener() {
//                @Override
//                public void onMapClick(LatLng point) {
//                }
//            });
        }
    }

    private void addLocationsToMap(ArrayList<TSOPlace> locationsToAdd, int selectedIndex) {
        if (locationsToAdd != null) {
            TSOPlace location;
            for (int i=0 ; i< locationsToAdd.size() ; ++i) {
                location = locationsToAdd.get(i);
                Marker marker;
                if (i == selectedIndex) {
                    marker = addMarker(new LatLng(location.getCoordinate().getLatitude(), location.getCoordinate().getLongitude()), location.getName(), location.getAddress(), SELECTED_LOCATION_COLOR);
                    marker.showInfoWindow();
                } else {
                    marker = addMarker(new LatLng(location.getCoordinate().getLatitude(), location.getCoordinate().getLongitude()), location.getName(), location.getAddress());
                }

                mMarkerLocationHash.put(marker, location);
            }
        }
    }

    private Marker addMarker(LatLng latLng, String title, String snippet, float color) {
        return addMarker(getMarkerOptions(latLng, title, snippet)
                .icon(BitmapDescriptorFactory.defaultMarker(color)));
    }

    private Marker addMarker(LatLng latLng, String title, String snippet) {
        return addMarker(getMarkerOptions(latLng, title, snippet));
    }

    private Marker addMarker(MarkerOptions markerOptions) {
        return mMap.addMarker(markerOptions);
    }

    private MarkerOptions getMarkerOptions(LatLng latLng, String title, String description) {
        return new MarkerOptions()
                .position(latLng)
                .title("\u200e" + title)            // "\u200e" - hack to handle right to left characters (will not show any title (at all) that _starts_ with right to left characters)
                .snippet("\u200e" + description);   // (taken from http://stackoverflow.com/questions/26244298/non-ascii-title-for-android-map-marker)
    }

    private void zoomMapToMarkers() {
        if (mMap != null && mMarkerLocationHash != null && !mMarkerLocationHash.isEmpty()) {
            if (mMarkerLocationHash.size() == 1) {
                Map.Entry<Marker, TSOPlace> firstEntry = mMarkerLocationHash.entrySet().iterator().next();
                TSOPlace location = firstEntry.getValue();
                mMap.animateCamera(CameraUpdateFactory.newLatLng(new LatLng(location.getCoordinate().getLatitude(), location.getCoordinate().getLongitude())));
            } else {
                LatLngBounds.Builder latLngBoundsBuilder = new LatLngBounds.Builder();
                for (Marker marker : mMarkerLocationHash.keySet()) {
                    latLngBoundsBuilder.include(marker.getPosition());
                }

                try {
                    LatLngBounds bounds = latLngBoundsBuilder.build();
                    CameraUpdate cameraUpdate = CameraUpdateFactory.newLatLngBounds(bounds, MAP_PADDING);
                    mMap.animateCamera(cameraUpdate);      // For zooming automatically
                } catch (IllegalStateException e) {
                    e.printStackTrace();
                    Toast.makeText(getApplicationContext(), getString(R.string.developer_map_error_no_locations, e.getMessage()), Toast.LENGTH_LONG).show();
                    zoomMapToDefault();
                }
            }
        }
    }

    private void zoomMapToDefault() {
        mMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(mSelectedPlace.getCoordinate().getLatitude(), mSelectedPlace.getCoordinate().getLongitude()), DEFAULT_ZOOM_LEVEL)); // For zooming automatically
    }

}
