package com.intel.wearable.platform.timeiq.refapp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

/**
 * Created by smoradof on 10/11/2015.
 */
public class AutoStartReceiver extends BroadcastReceiver
{

    @Override
    public void onReceive( Context context, Intent intent )
    {
        Intent callIntent = new Intent( context, TimeIQBGService.class );
        context.startService( callIntent );
    }
}