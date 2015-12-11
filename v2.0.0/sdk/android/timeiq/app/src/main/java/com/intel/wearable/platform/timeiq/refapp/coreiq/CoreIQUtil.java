package com.intel.wearable.platform.timeiq.refapp.coreiq;

import android.content.Context;
import android.content.SharedPreferences;
import android.text.TextUtils;
import android.util.Log;

import com.google.gson.Gson;
import com.intel.wearable.platform.core.Core;
import com.intel.wearable.platform.core.device.IWearableController;
import com.intel.wearable.platform.core.device.IWearableScanner;
import com.intel.wearable.platform.core.device.WearableBatteryStatus;
import com.intel.wearable.platform.core.device.WearableControllerFactory;
import com.intel.wearable.platform.core.device.WearableScannerFactory;
import com.intel.wearable.platform.core.device.WearableToken;
import com.intel.wearable.platform.core.device.listeners.IWearableControllerListener;
import com.intel.wearable.platform.core.event.user.IWearableUserEventListener;
import com.intel.wearable.platform.core.event.user.UserEventController;
import com.intel.wearable.platform.core.event.user.WearableUserEvent;
import com.intel.wearable.platform.core.model.datastore.WearableIdentity;
import com.intel.wearable.platform.core.notification.INotificationController;
import com.intel.wearable.platform.core.notification.WearableNotification;
import com.intel.wearable.platform.timeiq.refapp.bodyiq.BodyIQUtil;

import java.util.ArrayList;
import java.util.concurrent.TimeUnit;

/**
 * Created by smoradof on 11/30/2015.
 */
public class CoreIQUtil implements IWearableControllerListener, IWearableUserEventListener {

    private static final String TAG = CoreIQUtil.class.getSimpleName();

    private static final String CORE_TOKEN_PREFS_FILE = "coreTokenPrefs";
    private static final String CORE_TOKEN_KEY = "coreTokenKey";
    private static final String USER_DISCONNECTED_OR_UNPAIRED_KEY = "userDisconnectedOrUnpaired";
    private static final long SEND_TO_DEVICE_EXPIRATION_TIME = TimeUnit.SECONDS.toMillis(60);

    private static CoreIQUtil mCoreIQUtil = new CoreIQUtil();

    private static IWearableControllerListener mOuterWearableControllerListener;
    private static WearableToken mCoreIQToken;
    private static IWearableController mController;
    IWearableScanner mScanner;
    private boolean mIsInitialized = false;
    private Context mContext;

    private boolean mUserTryedToUnpairOrDisconnect;
    private ITappingEventListener mTappingEventListener;

    private IConnectionListener mConnectionListener;

    private ArrayList<SendToDeviceData> mSendToDeviceDataList = new ArrayList<>();

    public static CoreIQUtil getInstance(){
        return mCoreIQUtil;
    }

    public CoreIQUtil(){
        Log.d(TAG, "constructor");
    }

    public void init(Context context){
        Log.d(TAG, "init");
        if( ! mIsInitialized ) {
            mIsInitialized = true;
            Log.d(TAG, "init: initializing");
            Core.init(context);
            if(BodyIQUtil.USE_BODY_IQ) {
                BodyIQUtil.getInstance().init(context);
            }
            mContext = context;
            SharedPreferences sharedPreferences = context.getSharedPreferences(CORE_TOKEN_PREFS_FILE, Context.MODE_PRIVATE);
            String jsonString = sharedPreferences.getString(CORE_TOKEN_KEY, null);
            if (!TextUtils.isEmpty(jsonString)) {
                mCoreIQToken = new Gson().fromJson(jsonString, WearableToken.class);
                Log.d(TAG, "init got token for: " + mCoreIQToken.getDisplayName());
                mUserTryedToUnpairOrDisconnect = sharedPreferences.getBoolean(USER_DISCONNECTED_OR_UNPAIRED_KEY, false);
                if( ! mUserTryedToUnpairOrDisconnect ){
                    createCoreIQTController();
                    connect();
                }
            }
        }
    }


    public IWearableScanner getScanner(){
        if(mScanner == null){
            Log.d(TAG, "getScanner: getDefaultScanner");
            mScanner = WearableScannerFactory.getDefaultScanner();
        }
        return mScanner;
    }

    public void setCoreIQToken(WearableToken coreIQToken) {
        if(coreIQToken != null) {
            Log.d(TAG, "setCoreIQToken not null");
            mCoreIQToken = coreIQToken;
            String jsonString = new Gson().toJson(coreIQToken);
            SharedPreferences sharedPreferences = mContext.getSharedPreferences(CORE_TOKEN_PREFS_FILE, Context.MODE_PRIVATE);
            sharedPreferences.edit().putString(CORE_TOKEN_KEY, jsonString).commit();
            createCoreIQTController();
        }
        else {
            Log.d(TAG, "setCoreIQToken null");
        }
    }

    private IWearableController createCoreIQTController(){
        Log.d(TAG, "createCoreIQTController");
        if(mController != null){
            WearableToken wearableToken = mController.getWearableToken();
            if(mCoreIQToken == null || ! wearableToken.getAddress().equals(mCoreIQToken.getAddress()) ){
                mController = null;
            }
        }
        if(mController == null && mCoreIQToken != null){
            Log.d(TAG, "createCoreIQTController creating a new controller");
            mController = WearableControllerFactory.getWearableController(mCoreIQToken, this);
        }
        return mController;
    }


    public void setCoreIQTControllerListener(IWearableControllerListener listener){
        mOuterWearableControllerListener = listener;
    }


    private class SendToDeviceData{
        int r;
        int g;
        int b;
        int ledIntensity;
        byte vibrationIntensity;
        long expirationTime;

        public SendToDeviceData(int r, int g, int b, int ledIntensity, byte vibrationIntensity, long expirationTime) {
            this.r = r;
            this.g = g;
            this.b = b;
            this.ledIntensity = ledIntensity;
            this.vibrationIntensity = vibrationIntensity;
            this.expirationTime = expirationTime;
        }
    }

    public boolean tryToReconnectIfNeeded(){
        boolean alreadyConnected = false;
        createCoreIQTController();
        if (mController != null && !mUserTryedToUnpairOrDisconnect) {
            if (!mController.isConnected()) {
                try {
                    mController.connect();
                    Log.d(TAG, "tryToReconnectIfNeeded: connecting OK");
                } catch (Throwable throwable) {
                    Log.d(TAG, "tryToReconnectIfNeeded: connecting error:" +  throwable.getMessage());
                }
            }
            else {
                alreadyConnected = true;
            }
        }
        return alreadyConnected;
    }

    public String sendToDevice(int r, int g, int b, int ledIntensity, byte vibrationIntensity) {
        String errorStr;
        createCoreIQTController();
        if (mController != null && !mUserTryedToUnpairOrDisconnect) {
            if (mController.isConnected()) {
                errorStr = sendToDeviceInternal(r, g, b, ledIntensity, vibrationIntensity);
            } else {

                errorStr = "device not connected. trying to connect, and try to resend with expiration of "+SEND_TO_DEVICE_EXPIRATION_TIME+" milliseconds";
                synchronized (mSendToDeviceDataList) {
                    mSendToDeviceDataList.add(new SendToDeviceData(r, g, b, ledIntensity, vibrationIntensity, System.currentTimeMillis() + SEND_TO_DEVICE_EXPIRATION_TIME));
                }
                try {
                    mController.connect();
                    Log.d(TAG, "connect: OK");
                } catch (Throwable throwable) {
                    errorStr += throwable.getMessage();
                    Log.d(TAG, "connect: " + errorStr);
                }
            }
        }
        else{
            errorStr = "no controller";
        }
        Log.d(TAG, "sendToDevice: "+errorStr);
        return errorStr;
    }

    private String sendToDeviceInternal(int r, int g, int b, int ledIntensity, byte vibrationIntensity){
        String errorStr;
        if(mController.isConnected()) {
            //led pattern:
            WearableNotification.LedPattern ledPattern = new WearableNotification.LedPattern(WearableNotification.LedPattern.Type.LED_BLINK, 0, null, 2, ledIntensity);
            ledPattern.addRGBColor(new WearableNotification.RGBColor(r, g, b));
            ledPattern.addDuration(new WearableNotification.DurationPattern(500, 500));

            WearableNotification wearableNotification;
            // Vibration Pattern:
            if(vibrationIntensity > 0) {
                WearableNotification.VibrationPattern vibrationPattern =
                        new WearableNotification.VibrationPattern(WearableNotification.VibrationPattern.Type.VIBRA_SQUARE, vibrationIntensity, 3);
                vibrationPattern.addDuration(new WearableNotification.DurationPattern(500, 500));

                wearableNotification = new WearableNotification(vibrationPattern, ledPattern, 0);
            }
            else{
                wearableNotification = new WearableNotification(ledPattern);
            }
            INotificationController notificationController = mController.getNotificationController();
            if(notificationController != null) {
                notificationController.sendNotification(wearableNotification);
                errorStr = "notificationController.sendNotification";
            }
            else{
                errorStr = "notificationController = null";
            }
        }
        else{
            errorStr = "device not connected";
        }
        Log.d(TAG, "sendToDevice: "+errorStr);
        return errorStr;
    }


    public String pair(){
        Log.d(TAG, "pair ...");
        onUserTriedToDisconnectOrUnpair(false);
        String errorMsg = null;
        if(mController != null) {
            try {
                Log.d(TAG, "mController.pair()");
                mController.pair();
            } catch (Throwable throwable) {
                errorMsg = throwable.getMessage();
                Log.d(TAG, "pair: " + errorMsg);
            }
        }
        return errorMsg;
    }

    public void unpair(){
        Log.d(TAG, "unpair ...");
        onUserTriedToDisconnectOrUnpair(true);
        createCoreIQTController();
        if(mController != null) {
            mController.unpair();
        }
    }

    public String connect(){
        Log.d(TAG, "connect ...");
        onUserTriedToDisconnectOrUnpair(false);
        String errorMsg = null;
        createCoreIQTController();
        if(mController != null) {
            try {
                mController.connect();
                Log.d(TAG, "connect: OK");
            } catch (Throwable throwable) {
                errorMsg = throwable.getMessage();
                Log.d(TAG, "connect: " + errorMsg);
            }
        }
        return errorMsg;
    }

    public boolean disconnect(){
        Log.d(TAG, "disconnect ...");
        onUserTriedToDisconnectOrUnpair(true);
        boolean success = false;
        createCoreIQTController();
        if(mController != null) {
            success = mController.disconnect();
        }
        return success;
    }

    public boolean isConnected(){
        boolean isConnected = false;
        createCoreIQTController();
        if(mController != null){
            isConnected = mController.isConnected();
        }
        return isConnected;
    }

    public boolean isPaired(){
        boolean isPaired = false;
        if(mController != null){
            isPaired = mController.isPaired();
        }
        return isPaired;
    }

    public void getBatteryStatusAsync(){
        if(mController != null){
             mController.getBatteryStatus();
        }
    }

    private void onUserTriedToDisconnectOrUnpair(boolean userTryedToUnpairOrDisconnect){
        if(userTryedToUnpairOrDisconnect != mUserTryedToUnpairOrDisconnect) {
            mUserTryedToUnpairOrDisconnect = userTryedToUnpairOrDisconnect;
            SharedPreferences sharedPreferences = mContext.getSharedPreferences(CORE_TOKEN_PREFS_FILE, Context.MODE_PRIVATE);
            sharedPreferences.edit().putBoolean(USER_DISCONNECTED_OR_UNPAIRED_KEY, mUserTryedToUnpairOrDisconnect).commit();
        }
    }

    @Override
    public void onConnecting(IWearableController iWearableController) {
        Log.d(TAG, "onConnecting");
        if(mOuterWearableControllerListener != null){
            mOuterWearableControllerListener.onConnecting(iWearableController);
        }
    }

    @Override
    public void onConnected(IWearableController iWearableController) {
        Log.d(TAG, "onConnected");
        if(mOuterWearableControllerListener != null){
            mOuterWearableControllerListener.onConnected(iWearableController);
        }
        UserEventController.subscribe(this);
        // we connected successfully - so try to run the messages one by one:
        long now = System.currentTimeMillis();
        synchronized (mSendToDeviceDataList) {
            for (SendToDeviceData sendToDeviceData : mSendToDeviceDataList) {
                if (sendToDeviceData.expirationTime > now) {
                    String msg = sendToDeviceInternal(sendToDeviceData.r, sendToDeviceData.g, sendToDeviceData.b, sendToDeviceData.ledIntensity, sendToDeviceData.vibrationIntensity);
                    Log.d(TAG, "connect: send To Device waiting list: "+msg);
                }
                else{
                    Log.d(TAG, "connect: send To Device waiting list: expired");
                }
            }
            mSendToDeviceDataList.clear();
        }
        if(mConnectionListener != null){
            mConnectionListener.onConnect();
        }
    }

    @Override
    public void onDisconnecting(IWearableController iWearableController) {
        Log.d(TAG, "onDisconnecting");
        if(mOuterWearableControllerListener != null){
            mOuterWearableControllerListener.onDisconnecting(iWearableController);
        }
        UserEventController.unsubscribe();
        if(mConnectionListener != null){
            mConnectionListener.onDisconnecting();
        }
    }

    @Override
    public void onDisconnected(IWearableController iWearableController) {
        Log.d(TAG, "onDisconnected");
        if(mOuterWearableControllerListener != null){
            mOuterWearableControllerListener.onDisconnected(iWearableController);
        }
    }

    @Override
    public void onPairedStatusChanged(IWearableController iWearableController, boolean b) {
        Log.d(TAG, "onPairedStatusChanged: "+b);
        if(mOuterWearableControllerListener != null){
            mOuterWearableControllerListener.onPairedStatusChanged(iWearableController, b);
        }
    }

    @Override
    public void onBatteryStatusUpdate(IWearableController iWearableController, WearableBatteryStatus wearableBatteryStatus) {
        if(mOuterWearableControllerListener != null){
            mOuterWearableControllerListener.onBatteryStatusUpdate(iWearableController, wearableBatteryStatus);
        }
    }

    @Override
    public void onFailure(IWearableController iWearableController, com.intel.wearable.platform.core.error.Error error) {
        Log.d(TAG, "onFailure: "+error.getErrorMessage());
        if(mOuterWearableControllerListener != null){
            mOuterWearableControllerListener.onFailure(iWearableController, error);
        }
    }

    public interface ITappingEventListener{
        void onDoubleTap();
        void onTrippleTap();
    }


    public interface IConnectionListener{
        void onConnect();
        void onDisconnecting();
    }

    public void setConnectionListener(IConnectionListener connectionListener)
    {
        mConnectionListener = connectionListener;
    }

    public void setTappingEventListener( ITappingEventListener tappingEventListener){
        mTappingEventListener = tappingEventListener;
    }

    @Override
    public void onWearableUserEvent(WearableUserEvent event) {
        if(mTappingEventListener != null) {
            WearableUserEvent.UserEvent userEvent = event.getUserEvent();
            if (userEvent.getUserEventType().equals(WearableUserEvent.UserEventType.TAPPING)) {
                WearableUserEvent.TappingEvent tappingEvent = (WearableUserEvent.TappingEvent) userEvent;
                WearableUserEvent.TappingEventType tappingEventType = tappingEvent.getTappingEventType();
                switch (tappingEventType) {

                    case DOUBLE_TAP:
                        Log.d(TAG, "DOUBLE_TAP");
                        mTappingEventListener.onDoubleTap();
                        break;
                    case TRIPLE_TAP:
                        Log.d(TAG, "TRIPLE_TAP");
                        mTappingEventListener.onTrippleTap();
                        break;
                    case UNKNOWN:
                        break;
                }
            }
        }
    }

    public WearableToken getCoreIQToken(){
        return mCoreIQToken;
    }

    public String getDeviceSoftwareRevision(){
        String deviceSoftwareRevision = null;
        if(mController != null){
            final WearableIdentity wearableIdentity = mController.getWearableIdentity();
            if(wearableIdentity != null) {
                deviceSoftwareRevision = wearableIdentity.getSoftwareRevision();
            }
        }
        return deviceSoftwareRevision;
    }
}
