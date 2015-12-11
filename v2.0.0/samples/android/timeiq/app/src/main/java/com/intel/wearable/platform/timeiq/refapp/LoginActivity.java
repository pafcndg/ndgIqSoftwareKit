package com.intel.wearable.platform.timeiq.refapp;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.Editable;
import android.text.Html;
import android.text.TextUtils;
import android.text.method.LinkMovementMethod;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.api.common.auth.TSOUserInfo;
import com.intel.wearable.platform.timeiq.refapp.auth.AuthUtil;
import com.intel.wearable.platform.timeiq.refapp.auth.ICredentialsProvider;
import com.intel.wearable.platform.timeiq.refapp.main.MainActivity;

public class LoginActivity extends Activity implements View.OnClickListener
{
    private TextView mWrongCredentials;
    private ProgressBar mProgressBar;
    private Button mSignInButton;

    @Override
    protected void onCreate( Bundle savedInstanceState )
    {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_user_login);

        TextView signUpLink = (TextView) findViewById(R.id.sign_up_Link_textView);
        signUpLink.setText(Html.fromHtml(getString(R.string.sign_up_link)));
        signUpLink.setMovementMethod(LinkMovementMethod.getInstance());

        mWrongCredentials = (TextView) findViewById(R.id.wrong_credentials);
        mWrongCredentials.setVisibility(View.GONE);

        mProgressBar = (ProgressBar) findViewById(R.id.progressBar);
        mProgressBar.setVisibility(View.GONE);

        mSignInButton = (Button)findViewById(R.id.sign_in_button);
        mSignInButton.setOnClickListener(this);
    }


    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_user_id, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if ( id == R.id.action_settings )
        {
            return true;
        }

        return super.onOptionsItemSelected( item );
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();
        if (id == R.id.sign_in_button) {
            signIn();
        }
    }

    private void signIn() {
        EditText viewById = (EditText) this.findViewById(R.id.password_edit);
        Editable passwordText = viewById.getText();

        EditText userNameEdit = (EditText) this.findViewById(R.id.username_edit);
        Editable userNameText = userNameEdit.getText();

        if ( (passwordText == null || passwordText.toString().trim().isEmpty() ) ||
                ( userNameText == null || userNameText.toString().trim().isEmpty()) )
        {
            mWrongCredentials.setVisibility(View.VISIBLE);
            mWrongCredentials.setText(getString(R.string.phone_name_cant_be_empty));
        }
        else{
            onLoginPressed(userNameText.toString(), passwordText.toString());
        }
    }

    private void onLoginPressed(final String userNameText, final String passwordText) {
        mProgressBar.setVisibility(View.VISIBLE);
        mSignInButton.setVisibility(View.GONE);

        final ICredentialsProvider authProvider = AuthUtil.getAuthProvider(getApplicationContext());

        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                authProvider.setUserInfo(new TSOUserInfo(userNameText, userNameText, passwordText));
                final ResultObject<Boolean> loggedIn = authProvider.login();
                if (loggedIn.isSuccess()) {
                    authProvider.saveData();
                    Intent openMainActivity = new Intent(getApplicationContext(), MainActivity.class);
                    startActivity(openMainActivity);
                    finish();
                }
                else{
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mWrongCredentials.setVisibility(View.VISIBLE);
                            String errorMsg = loggedIn.getMsg();
                            mWrongCredentials.setText(TextUtils.isEmpty(errorMsg) ? getString(R.string.unknown_login_error) : errorMsg);
                            mProgressBar.setVisibility(View.GONE);
                            mSignInButton.setVisibility(View.VISIBLE);

                        }
                    });
                }
                return null;
            }
        }.execute();

    }
}
