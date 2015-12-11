package com.intel.wearable.platform.timeiq.refapp;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

import com.intel.wearable.platform.timeiq.refapp.auth.AuthUtil;
import com.intel.wearable.platform.timeiq.refapp.auth.ICredentialsProvider;
import com.intel.wearable.platform.timeiq.refapp.main.MainActivity;


public class SplashActivity extends Activity {

    private static final String TAG = SplashActivity.class.getSimpleName();

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d(TAG, "onCreate");

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.splash_layout);

        Context context = getApplicationContext();
        final ICredentialsProvider authProvider = AuthUtil.getAuthProvider(context);
        authProvider.loadData();

        new AsyncTask<Void, Void, Void>(){

            public boolean mLoginSuccess;

            @Override
            protected Void doInBackground(Void... params) {
                Log.d(TAG, "doInBackground");
                mLoginSuccess = authProvider.login().isSuccess();
                return null;
            }

            @Override
            protected void onPostExecute(Void aVoid) {
                super.onPostExecute(aVoid);
                Log.d(TAG, "onPostExecute mLoginSuccess=" + mLoginSuccess);
                if(mLoginSuccess){
                    Handler handler = new Handler();
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            startMainActivity();
                            finish();
                        }
                    }, 1000);
                }
                else{
                    startUserIdActivity();
                    finish();
                }
            }


        }.execute();
    }

    private void startUserIdActivity(){
        Intent idActivity = new Intent(getApplicationContext(), LoginActivity.class);
        startActivity(idActivity);
    }


    private void startMainActivity() {
        Intent openMainActivity = new Intent(getApplicationContext(), MainActivity.class);
        startActivity(openMainActivity);
    }
}