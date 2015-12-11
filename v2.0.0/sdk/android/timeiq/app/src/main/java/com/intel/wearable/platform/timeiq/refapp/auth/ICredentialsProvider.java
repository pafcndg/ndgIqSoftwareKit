package com.intel.wearable.platform.timeiq.refapp.auth;


import com.intel.wearable.platform.timeiq.api.common.auth.IAuthCredentialsProvider;
import com.intel.wearable.platform.timeiq.api.common.auth.TSOUserInfo;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;

/**
 * Created by orencoh2 on 10/29/15.
 */
public interface ICredentialsProvider extends IAuthCredentialsProvider {
    boolean setUserInfo(TSOUserInfo userInfo);
    void loadData();
    boolean saveData();

    ResultObject<Boolean> login();


}
