package com.intel.wearable.platform.bodyiq.refapp.listeners;

import android.content.Context;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.customui.CustomToastMessage;
import com.intel.wearable.platform.core.event.user.IWearableUserEventListener;
import com.intel.wearable.platform.core.event.user.WearableUserEvent;

/**
 * Created by fissaX on 12/10/15.
 */
public class BodyIQUserEventListener implements IWearableUserEventListener {

    Context context;

    public BodyIQUserEventListener(Context context)
    {
        this.context = context;
    }

    @Override
    public void onWearableUserEvent(WearableUserEvent event) {

        switch (event.getUserEvent().getUserEventType())
        {
            case TAPPING:
                WearableUserEvent.TappingEvent tappingEvent = (WearableUserEvent.TappingEvent)event.getUserEvent();
                new CustomToastMessage(context).showMessage(context.getResources().getString(R.string.tappingevent) + tappingEvent.getUserEventType().name());
                break;
            case BUTTON:
                WearableUserEvent.ButtonEvent buttonEvent = (WearableUserEvent.ButtonEvent)event.getUserEvent();
                new CustomToastMessage(context).showMessage(context.getResources().getString(R.string.buttonevent) + buttonEvent.getUserEventType().name());
                break;
        }
    }
}
