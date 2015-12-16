package com.intel.wearable.platform.bodyiq.refapp.apimanager;

import android.content.Context;
import android.graphics.Color;

import com.intel.wearable.cloudsdk.blobstore.CloudPublicBlobStore;
import com.intel.wearable.cloudsdk.core.CloudDownloadPolicy;
import com.intel.wearable.platform.body.Body;
import com.intel.wearable.platform.body.listen.ActivityIntervalListener;
import com.intel.wearable.platform.body.persistence.ActivityIntervalDao;
import com.intel.wearable.platform.bodyiq.refapp.listeners.BodyIQCoreInitListener;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUser;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUserStorageManager;
import com.intel.wearable.platform.core.device.IWearableController;
import com.intel.wearable.platform.core.device.IWearableScanner;
import com.intel.wearable.platform.core.device.WearableControllerFactory;
import com.intel.wearable.platform.core.device.WearableScannerFactory;
import com.intel.wearable.platform.core.device.WearableToken;
import com.intel.wearable.platform.core.device.listeners.IWearableControllerListener;
import com.intel.wearable.platform.core.device.listeners.IWearableScannerListener;
import com.intel.wearable.platform.core.error.*;
import com.intel.wearable.platform.core.event.system.IWearableSystemEventListener;
import com.intel.wearable.platform.core.event.system.SystemEventController;
import com.intel.wearable.platform.core.event.user.IWearableUserEventListener;
import com.intel.wearable.platform.core.event.user.UserEventController;
import com.intel.wearable.platform.core.firmware.IFirmwareController;
import com.intel.wearable.platform.core.notification.WearableNotification;

/**
 * Created by fissaX on 11/18/15.
 */
public class BodyIQAPIManager {



    private static BodyIQAPIManager tdapiManager = null;
    Context context = null;

    BodyIQUser user = null;

    //Scanner
    IWearableScanner scanner = null;

    ActivityIntervalListener listener = null;

    //Body Activities
    ActivityIntervalDao intervalDao = null;

    //WearableController
    IWearableController controller = null;

    //Stores battery updates
    String batteryStatus = "";

    //Cloud
    private final String PRODUCT_ID = "1";
    private CloudPublicBlobStore cloudPublicBlobStore = null;



    private BodyIQAPIManager()
    {

    }


    public void setBatteryStatusUpdate(String batteryStatus)
    {
        this.batteryStatus = batteryStatus;
    }

    /**
     * One BodyIQAPIManager for all the app service
     * @return BodyIQAPIManager instance
     */
    public static BodyIQAPIManager getInstance()
    {
        if(tdapiManager==null)
            tdapiManager = new BodyIQAPIManager();
        return tdapiManager;

    }

    /**
     * Init Core and Body
     * @param context application context
     */
    public void init(Context context, BodyIQCoreInitListener listener)
    {
        this.context = context;
        Body.init(this.context,listener, BodyIQEncryptionKeyUtil.getKey(context));  // This inits Core as well

        scanner = WearableScannerFactory.getDefaultScanner();

        intervalDao = new ActivityIntervalDao(Body.context());

        // checking crash on initializing cloud at this point
        initCloud();

        initUser();

    }



    private void initUser()
    {
        user = BodyIQUserStorageManager.getUser(context);
    }


    private void initCloud()
    {
        final boolean wifiOnly = false;
        final boolean showInDownloadApp = true;
        final String temporaryDir = null;

        cloudPublicBlobStore = new CloudPublicBlobStore(PRODUCT_ID, new CloudDownloadPolicy(wifiOnly, showInDownloadApp, temporaryDir),
                context);

    }

    public BodyIQUser getUser()
    {
        return user;
    }

    public void saveUser()
    {
        BodyIQUserStorageManager.saveUser(context,user);
    }

    public void saveUser(BodyIQUser newUser)
    {
        user = newUser;
        BodyIQUserStorageManager.saveUser(context,user);
    }


    public boolean isUserExist()
    {
        return user!=null;
    }


    public ActivityIntervalDao getBodyActivityAccess()
    {
        return intervalDao;
    }

    public CloudPublicBlobStore getCloudHandler()
    {
        return cloudPublicBlobStore;

    }

    public IFirmwareController getFirmwareInstallController()
    {
        if(controller.isConnected())
            return controller.getFirmwareController();
        else
            return null;

    }

    /**
     * Start BLE Scanner
     * @param listener scan results listener: found devices, error
     */
    public void startScan(IWearableScannerListener listener)
    {
        scanner.startScan(listener);
    }

    /**
     * Stop BLE Scanner
     */
    public void stoptScan()
    {
        scanner.stopScan();
    }


    Object lock = new Object();
    /**
     * connect to BLE Device
     * @param token device to connect to
     * @param listener listen to connection status
     */
    public void connect(WearableToken token, final IWearableControllerListener listener)
    {
        if(controller==null) {
            controller = WearableControllerFactory.getWearableController(token, listener);
        }

        if(!controller.isConnected()) {
            controller.connect();
        }

    }

    /**
     * Disconnect from the current connected BLE device
     */
    public void disconnect()
    {
        if(controller!=null && controller.isConnected() )
            controller.disconnect();
    }


    /**
     * get an async call for current battery status
     */
    public void getSingleBatteryRead()
    {
        if(controller!=null && controller.isConnected() )
            controller.getBatteryStatus();
    }

    /**
     * subscribeToBatteryEvents to the current controller's BLE device Battery Updates
     */
    public void subscribeToBatteryUpdateEvents()
    {
        if(controller!=null && controller.isConnected())
            controller.subscribeToBatteryStatusUpdateEvents();
    }


    /**
     * unsubscribeToBatteryEvents from the current controller's BLE device
     */
    public void unsubscribeToBatteryUpdateEvents()
    {
        if(controller!=null  && controller.isConnected())
            controller.unsubscribeFromBatteryStatusUpdateEvents();
    }


    /**
     * getBatteryStatus gets latest updates battery status
     */
    public String getBatteryStatus()
    {
        return batteryStatus;

    }



    /**
     * subscribeToBodyActivities from the current controller's BLE device
     */
    public void subscribeToBodyActivities(ActivityIntervalListener listener)
    {
        this.listener = listener;
        Body.addActivityListener(listener);
    }

    /**
     * unsubscribeToBodyActivities from the Body Activity events
     */

    public void unsubscribeToBodyActivities()
    {
        if(listener!=null)
            Body.removeActivityListener(listener);
        listener = null;
    }

    /**
     * isSubscribedToBodyActivities returns true if a listener is subscribed to BodyActivites
     */

    public boolean isSubscribedToBodyActivities()
    {
        return listener!=null;
    }


    /**
     * getSerialNumber from the current controller's BLE device
     */
    public String getSerialNumber()
    {

        if(controller!=null  && controller.isConnected())
            return controller.getWearableIdentity().getSerialNumber();
        return "";
    }


    /**
     * returns true if the controller was set to a specific IWearableToken and is connected to it
     * else it returns false
     */
    public boolean isConnected()
    {
        return (controller==null) ? false : controller.isConnected();
    }

    /**
     * getFirmwareRevision from the current controller's BLE device
     */

    public String getFirmwareRevision()
    {

        if(controller!=null  && controller.isConnected())
            return controller.getWearableIdentity().getFirmwareRevision();
        return "";
    }

    /**
     * getSoftwareRevision from the current controller's BLE device
     */

    public String getSoftwareRevision()
    {

        if(controller!=null && controller.isConnected())
            return controller.getWearableIdentity().getSoftwareRevision();
        return "";
    }

    /**
     * getSoftwareRevision from the current controller's BLE device
     */

    public String getAddress()
    {
        if(controller!=null  && controller.isConnected())
            return controller.getWearableIdentity().getAddress();
        return "";
    }

    public String getManufacturer()
    {
        if(controller!=null  && controller.isConnected())
            return controller.getWearableIdentity().getManufacturer();
        return "";
    }

    public String getModelName()
    {
        if(controller!=null  && controller.isConnected())
            return controller.getWearableIdentity().getModel();
        return "";
    }


    public String getHardwareRevision()
    {
        if(controller!=null  && controller.isConnected())
            return controller.getWearableIdentity().getHardwareRevision();
        return "";
    }



    public String getDisplayName()
    {
        if (controller != null  && controller.isConnected())
            return controller.getWearableIdentity().getDisplayName();
        return "";
    }

    public boolean sendLedNotification(WearableNotification.LedPattern.Type type,int colors[],int repeatCnt, int intensity,int onDuration, int offDuration)
    {
        boolean res = false;

        WearableNotification.LedPattern ledPattern =
                new WearableNotification.LedPattern(type, 0, null, 2, intensity);

        for(int color : colors)
        {
            ledPattern.addRGBColor(new WearableNotification.RGBColor(Color.red(color), Color.green(color), Color.blue(color)));
            ledPattern.addDuration(
                    new WearableNotification.DurationPattern(onDuration, offDuration));
        }

        if(controller!=null && controller.isConnected()) {
            controller.getNotificationController().sendNotification(new WearableNotification(ledPattern));
            res = true;
        }

        return res;
    }


    public boolean sendHapticNotification(WearableNotification.VibrationPattern.Type type,int repeatCnt, int amplitude, int durationOn, int durationOff)
    {
        boolean res = false;

        WearableNotification.DurationPattern durationPattern = new WearableNotification.DurationPattern(durationOn,durationOff);
        WearableNotification.VibrationPattern vibrationPattern =
                new WearableNotification.VibrationPattern(type,amplitude, repeatCnt,durationPattern);
        if(controller!=null && controller.isConnected()) {
            controller.getNotificationController().sendNotification(new WearableNotification(vibrationPattern));
            res = true;
        }
        return res;
    }


    public void subscribeToUserEvents(IWearableUserEventListener listener)
    {
        UserEventController.subscribe(listener);
    }

    public void unsubscribeToUserEvents()
    {
        UserEventController.unsubscribe();

    }

    public void subscribeToSystemEvents(IWearableSystemEventListener listener)
    {
        SystemEventController.subscribe(listener);

    }

    public void unsubscribeToSystemEvents()
    {
        SystemEventController.unsubscribe();

    }


}
