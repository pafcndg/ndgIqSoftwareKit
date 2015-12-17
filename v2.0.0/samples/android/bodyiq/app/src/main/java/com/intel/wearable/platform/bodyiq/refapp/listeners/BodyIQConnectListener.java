package com.intel.wearable.platform.bodyiq.refapp.listeners;

import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.widget.ProgressBar;

import com.intel.wearable.platform.bodyiq.refapp.activities.TrackerActivity;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.customui.CustomToastMessage;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUser;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUserStorageManager;
import com.intel.wearable.platform.core.device.IWearableController;
import com.intel.wearable.platform.core.device.WearableBatteryStatus;
import com.intel.wearable.platform.core.device.listeners.IWearableControllerListener;
import com.intel.wearable.platform.core.util.Logger;

/**
 * Created by fissaX on 12/5/15.
 */
public class BodyIQConnectListener implements IWearableControllerListener
{
    Context context = null;
    ProgressBar progressBar = null;

    public BodyIQConnectListener(Context context, ProgressBar progressBar)
    {
        this.context = context;
        this.progressBar = progressBar;
        progressBar.setVisibility(View.VISIBLE);

    }


    @Override
    public void onConnecting(IWearableController wearableController) {

        Logger.d("BodyIQConnectListener onConnecting");
    }

    @Override
    public void onConnected(IWearableController wearableController) {

        if(context!=null) {
            BodyIQUser user = BodyIQAPIManager.getInstance().getUser();
            user.setDeviceAddress(wearableController.getWearableToken().getAddress());
            user.setDeviceName(wearableController.getWearableToken().getDisplayName());
            BodyIQAPIManager.getInstance().saveUser();
            progressBar.setVisibility(View.GONE);
            Intent intent = new Intent(context, TrackerActivity.class);
            context.startActivity(intent);
            context = null; // new activity is running
        }
        Logger.d("BodyIQConnectListener onConnected");
        BodyIQAPIManager.getInstance().getSingleBatteryRead(); // for current status
        BodyIQAPIManager.getInstance().subscribeToBatteryUpdateEvents(); // for battery changes

    }

    @Override
    public void onDisconnecting(IWearableController wearableController) {

        Logger.d("BodyIQConnectListener onDisconnecting");

    }

    @Override
    public void onDisconnected(IWearableController wearableController) {
        Logger.d("BodyIQConnectListener onDisconnected");
        progressBar.setVisibility(View.GONE);



    }

    @Override
    public void onPairedStatusChanged(IWearableController wearableController, boolean isPaired) {
        Logger.d("BodyIQConnectListener onPairedStatusChanged " + isPaired);

    }

    @Override
    public void onBatteryStatusUpdate(IWearableController wearableController, WearableBatteryStatus batteryStatus) {

        BodyIQAPIManager.getInstance().setBatteryStatusUpdate(String.valueOf(batteryStatus.getBatteryLevel()));
        Logger.d("BodyIQConnectListener onBatteryStatusUpdate " + batteryStatus.getBatteryLevel());

    }

    @Override
    public void onFailure(IWearableController wearableController, com.intel.wearable.platform.core.error.Error error) {
        progressBar.setVisibility(View.GONE);
        new CustomToastMessage(context).showMessage("BodyIQConnectListener Failure " + error.getErrorMessage());
        Logger.d("BodyIQConnectListener Failure " + error.getErrorMessage());

    }

}

