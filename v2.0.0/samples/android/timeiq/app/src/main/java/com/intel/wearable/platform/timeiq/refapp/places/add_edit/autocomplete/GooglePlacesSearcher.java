package com.intel.wearable.platform.timeiq.refapp.places.add_edit.autocomplete;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.PendingResult;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.common.api.Status;
import com.google.android.gms.location.places.Place;
import com.google.android.gms.location.places.PlaceBuffer;
import com.google.android.gms.location.places.Places;
import com.google.android.gms.maps.model.LatLng;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.location.TSOCoordinate;
import com.intel.wearable.platform.timeiq.api.resolver.ResolvedLocation;
import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.concurrent.TimeUnit;

public class GooglePlacesSearcher implements GoogleApiClient.ConnectionCallbacks, GoogleApiClient.OnConnectionFailedListener {

    private static final String TAG = GooglePlacesSearcher.class.getSimpleName();
    private static final long WAIT_FOR_RESPONSE_TIMEOUT = 5000;

    private Activity mActivity;
    private IResolvedLocationListener mResolvedLocationListener;
    private ProgressDialog mSavingProgressDialog;

    private TextView mAutocompleteEditText;
    private ArrayAdapter<String> mAdapterAutoComplete;
    private TextWatcher mTextWatcher = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            if (mResolvedLocationListener != null) {
                mResolvedLocationListener.onSearchTermChanged(s.toString());
            }

            mAdapterAutoComplete.getFilter().filter(s);
        }

        @Override
        public void afterTextChanged(Editable s) {
        }
    };

    private GoogleApiClient mGoogleApiClient;
    private AutocompleteData mSelectedAutocompleteData;

    private String mPlaceAddress;

    public GooglePlacesSearcher(Activity activity, IResolvedLocationListener resolvedLocationListener) {
        mActivity = activity;
        mResolvedLocationListener = resolvedLocationListener;

        prepareAutocompleteEditText(activity);
        prepareListView(activity);

        mGoogleApiClient = new GoogleApiClient.Builder(activity)
                .addApi(Places.GEO_DATA_API)
                .addConnectionCallbacks(this)
                .addOnConnectionFailedListener(this)
                .build();
    }

    private void prepareAutocompleteEditText(Activity activity) {
        mAutocompleteEditText = (EditText) activity.findViewById(R.id.add_or_edit_place_search_autocomplete_search);
        mAutocompleteEditText.addTextChangedListener(mTextWatcher);
        mAutocompleteEditText.requestFocus();
    }

    private void prepareListView(Activity activity) {
        ListView resolvedResultsListView = (ListView) activity.findViewById(R.id.add_or_edit_place_search_results_list);
        mAdapterAutoComplete = new GooglePlacesAutocompleteAdapter(activity, android.R.layout.simple_list_item_1);
        resolvedResultsListView.setAdapter(mAdapterAutoComplete);

        resolvedResultsListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                try {
                    GooglePlacesAutocompleteAdapter PlacesAutocompleteAdapter = (GooglePlacesAutocompleteAdapter) parent.getAdapter();
                    mSelectedAutocompleteData = PlacesAutocompleteAdapter.getItemData(position);
                    setAddress(mSelectedAutocompleteData.mPlaceDescription);
                    if (mResolvedLocationListener != null) {
                        mResolvedLocationListener.onPlacePicked(mSelectedAutocompleteData.mPlaceDescription);
                    }
                } catch (Exception e) {
                    Log.e(TAG, e.getClass().getName() + " - Message: " + e.getMessage());
                }

            }
        });
    }

    @Override
    public void onConnected(Bundle connectionHint) {
        Log.d(TAG, "GoogleApiClient - onConnected");
    }

    @Override
    public void onConnectionSuspended(int cause) {
        Log.w(TAG, "GoogleApiClient - onConnectionSuspended, cause: " + cause);
    }

    @Override
    public void onConnectionFailed(ConnectionResult result) {
        Log.e(TAG, "GoogleApiClient - onConnectionFailed, result: errorMessage: " + result.getErrorMessage() + " errorCode" + result.getErrorCode());
        if (mResolvedLocationListener != null) {
            String error = mActivity.getString(R.string.google_api_client_error_cannot_connect, result.toString());
            mResolvedLocationListener.onResolvedLocationError(error);
        }
    }

    public void setAddress(String address) {
    // this is done to disable search when the text is changed by the app (and not the user)
        mAutocompleteEditText.removeTextChangedListener(mTextWatcher);
        if (!TextUtils.isEmpty(address)) {
            mPlaceAddress = address;
            mAutocompleteEditText.setText(address);
        }

        mAutocompleteEditText.addTextChangedListener(mTextWatcher);
    }

    public String getAddress() {
        return mPlaceAddress;
    }

    public void connect() {
        mGoogleApiClient.connect();
    }

    public void disconnect() {
        mGoogleApiClient.disconnect();
    }

    public void setResolvedLocation() {
        if (mSelectedAutocompleteData != null ) {
            String googlePlacesId = mSelectedAutocompleteData.mGooglePlacesId;
            String dialogTitle = mActivity.getString(R.string.save_dialog_progress_dialog_title);
            String dialogBody = mActivity.getString(R.string.save_dialog_progress_dialog_body);
            mSavingProgressDialog = ProgressDialog.show(mActivity, dialogTitle, dialogBody);

            try {
                PendingResult<PlaceBuffer> placeById = Places.GeoDataApi.getPlaceById(mGoogleApiClient, googlePlacesId);
                placeById.setResultCallback(new ResultCallback<PlaceBuffer>() {
                    @Override
                    public void onResult(PlaceBuffer places) {
                        Log.d(TAG, "getPlaceById - onResult");
                        Status status = places.getStatus();
                        if (status.isSuccess()) {
                            try {
                                final Place place = places.get(0);
                                LatLng locationLatLon = place.getLatLng();
                                ResolvedLocation resolvedLocation = new ResolvedLocation();
                                TSOCoordinate tsoCoordinate = new TSOCoordinate(locationLatLon.latitude, locationLatLon.longitude);
                                resolvedLocation.setLocation(tsoCoordinate);
                                resolvedLocation.setName(mSelectedAutocompleteData.mPlaceDescription);
                                resolvedLocation.setAddress(mPlaceAddress);
                                places.release();
                                mResolvedLocationListener.onResolvedLocation(resolvedLocation);
                            } catch (Exception e) {
                                mSavingProgressDialog.dismiss();
                                Log.e(TAG, e.getClass().getName() + " during getPlaceById - " + e.getMessage(), e);
                                if (mResolvedLocationListener != null) {
                                    String error = e.getMessage();
                                    mResolvedLocationListener.onResolvedLocationError(error);
                                }
                            }
                        } else {
                            Log.e(TAG, "getPlaceById - ERROR StatusMessage: " + status.getStatusMessage());
                            Log.e(TAG, "getPlaceById - ERROR StatusCode:    " + status.getStatusCode());
                            mSavingProgressDialog.dismiss();
                            if (mResolvedLocationListener != null) {
                                String error = status.getStatusMessage();
                                mResolvedLocationListener.onResolvedLocationError(error);
                            }
                        }
                    }
                }, WAIT_FOR_RESPONSE_TIMEOUT, TimeUnit.MILLISECONDS);

            } catch (Exception e) {
                mSavingProgressDialog.dismiss();
                Log.e(TAG, e.getClass().getName() + " during setResolvedLocation " + e.getMessage(), e);
                if (mResolvedLocationListener != null) {
                    String error = e.getMessage();
                    mResolvedLocationListener.onResolvedLocationError(error);
                }
            }
        }
    }

    public void closeWaitDialog() {
        if (mSavingProgressDialog != null) {
            mSavingProgressDialog.dismiss();
            mSavingProgressDialog = null;
        }
    }

}
