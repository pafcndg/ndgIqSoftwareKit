package com.intel.wearable.platform.timeiq.refapp.coreiq;

import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import com.intel.wearable.platform.core.device.IWearableScanner;
import com.intel.wearable.platform.core.device.WearableToken;
import com.intel.wearable.platform.core.device.listeners.IWearableScannerListener;
import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.ArrayList;

public class ScanningActivity extends AppCompatActivity {

    private Button mScanButton;
    private boolean mScanning = false;
    private static final int REQUEST_ENABLE_BT = 1;
    private View mSscanProgressBar;
    private ArrayList<WearableToken> mDevicesArray;
    private DevicesListAdapter mDevicesListAdapter;
    private View mNoPairedDevicesMsg;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        CoreIQUtil coreIQUtil = CoreIQUtil.getInstance();
        if(coreIQUtil.isConnected() || coreIQUtil.isPaired() && coreIQUtil.getCoreIQToken() != null){
            Intent pairingActivityIntent = new Intent(getApplicationContext(), PairingActivity.class);
            pairingActivityIntent.putExtra(PairingActivity.WEARABLE_TOKEN, coreIQUtil.getCoreIQToken());
            startActivity(pairingActivityIntent);
        }
        final IWearableScanner scanner = coreIQUtil.getScanner();
        if(scanner.isScanning()){
            scanner.stopScan();
        }
        mScanning = false;

        setContentView(R.layout.activity_scanning);

        mScanButton = (Button)findViewById(R.id.scan_button);

        mSscanProgressBar = findViewById(R.id.scan_progressBar);

        mNoPairedDevicesMsg = findViewById(R.id.no_paired_device);

        mDevicesArray = new ArrayList<>();

        mDevicesListAdapter = new DevicesListAdapter(this, R.id.devices_ListView, mDevicesArray);

        ListView listView = (ListView) findViewById(R.id.devices_ListView);
        listView.setAdapter(mDevicesListAdapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long l) {
                //pair device:
                if (mScanning) {
                    scanner.stopScan();
                    stopScanUi();
                }
                WearableToken token = mDevicesArray.get(position);
                if (token != null) {
                    if(TextUtils.isEmpty(token.getDisplayName())){
                        Toast.makeText(getApplication(), getString(R.string.cant_pair_non_wearable), Toast.LENGTH_LONG).show();
                    }
                    else{
                        Intent pairingActivityIntent = new Intent(getApplicationContext(), PairingActivity.class);
                        pairingActivityIntent.putExtra(PairingActivity.WEARABLE_TOKEN, token);
                        startActivity(pairingActivityIntent);
                    }
                }
            }
        });

        stopScanUi();
    }

    public void onScanClicked(View view) {
        if( ! mScanning ){
            startScan();
        }
        else{
            CoreIQUtil.getInstance().getScanner().stopScan();
            stopScanUi();
        }
    }


    public void startScan(){
        mScanButton.setText(R.string.stop_scan_button_text);
        mScanning = true;
        mSscanProgressBar.setVisibility(View.VISIBLE);
        mNoPairedDevicesMsg.setVisibility(mDevicesArray.isEmpty() ? View.VISIBLE :  View.GONE);
        mDevicesArray.clear();
        mDevicesListAdapter.notifyDataSetChanged();

        CoreIQUtil.getInstance().getScanner().startScan(new IWearableScannerListener() {

            @Override
            public void onWearableFound(IWearableScanner iWearableScanner, WearableToken wearableToken) {
                mNoPairedDevicesMsg.setVisibility(View.GONE);
                mDevicesArray.add(wearableToken);
                mDevicesListAdapter.notifyDataSetChanged();
            }

            @Override
            public void onScannerError(IWearableScanner wearableScanner, com.intel.wearable.platform.core.error.Error error) {
                stopScanUi();
                if (error.getErrorCode() == com.intel.wearable.platform.core.error.Error.BLE_ERROR_BT_DISABLED) {
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
                } else {
                    Toast.makeText(getApplication(), error.getErrorMessage(), Toast.LENGTH_LONG).show();
                }

            }
        });
    }

    private void stopScanUi(){
        mScanButton.setText(R.string.scan_button_text);
        mScanning = false;
        mSscanProgressBar.setVisibility(View.GONE);
        mNoPairedDevicesMsg.setVisibility(mDevicesArray.isEmpty() ? View.VISIBLE :  View.GONE);

    }
}
