package com.intel.wearable.platform.timeiq.refapp.apitoolbox;

import android.content.Context;

import com.intel.wearable.platform.timeiq.api.common.auth.IAuthCredentialsProvider;
import com.intel.wearable.platform.timeiq.api.common.messageHandler.IMessageHandler;
import com.intel.wearable.platform.timeiq.api.common.messageHandler.IMessageListener;
import com.intel.wearable.platform.timeiq.refapp.TimeIQBGService;

/**
 * Created by smoradof on 10/28/2015.
 */
public final class TimeIQInitUtils {

    /**
     * init TimeIQ from activity, and starts its engines.
     * this should be called only after registration to identity was successful.
     * @param ctx
     */
    public static boolean initTimeIQ(Context ctx, IAuthCredentialsProvider authProvider, String cloudServerURL){
        return TimeIQBGService.mTimeIQApi.init(ctx, authProvider, cloudServerURL);
    }



    /**
     *
     * @param messageListener an interface that listens to messages from the TimeIQApi
     */
    public static void initTimeIQMessageListener(IMessageListener messageListener){

    // Add listener to TimeIQ messages:
        IMessageHandler timeIQMessageHandler = TimeIQBGService.mTimeIQApi.getMessageHandler();
        timeIQMessageHandler.register(messageListener); // register only adds a listener. Messages will be queued until the messageHandler will be initialized.
        timeIQMessageHandler.init(); // start getting messages
    }


    /**
     * Clean up resources
     * @param messageListener
     */
    public static void onDestroyService(IMessageListener messageListener){
        IMessageHandler timeIQMessageHandler = TimeIQBGService.mTimeIQApi.getMessageHandler();
        timeIQMessageHandler.unRegister(messageListener);
        TimeIQBGService.mTimeIQApi.onDestroy();
    }

}
