package com.intel.wearable.platform.timeiq.refapp.places.add_edit;

import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.google.gson.Gson;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.dbobjects.places.semantic.SemanticKey;
import com.intel.wearable.platform.timeiq.api.resolver.ResolvedLocation;
import com.intel.wearable.platform.timeiq.refapp.ColoredStatusBarActivity;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQPlacesUtils;
import com.intel.wearable.platform.timeiq.refapp.googleAnalytics.GoogleAnalyticsTrackers;
import com.intel.wearable.platform.timeiq.refapp.places.PlaceType;
import com.intel.wearable.platform.timeiq.refapp.places.add_edit.autocomplete.GooglePlacesSearcher;
import com.intel.wearable.platform.timeiq.refapp.places.add_edit.autocomplete.IResolvedLocationListener;

public class ActivityAddOrEditPlace extends ColoredStatusBarActivity implements IResolvedLocationListener {

    private static final String TAG = ActivityAddOrEditPlace.class.getSimpleName();

    public static final String PLACE_KEY_TITLE      = "place_key_title";
    public static final String PLACE_KEY_IS_EDIT    = "place_key_is_edit";
    public static final String PLACE_KEY_NAME       = "place_key_name";
    public static final String PLACE_KEY_ADDRESS_NAME = "place_key_address_name";
    public static final String PLACE_KEY_ADDRESS_LAT = "PLACE_KEY_ADDRESS_LAT";
    public static final String PLACE_KEY_ADDRESS_LON = "PLACE_KEY_ADDRESS_LON";
    public static final String PLACE_KEY_PLACED_ID  = "place_key_place_id";
    public static final String PLACE_KEY_TYPE       = "place_key_type";

    private GooglePlacesSearcher mGooglePlacesSearcher;
    private Toolbar mToolbar;

    private EditText mNameView;

    private boolean mIsEdit;
    private String mOriginalPlaceName;
    private String mPlaceName;
    private String mAddressName;
    private TSOCoordinate mCoordinate;
    private PlaceType mPlaceType;
    private PlaceID mPlaceId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_or_edit_place);

        mGooglePlacesSearcher = new GooglePlacesSearcher(this, this);

        setToolbar();
        prepareNameView();
        getInput();
    }

    private void setToolbar() {
        mToolbar = (Toolbar) findViewById(R.id.toolbar);

        mToolbar.inflateMenu(R.menu.refapp_activity_title_action_bar);

        mToolbar.setNavigationIcon(R.drawable.ra_ic_action_cancel);
        mToolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });

        mToolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int id = item.getItemId();
                if (id == R.id.edit_ok) {
                    if (!mIsEdit || addressHasChanged()) {
                        mGooglePlacesSearcher.setResolvedLocation();
                    } else if (nameHasChanged()) {
                        saveAndExit(mAddressName, mCoordinate);
                    } // else nothing has changed

                    Log.d(TAG, "END setResolvedLocation");
                }
                return false;
            }
        });
    }

    private boolean addressHasChanged() {
        return mAddressName != null ? !mAddressName.equals(mGooglePlacesSearcher.getAddress()) : mGooglePlacesSearcher.getAddress() != null;
    }

    private boolean nameHasChanged() {
        boolean nameHasChanged = false;
        if (mNameView != null && mNameView.getText() != null) {
            nameHasChanged = !mOriginalPlaceName.equals(mNameView.getText().toString());
        }

        return nameHasChanged;
    }

    private void prepareNameView() {
        mNameView = (EditText) findViewById(R.id.add_or_edit_place_search_place_name);
        mNameView.setOnKeyListener(new View.OnKeyListener() {
            public boolean onKey(View arg0, int arg1, KeyEvent arg2) {
                return false;
            }
        });

        mNameView.addTextChangedListener(new TextWatcher() {
            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                String name = "";
                if (mNameView != null && mNameView.getText() != null) {
                    name = mNameView.getText().toString();
                }

                mPlaceName = name;
            }
        });
    }

    private void getInput() {
        try {
            Bundle bundle = getIntent().getExtras();

        // Title
            String title = bundle.getString(PLACE_KEY_TITLE);
            mToolbar.setTitle(title);

        // Place ID
            String placeIdJson = bundle.getString(PLACE_KEY_PLACED_ID);
            if(!TextUtils.isEmpty(placeIdJson)) {
                mPlaceId = new Gson().fromJson(placeIdJson, PlaceID.class);
            }

        //Place type
            if(bundle.containsKey(PLACE_KEY_TYPE)) {
                mPlaceType = PlaceType.values()[bundle.getInt(PLACE_KEY_TYPE)];
            } else {
                mPlaceType = PlaceType.OTHER;
            }

        // Delete button
            mIsEdit = bundle.getBoolean(PLACE_KEY_IS_EDIT);
            View deleteLayout = findViewById(R.id.add_or_edit_place_delete_layout);
            Button deleteButton = (Button) findViewById(R.id.add_or_edit_place_delete_button);
            if (mIsEdit && mPlaceId != null && mPlaceType == PlaceType.OTHER) { // show delete when in edit and not (home or work)
                deleteLayout.setVisibility(View.VISIBLE);
                deleteButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        deletePlace();
                    }
                });
            } else {
                deleteLayout.setVisibility(View.GONE);
            }

        // Name
            String name = bundle.getString(PLACE_KEY_NAME);
            mOriginalPlaceName = name;
            setName(name);

        // Name is editable
            if (name != null) {
                mNameView.setEnabled(mPlaceType == PlaceType.OTHER); // cannot edit name for home or work
            }

        // Address
            if(bundle.containsKey(PLACE_KEY_ADDRESS_NAME)) {
                mAddressName = bundle.getString(PLACE_KEY_ADDRESS_NAME);
                mGooglePlacesSearcher.setAddress(mAddressName);
            }

        // Coordinate
            if(bundle.containsKey(PLACE_KEY_ADDRESS_LAT) && bundle.containsKey(PLACE_KEY_ADDRESS_LON)) {
                double lat = bundle.getDouble(PLACE_KEY_ADDRESS_LAT);
                double lon = bundle.getDouble(PLACE_KEY_ADDRESS_LON);
                mCoordinate = new TSOCoordinate(lat, lon);
            }

        } catch (Exception e) {
            Log.e(TAG, "failed to get input");
        }
    }

    private void deletePlace() {
        if (TimeIQPlacesUtils.deletePlace(mPlaceId)) {
            finish();
        }
        else {
            Log.e(TAG, "deletePlace: failed");
        }
    }

    public void setName(String name) {
        if (!TextUtils.isEmpty(name)) {
            mPlaceName = name;
            mNameView.setText(name);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        mGooglePlacesSearcher.connect();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mGooglePlacesSearcher.disconnect();
    }

    @Override
    public void onSearchTermChanged(String text) { }

    @Override
    public void onPlacePicked(String name) {
        if (TextUtils.isEmpty(mPlaceName)) {
            setName(name);
        }
    }

    @Override
    public void onResolvedLocation(ResolvedLocation resolvedLocation) {
        saveAndExit(resolvedLocation.getAddress(), resolvedLocation.getLocation());
    }

    @Override
    public void onResolvedLocationError(String error) {
        Toast.makeText(this, error, Toast.LENGTH_LONG).show();
    }

    private void saveAndExit(String address, TSOCoordinate location) {
        Log.d(TAG, "saveAndExit");

        String errorMessage = null;
        if(address != null && location != null) {
            ResultObject<PlaceID> result;
            if (mPlaceId == null) {
                result = TimeIQPlacesUtils.savePlace(this, mPlaceType, address, mPlaceName, location);
            } else {
                ResultObject<Boolean> editPlaceResultObject = TimeIQPlacesUtils.editPlace(this, mPlaceId, address, mPlaceName, location);
                result = new ResultObject<>(editPlaceResultObject.isSuccess(), editPlaceResultObject.getMsg(), mPlaceId);
            }

            if(!result.isSuccess()){
                errorMessage = result.getMsg();
            } else {
                PlaceID placeID = result.getData();
                SemanticKey semanticKey = placeID.getSemanticKey();
                int semanticKeyId = semanticKey.isHome() ? R.string.home : (semanticKey.isWork() ? R.string.work : R.string.google_analytics_place_other);
                if (mPlaceId == null) {
                    GoogleAnalyticsTrackers.getInstance().trackEvent(R.string.google_analytics_place, R.string.google_analytics_add, semanticKeyId);
                } else {
                    GoogleAnalyticsTrackers.getInstance().trackEvent(R.string.google_analytics_place, R.string.google_analytics_edit, semanticKeyId);
                }
            }
        } else {
            errorMessage = getString(R.string.toast_no_resolved_location);
        }

        mGooglePlacesSearcher.closeWaitDialog();
        if(errorMessage == null){
            finish();
        } else {
            Log.i(TAG, errorMessage);
            Toast.makeText(this, errorMessage, Toast.LENGTH_LONG).show();
        }
    }

}
