package com.intel.wearable.platform.timeiq.refapp.auth;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;

import com.intel.wearable.cloudsdk.core.AuthCredentials;
import com.intel.wearable.cloudsdk.core.CloudUAAAuthProvider;
import com.intel.wearable.cloudsdk.core.ICloudAuthLoginCb;
import com.intel.wearable.cloudsdk.core.errors.CloudError;
import com.intel.wearable.platform.timeiq.api.common.auth.TSOUserInfo;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;


/**
 * Created by orencoh2 on 23/3/15.
 */
public class CSWAuthCredentialsProvider implements ICredentialsProvider {


    private static final String TAG = CSWAuthCredentialsProvider.class.getName();

    private static final String PREF_FILE = "CSWUserAuth";

    private static final String IDENTIFIER_FIELD = "identifier";
    private static final String USER_FIELD = "username";
    private static final String PASS_FIELD = "password";


    private final Context m_context;

    private TSOUserInfo m_userInfo;

    private AuthCredentials m_credentials;

    private static final Object m_loginLock = new Object();
    private boolean m_loginInProgress = false;
    private boolean m_loggedIn = false;

    private static CSWAuthCredentialsProvider m_instance;
    private String mLoginError;


    public static CSWAuthCredentialsProvider getInstance(Context context) {
        if (m_instance == null) {
            m_instance = new CSWAuthCredentialsProvider(context);
        }

        return m_instance;

    }

    private CSWAuthCredentialsProvider(Context context) {
        m_context = context;
    }

    @Override
    public boolean setUserInfo(TSOUserInfo userInfo) {

        m_userInfo = userInfo;

        return true;

    }

    @Override
    public synchronized void loadData() {

        SharedPreferences prefs = m_context.getSharedPreferences(PREF_FILE, Context.MODE_PRIVATE);


        String identifier = prefs.getString(IDENTIFIER_FIELD, null);
        String user = prefs.getString(USER_FIELD, null);
        String pass = prefs.getString(PASS_FIELD, null);

        m_userInfo = new TSOUserInfo(identifier, user, pass);

    }

    @Override
    public synchronized boolean saveData() {

        SharedPreferences.Editor editor = m_context.getSharedPreferences(PREF_FILE, Context.MODE_PRIVATE).edit();
        editor.putString(IDENTIFIER_FIELD, m_userInfo.getIdentifier());
        editor.putString(USER_FIELD, m_userInfo.getUserName());
        editor.putString(PASS_FIELD, m_userInfo.getPassword());
        editor.commit();

        return true;
    }


    @Override
    public boolean isUserLoggedIn() {

        if (m_loginInProgress) {

            synchronized (m_loginLock) {
                try {
                    m_loginLock.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        return m_loggedIn;

    }

    @Override
    public TSOUserInfo getUserInfo() {
        return m_userInfo;
    }

    @Override
    public Object getCredentials() {
        return m_credentials;
    }


    @Override
    public ResultObject<Boolean> login() {
        boolean loginOK = doCloudLogin() /* doesn't mean we are logged-in, verify with --> */ && isUserLoggedIn();
        return new  ResultObject<Boolean>(loginOK, mLoginError, null);
    }

    private boolean doCloudLogin() {
        mLoginError = null;
        boolean missingUserInfo = (m_userInfo == null);
        boolean missingUserName = (m_userInfo.getUserName() == null);
        boolean missingPassword = (m_userInfo.getPassword() == null);
        if ( missingUserInfo || missingUserName || missingPassword) {//The email address or password you entered is incorrect
            String missingUserInfoStr = (missingUserInfo ? m_context.getString(R.string.missing_user_info) : "");
            String missingUserNameStr = (missingUserName ? m_context.getString(R.string.missing_email) : "");
            String missingPasswordStr = (missingPassword ? m_context.getString(R.string.missing_password) : "");
            mLoginError = m_context.getString(R.string.missing_credentials, missingUserInfoStr, missingUserNameStr, missingPasswordStr);
            Log.e(TAG, "attempt to login with invalid "+mLoginError);
            return false;
        }

        m_loginInProgress = true;

        final CloudUAAAuthProvider cloudMgr = new CloudUAAAuthProvider(m_context);


        Log.d(TAG, "Attempt to login with user:" + m_userInfo.getUserName());

        cloudMgr.login(m_userInfo.getUserName(), m_userInfo.getPassword(), new ICloudAuthLoginCb() {
            @Override
            public void onFailed(final CloudError cloudError) {

                if (cloudError != null) {
                    Log.e(TAG, "Login failed - " + cloudError.toString());
                    int cloudErrorCode = cloudError.getCode();
                    if (cloudErrorCode == 401) {
                        mLoginError = m_context.getString(R.string.wrong_credentials);
                    } else {
                        mLoginError = m_context.getString(R.string.cloud_error_code, cloudErrorCode);
                    }
                }

                synchronized (m_loginLock) {
                    m_loginInProgress = false;
                    m_loginLock.notifyAll();
                }
            }

            @Override
            public void onSuccess() {

                Log.v(TAG, "Login successful");

                m_credentials = cloudMgr.getCredentials();

                updateUserInfo();

                m_loggedIn = true;

                synchronized (m_loginLock) {
                    m_loginInProgress = false;
                    m_loginLock.notifyAll();
                }

            }

        });

        return true;
    }

    /**
     * Update identifier according to the credentials information
     */
    private void updateUserInfo() {
        if (m_userInfo != null) {
            m_userInfo = new TSOUserInfo(createUserIdentifier(), m_userInfo.getUserName(), m_userInfo.getPassword());
        }

    }

    private String createUserIdentifier() {
        if ((m_credentials != null) && (m_credentials.getUserId() != null) && (m_credentials.getClientId() != null)) {
            return m_credentials.getUserId() + "@" + m_credentials.getClientId();
        } else {
            return null;
        }

    }

}
