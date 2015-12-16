package com.intel.wearable.platform.bodyiq.refapp.listeners;

import android.content.Context;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.customui.CustomToastMessage;
import com.intel.wearable.platform.core.event.system.IWearableSystemEventListener;
import com.intel.wearable.platform.core.event.system.WearableSystemEvent;

/**
 * Created by fissaX on 12/10/15.
 */
public class BodyIQSystemEventListener implements IWearableSystemEventListener {
    Context context = null;
    public BodyIQSystemEventListener(Context context) {
        this.context = context;
    }
    @Override
    public void onWearableSystemEvent(WearableSystemEvent event) {



        if (event.getSystemEvent() instanceof WearableSystemEvent.CrashEvent) {
            WearableSystemEvent.CrashEvent.CrashEventType crashType = ((WearableSystemEvent.CrashEvent) event.getSystemEvent()).getCrashType();
            new CustomToastMessage(context).showMessage(context.getResources().getString(R.string.systemcrash) +crashType.name());

        } else if (event.getSystemEvent() instanceof WearableSystemEvent.LowBattEvent) {
            int batterLevel = ((WearableSystemEvent.LowBattEvent) event.getSystemEvent()).getBatteryLevel();
            new CustomToastMessage(context).showMessage(context.getResources().getString(R.string.batterylevellow) + batterLevel);


        } else if (event.getSystemEvent() instanceof WearableSystemEvent.ShutdownEvent) {
            WearableSystemEvent.ShutdownEvent.ShutdownType shutDownType = ((WearableSystemEvent.ShutdownEvent) event.getSystemEvent()).getShutdownType();
            new CustomToastMessage(context).showMessage(context.getResources().getString(R.string.systemshutdown) + shutDownType.name());

        } else if (event.getSystemEvent() instanceof WearableSystemEvent.BootEvent) {
            WearableSystemEvent.BootEvent.BootType bootType = ((WearableSystemEvent.BootEvent) event.getSystemEvent()).getBootType();
            new CustomToastMessage(context).showMessage(context.getResources().getString(R.string.systemboot) + bootType.name());

        }



    }
}
