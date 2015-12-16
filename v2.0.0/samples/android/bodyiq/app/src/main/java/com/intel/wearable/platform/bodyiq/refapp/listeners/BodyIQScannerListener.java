package com.intel.wearable.platform.bodyiq.refapp.listeners;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.intel.wearable.platform.bodyiq.refapp.activities.DevicesListActivity;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.core.device.IWearableScanner;
import com.intel.wearable.platform.core.device.WearableToken;
import com.intel.wearable.platform.core.device.listeners.IWearableScannerListener;

/**
 * BodyIQScannerListener handles scanner found and error result
 * It also takes care on adding and removing progressBar and reporting
 * back to mother activity when a device is found.
 */
public class BodyIQScannerListener implements IWearableScannerListener {

    Context                 context = null;
    DevicesListActivity activity = null;
    final ProgressBar       progressBar;

    private final long      SCANNER_TIMEOUT_MSEC = 2000;
    private final int       REQUEST_ENABLE_BT = 1;

    public BodyIQScannerListener(Context context, final DevicesListActivity activity, ProgressBar progressBar)
    {
        this.context = context;
        this.progressBar = progressBar;
        this.activity = activity;

        this.progressBar.setVisibility(View.VISIBLE);
        this.progressBar.animate();

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                BodyIQAPIManager.getInstance().stoptScan();
                BodyIQScannerListener.this.progressBar.setVisibility(View.GONE);
            }
        },SCANNER_TIMEOUT_MSEC);

    }
    @Override
    public void onWearableFound(IWearableScanner wearableScanner, WearableToken wearableToken) {
        activity.addFoundDevice(wearableToken);

    }

    @Override
    public void onScannerError(IWearableScanner wearableScanner, com.intel.wearable.platform.core.error.Error error) {
        progressBar.setVisibility(View.GONE);
        if (error.getErrorCode() == com.intel.wearable.platform.core.error.Error.BLE_ERROR_BT_DISABLED) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            activity.startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        } else {
            Toast.makeText(context, error.getErrorMessage(), Toast.LENGTH_LONG).show();
        }

    }
}
