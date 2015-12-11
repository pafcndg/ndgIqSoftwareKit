package com.intel.wearable.platform.timeiq.refapp.coreiq;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.intel.wearable.platform.core.device.IWearableController;
import com.intel.wearable.platform.core.device.WearableBatteryStatus;
import com.intel.wearable.platform.core.device.WearableToken;
import com.intel.wearable.platform.core.device.listeners.IWearableControllerListener;
import com.intel.wearable.platform.timeiq.refapp.R;

public class PairingActivity extends AppCompatActivity implements IWearableControllerListener {

    public static final String WEARABLE_TOKEN = "WEARABLE_TOKEN";
    private static final String TAG = PairingActivity.class.getSimpleName();
    private WearableToken mToken;
    private TextView mDeviceNameTextView;
    private TextView mDeviceIDTextView;
    private TextView mPairingTitleTextView;
    private ProgressBar mPairingProgressBar;
    private Button mConnectDeviceButton;
    private Button mPairDeviceButton;
    private PairingUiState mPairingUiState;
    private TextView mBatteryLevelTextView;
    private TextView mSoftwareRevisionTextView;


    enum PairingUiState{
        ERROR,
        NOT_PAIRED,
        PAIRING,
        PAIRED,
        CONNECTING,
        CONNECTED
    }

    enum PairingErrorType{
        NO_ERRORS,
        NO_EXTRA,
        NO_TOKEN,
        NO_CONTROLLER
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pairing);
        //pairing_title_text
        mPairingTitleTextView = (TextView) findViewById(R.id.pairing_title_text);
        mDeviceNameTextView = (TextView) findViewById(R.id.device_name_TextView);
        mDeviceIDTextView = (TextView) findViewById(R.id.device_id_TextView);
        mPairingProgressBar = (ProgressBar)findViewById(R.id.pairingProgressBar);
        mConnectDeviceButton = (Button)findViewById(R.id.connect_deviceButton);
        mPairDeviceButton = (Button)findViewById(R.id.pair_device_button);
        mBatteryLevelTextView = (TextView) findViewById(R.id.battery_levelTextView);
        mSoftwareRevisionTextView = (TextView) findViewById(R.id.software_revisionTextView);


        final Bundle extras = getIntent().getExtras();
        if (null != extras) {
            mToken = (WearableToken) extras.getSerializable(WEARABLE_TOKEN);
            if (null != mToken) {
                mDeviceNameTextView.setText(mToken.getDisplayName());
                mDeviceIDTextView.setText(mToken.getAddress());
                CoreIQUtil coreIQUtil = CoreIQUtil.getInstance();
                coreIQUtil.setCoreIQTControllerListener(this);
                coreIQUtil.setCoreIQToken(mToken);

                if (coreIQUtil.isConnected()) {
                    resetUi(PairingUiState.CONNECTED, PairingErrorType.NO_ERRORS);
                    coreIQUtil.getBatteryStatusAsync();
                } else if (coreIQUtil.isPaired()) {
                    resetUi(PairingUiState.PAIRED, PairingErrorType.NO_ERRORS);
                } else {
                    resetUi(PairingUiState.NOT_PAIRED, PairingErrorType.NO_ERRORS);
                }
            } else {
                resetUi(PairingUiState.ERROR, PairingErrorType.NO_TOKEN);
            }
        } else{
            resetUi(PairingUiState.ERROR, PairingErrorType.NO_EXTRA);
        }
    }


    @Override
    protected void onStop() {
        Log.d(TAG, "onStop ");
        CoreIQUtil.getInstance().setCoreIQTControllerListener(null);
        super.onStop();
    }

    private boolean pair(){
        String errorMsg = CoreIQUtil.getInstance().pair();
        if(errorMsg != null) {
            Toast.makeText(getApplication(), errorMsg, Toast.LENGTH_LONG).show();
            Log.d(TAG, "pair: " + errorMsg);
        }
        return errorMsg == null;
    }

    private boolean connect(){
        String errorMsg = CoreIQUtil.getInstance().connect();
        if( errorMsg != null ) {
            Toast.makeText(getApplication(), errorMsg, Toast.LENGTH_LONG).show();
        }
        return errorMsg == null;
    }

    public void onConnectClicked(View view) {
        if(mPairingUiState == PairingUiState.CONNECTED) {
            boolean disconnect = CoreIQUtil.getInstance().disconnect();
            Log.d(TAG, "disconnect ok ? " + disconnect);
            resetUi(PairingUiState.PAIRED, PairingErrorType.NO_ERRORS);
        }
        else{
            if(connect()){
                resetUi(PairingUiState.CONNECTING, PairingErrorType.NO_ERRORS);
            }
        }
        // on all other options - we will not enable the button
    }

    public void onPairClicked(View view) {
        if(mPairingUiState == PairingUiState.NOT_PAIRED) {
            if(pair()) {
                Log.d(TAG, "paired... OK");
                resetUi(PairingUiState.PAIRING, PairingErrorType.NO_ERRORS);
            }
            else{
                Log.d(TAG, "paired... not OK");
            }
        }
        else if(mPairingUiState == PairingUiState.PAIRED) {
            Log.d(TAG, "try to unpair");
            CoreIQUtil.getInstance().unpair();
            resetUi(PairingUiState.NOT_PAIRED, PairingErrorType.NO_ERRORS);
        }
        // on all other options - we will not enable the button
    }

    private void resetUi(PairingUiState pairingUiState, PairingErrorType errorType){
        String deviceSoftwareRevision = CoreIQUtil.getInstance().getDeviceSoftwareRevision();
        if(TextUtils.isEmpty(deviceSoftwareRevision)){
            deviceSoftwareRevision = getString(R.string.unknown);
        }
        mSoftwareRevisionTextView.setText(deviceSoftwareRevision);

        mPairingUiState = pairingUiState;
        switch (pairingUiState){
            case ERROR:
                String error = "error: ";
                switch (errorType){

                    case NO_EXTRA:
                        error += "no extra";
                        break;
                    case NO_TOKEN:
                        error += "no token";
                        break;
                    case NO_CONTROLLER:
                        error += "no controller";
                        break;
                }
                mPairingTitleTextView.setText(error);
                mPairingProgressBar.setVisibility(View.GONE);
                mConnectDeviceButton.setText(R.string.connect_device_button);
                mPairDeviceButton.setText(R.string.pair_device_button);
                mConnectDeviceButton.setEnabled(false);
                mPairDeviceButton.setEnabled(false);
                break;
            case PAIRING:
                mPairingTitleTextView.setText(R.string.pairing_to_text);
                mPairingProgressBar.setVisibility(View.VISIBLE);
                mConnectDeviceButton.setText(R.string.connect_device_button);
                mPairDeviceButton.setText(R.string.pair_device_button);
                mConnectDeviceButton.setEnabled(false);
                mPairDeviceButton.setEnabled(false);
                break;
            case PAIRED:
                mPairingTitleTextView.setText(R.string.paired_to_text);
                mPairingProgressBar.setVisibility(View.GONE);
                mConnectDeviceButton.setText(R.string.connect_device_button);
                mPairDeviceButton.setText(R.string.unpair_device_button);
                mConnectDeviceButton.setEnabled(true);
                mPairDeviceButton.setEnabled(true);
                break;
            case CONNECTING:
                mPairingTitleTextView.setText(R.string.connecting_to_text);
                mPairingProgressBar.setVisibility(View.VISIBLE);
                mConnectDeviceButton.setText(R.string.connect_device_button);
                mPairDeviceButton.setText(R.string.pair_device_button);
                mConnectDeviceButton.setEnabled(false);
                mPairDeviceButton.setEnabled(false);
                break;
            case CONNECTED:
                mPairingTitleTextView.setText(R.string.connected_to_text);
                mPairingProgressBar.setVisibility(View.GONE);
                mConnectDeviceButton.setText(R.string.disconnect_device_button);
                mPairDeviceButton.setText(R.string.pair_device_button);
                mConnectDeviceButton.setEnabled(true);
                mPairDeviceButton.setEnabled(false);
                break;
            case NOT_PAIRED:
                mPairingTitleTextView.setText(R.string.not_paired_to_text);
                mPairingProgressBar.setVisibility(View.GONE);
                mConnectDeviceButton.setText(R.string.connect_device_button);
                mPairDeviceButton.setText(R.string.pair_device_button);
                mConnectDeviceButton.setEnabled(true);
                mPairDeviceButton.setEnabled(true);
                break;
        }
    }

    @Override
    public void onConnecting(IWearableController iWearableController) {
        Log.d(TAG, "onConnecting ");
    }

    @Override
    public void onConnected(IWearableController iWearableController) {
        Log.d(TAG, "onConnected ");
        resetUi(PairingUiState.CONNECTED, PairingErrorType.NO_ERRORS);
        CoreIQUtil.getInstance().getBatteryStatusAsync();
    }

    @Override
    public void onDisconnecting(IWearableController iWearableController) {
        Log.d(TAG, "onDisconnecting ");
    }

    @Override
    public void onDisconnected(IWearableController iWearableController) {
        Log.d(TAG, "onDisconnected ");
        resetUi(PairingUiState.PAIRING, PairingErrorType.NO_ERRORS);
    }

    @Override
    public void onPairedStatusChanged(IWearableController iWearableController, boolean isPaired) {
        Log.d(TAG, "onPairedStatusChanged: " + isPaired);
        resetUi(isPaired? PairingUiState.PAIRED : PairingUiState.NOT_PAIRED, PairingErrorType.NO_ERRORS);

        CoreIQUtil.getInstance().setCoreIQToken(isPaired ? mToken : null);
    }

    @Override
    public void onBatteryStatusUpdate(IWearableController iWearableController, WearableBatteryStatus wearableBatteryStatus) {
        byte batteryLevel = wearableBatteryStatus.getBatteryLevel();
        Log.d(TAG, "onBatteryStatusUpdate: " + batteryLevel);
        if(mBatteryLevelTextView != null) {
            mBatteryLevelTextView.setText("" + batteryLevel);
        }
    }

    @Override
    public void onFailure(IWearableController iWearableController, com.intel.wearable.platform.core.error.Error error) {
        resetUi(PairingUiState.NOT_PAIRED, PairingErrorType.NO_ERRORS);
        Toast.makeText(getApplication(), error.getErrorMessage(), Toast.LENGTH_LONG).show();
        Log.d(TAG, "onFailure: "+error.getErrorMessage());

    }
}
