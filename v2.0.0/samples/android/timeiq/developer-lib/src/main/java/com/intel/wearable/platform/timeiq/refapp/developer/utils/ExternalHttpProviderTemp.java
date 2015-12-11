package com.intel.wearable.platform.timeiq.refapp.developer.utils;

import com.intel.wearable.platform.timeiq.api.common.result.ResultCode;
import com.intel.wearable.platform.timeiq.api.common.result.ResultData;
import com.intel.wearable.platform.timeiq.common.factory.CommonClassPool;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.common.logger.ITSOLogger;
import com.intel.wearable.platform.timeiq.common.logger.TSOLoggerConst;
import com.intel.wearable.platform.timeiq.common.preferences.BuildPrefs;
import com.intel.wearable.platform.timeiq.common.preferences.IBuildPrefs;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.Charset;
import java.util.Map;
import java.util.Set;

/**
 * Created by mleib on 26/02/2015.
 */
public class ExternalHttpProviderTemp {

    private static final String TAG = TSOLoggerConst.TAG + ExternalHttpProviderTemp.class.getName();
    private static ITSOLogger logger = CommonClassPool.getTSOLogger();
    private static IBuildPrefs buildConfiguration;

    public static ResultData<String> httpGet(String url, Map<String,String> headers, Charset charset) {
        HttpURLConnection connection;
        ResultData<String> result = null;
        try
        {
            URL urlToRequest = new URL(url);
            connection = (HttpURLConnection)urlToRequest.openConnection();
            connection.setDoInput(true);
            connection.setUseCaches(false);
            connection.setRequestMethod("GET");

            if (buildConfiguration == null) {
                buildConfiguration = ClassFactory.getInstance().resolve(IBuildPrefs.class);
            }

            connection.setConnectTimeout(buildConfiguration.getInt(BuildPrefs.INT_CONNECTION_TIMEOUT));
            connection.setReadTimeout(buildConfiguration.getInt(BuildPrefs.INT_DATA_RETRIEVAL_TIMEOUT));
            Set<String> types = headers.keySet();
            for (String type: types)
            {
                connection.setRequestProperty(type, headers.get(type));
            }

            int statusCode = connection.getResponseCode();
            if (statusCode == HttpURLConnection.HTTP_OK  ) {
                DataInputStream is = new DataInputStream(connection.getInputStream());
                BufferedReader reader = new BufferedReader(new InputStreamReader(is, charset));
                StringBuilder sBuf = new StringBuilder();
                String line;
                try {
                    while ((line = reader.readLine()) != null) {
                        sBuf.append(line);
                    }

                    result = new ResultData<String>(ResultCode.SUCCESS, sBuf.toString());
                } catch (Exception e) {
                    result = new ResultData<String>(ResultCode.GENERAL_EXCEPTION_WAS_THROWN, e.getMessage(), null);
                    logger.addToLog(true, TAG, "Exception at httpGet [1]  Message: " + e.getMessage() + "  Cause:" + e.getCause());
                    e.printStackTrace();
                } finally {
                    try {
                        is.close();
                    } catch (Exception e) {
                        if (result == null) {
                            result = new ResultData<String>(ResultCode.GENERAL_EXCEPTION_WAS_THROWN, e.getMessage(), null);
                        }
                        logger.addToLog(true, TAG, "Exception at httpGet [2]  Message: " + e.getMessage() + "  Cause:" + e.getCause());
                        e.printStackTrace();
                    }
                }
            } else if (statusCode == HttpURLConnection.HTTP_NO_CONTENT) {
                result = new ResultData<String>(ResultCode.NETWORK_HTTP_NO_CONTENT, null);
                logger.addToLog(false, TAG,"Response : NO CONTENT" );
            } else {
                result = new ResultData<String>(ResultCode.GENERAL_SERVER_ERROR, null);
                logger.addToLog(false, TAG,"Server Internal Error ,status code is" + statusCode );
            }

        } catch (Throwable t) {
            result = new ResultData<String>(ResultCode.GENERAL_EXCEPTION_WAS_THROWN, t.getMessage(), null);
            logger.addToLog(true, TAG, "Exception at httpGet [3]  Message: " + t.getMessage() + "  Cause:" + t.getCause());
            t.printStackTrace();
        }

        logger.addToLog(false, TAG, "GetURL: " +url+ " | response: " + result + "|");

        return result;
    }
}
