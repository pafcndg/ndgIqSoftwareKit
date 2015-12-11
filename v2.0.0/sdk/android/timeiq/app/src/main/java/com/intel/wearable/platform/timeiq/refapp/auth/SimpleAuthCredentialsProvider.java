package com.intel.wearable.platform.timeiq.refapp.auth;

/**
 * Created by orencoh2 on 10/29/15.
 */

import android.content.Context;
import android.content.SharedPreferences;

import com.intel.wearable.platform.timeiq.api.common.auth.TSOUserInfo;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;


public class SimpleAuthCredentialsProvider implements ICredentialsProvider {

    private static final String PREF_FILE = "UserAuth";

    private static final String IDENTIFIER_FIELD ="identifier";
    private static final String USER_FIELD ="username";
    private static final String PASS_FIELD ="password";


    private final Context m_context;

    private TSOUserInfo m_userInfo;

    private boolean m_bLoggedIn;

    private static SimpleAuthCredentialsProvider m_instance;


    public static SimpleAuthCredentialsProvider getInstance(Context context)
    {
        if ( m_instance == null )
        {
            m_instance = new SimpleAuthCredentialsProvider(context);
        }

        return m_instance;

    }

    private SimpleAuthCredentialsProvider(Context context)
    {
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

        m_userInfo = new TSOUserInfo(identifier,user,pass);

    }

    @Override
    public synchronized boolean saveData()
    {

        SharedPreferences.Editor editor = m_context.getSharedPreferences(PREF_FILE, Context.MODE_PRIVATE).edit();
        editor.putString(IDENTIFIER_FIELD, m_userInfo.getIdentifier());
        editor.putString(USER_FIELD, m_userInfo.getUserName());
        editor.putString(PASS_FIELD, m_userInfo.getPassword());
        editor.commit();

        return true;
    }

    @Override
    public ResultObject<Boolean> login() {

        if ( ( m_userInfo != null ) && ( m_userInfo.getIdentifier() != null ) && ( m_userInfo.getUserName() != null ) )
        {
            m_bLoggedIn = true;
        }

        return new ResultObject<Boolean>(m_bLoggedIn, null, null);
    }


    @Override
    public boolean isUserLoggedIn() {

        return m_bLoggedIn;
    }

    @Override
    public TSOUserInfo getUserInfo() {
        return m_userInfo;
    }

    @Override
    public Object getCredentials() {
        return null;
    }
}
