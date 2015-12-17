package com.intel.wearable.platform.bodyiq.refapp.apimanager;

import android.os.Environment;

import com.intel.wearable.cloudsdk.core.CloudDownloadResponse;
import com.intel.wearable.cloudsdk.core.CloudResponse;
import com.intel.wearable.cloudsdk.core.ICloudDownloadCb;
import com.intel.wearable.cloudsdk.core.ICloudResponseCb;
import com.intel.wearable.platform.bodyiq.refapp.activities.FirmwareActivity;
import com.intel.wearable.platform.core.device.IWearableController;
import com.intel.wearable.platform.core.error.*;
import com.intel.wearable.platform.core.firmware.IFirmwareInstallListener;
import com.intel.wearable.platform.core.util.Logger;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;

/**
 * FirmwareHandler check, download and uploads firmware.
 * Takes care on adding/removing a progress bar and reporting back error to user
 */
public class BodyIQFirmwareManager {


    final String SW_ASSET_INFO      = "swAssetInfo";
    final String SW_ASSET_ID        = "assetId";
    final String SW_ASSET_VERSION   = "assetVersion";
    final String assetType          = "Firmware";
    String cloudAssetID             = "";
    String fileDirectory            = "/TDFirmware/";
    FirmwareActivity firmwareActivity       = null;

    public BodyIQFirmwareManager(FirmwareActivity firmwareActivity)
    {
        this.firmwareActivity = firmwareActivity;
    }


    public boolean checkForUpdate()
    {

        final String currentVersion = BodyIQAPIManager.getInstance().getFirmwareInstallController().getWearableFirmwareVersion();
        BodyIQAPIManager.getInstance().getCloudHandler().getLatestDownloadInfo(assetType, new ICloudResponseCb() {
            @Override
            public void onSuccess(CloudResponse cloudResponse) {

                JSONObject jsonObject = cloudResponse.getPayload();
                if(jsonObject!=null)
                {
                    try {
                        JSONObject objAsset = jsonObject.getJSONObject(SW_ASSET_INFO);

                        cloudAssetID = objAsset.getString(SW_ASSET_ID);
                        String cloudVersion = objAsset.getString(SW_ASSET_VERSION);

                        if(isNewVersion(currentVersion,cloudVersion))
                        {
                            firmwareActivity.askToDownloadNewVersion();
                        }
                    }
                    catch (JSONException e)
                    {
                        Logger.e(e.getMessage());

                    }


                }



            }

            @Override
            public void onFailed(CloudResponse cloudResponse) {



            }
        });
        return false;

    }


    private boolean isNewVersion(String currentVersion,String cloudVersion)
    {
        boolean res = false;
        try {

            String current[] = currentVersion.split("\\.");
            String  cloud[] = cloudVersion.split("\\.");

            for(int i=0; i<cloud.length; i++)
            {

                if(Integer.parseInt(cloud[i])>Integer.parseInt(current[i]))
                {
                    Logger.d("Cloud version is newer than current version");
                    res = true;
                    break;

                }
            }
        }
        catch (Exception e)
        {
            Logger.e("Unexpected version format currentVersion: "+currentVersion + " or: cloudVersion" + cloudVersion);
        }

        return res;
    }

    public void dowanload()
    {

        String path = Environment.getExternalStorageDirectory().getPath();
        BodyIQAPIManager.getInstance().getCloudHandler().download(cloudAssetID, path+fileDirectory, cloudAssetID, new ICloudDownloadCb() {
            @Override
            public void onStarted(CloudDownloadResponse cloudDownloadResponse) {
                Logger.d("Donwload version started");


            }

            @Override
            public void onFailed(CloudDownloadResponse cloudDownloadResponse) {
                Logger.d("Donwload version failed ");

            }

            @Override
            public void onFinished(CloudDownloadResponse cloudDownloadResponse) {

                Logger.d("Donwload version finished");

                String absPath = firmwareActivity.getApplicationContext().getExternalFilesDir(null).getAbsoluteFile().getAbsolutePath() + File.separator + cloudAssetID;
                File firmwareFile = new File(absPath);
                Logger.d("firmwareFile.exists(): " + firmwareFile.exists());
                Logger.d("firmwareFile.getAbsolutePath(): " + firmwareFile.getAbsolutePath());


                firmwareActivity.askToUploadNewVersion();

            }

            @Override
            public void onProgress(int i, int i1) {

                Logger.d("Cloud progress "+ i + "  "+i1);


            }
        });

    }




    public void upload()
    {
        String path = Environment.getExternalStorageDirectory().getPath();

        File file = new File(path+fileDirectory+cloudAssetID);
        BodyIQAPIManager.getInstance().getFirmwareInstallController().installFirmware(file, new IFirmwareInstallListener() {
            @Override
            public void onFirmwareInstallStarted(IWearableController controller) {
                Logger.d("Upload onFirmwareInstallStarted");
            }

            @Override
            public void onFirmwareInstallProgress(IWearableController controller, int progress, int total) {
                Logger.d("Upload progress "+ progress + "  "+total);

            }

            @Override
            public void onFirmwareInstallComplete(IWearableController controller) {
                Logger.d("Upload onFirmwareInstallComplete");
                firmwareActivity.finishedUpload(true);
            }

            @Override
            public void onFirmwareInstallError(IWearableController controller, com.intel.wearable.platform.core.error.Error error) {
                Logger.d("Upload onFirmwareInstallError "+error.getErrorMessage());
                firmwareActivity.finishedUpload(false);
            }
        });


    }
}
