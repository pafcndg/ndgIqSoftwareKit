package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.customui.CustomToastMessage;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUser;


/**
 * Created by fissaX on 11/22/15.
 */
public class LoginActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.login);

        ColorDrawable colorDrawable = new ColorDrawable(ContextCompat.getColor(this, R.color.blue));
        getSupportActionBar().setBackgroundDrawable(colorDrawable);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setTitle(R.string.login);



        if(BodyIQAPIManager.getInstance().isUserExist())
        {
            BodyIQUser user = BodyIQAPIManager.getInstance().getUser();
            ((EditText)findViewById(R.id.email)).setText(user.getEmail());
            ((EditText)findViewById(R.id.password)).setText(user.getPassword());

        }

        findViewById(R.id.submit).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String enteredMail = ((EditText)findViewById(R.id.email)).getText().toString();
                String enteredPass = ((EditText)findViewById(R.id.password)).getText().toString();

                if(BodyIQAPIManager.getInstance().isUserExist()) {
                    BodyIQUser user = BodyIQAPIManager.getInstance().getUser();
                    if (user.getEmail().equals(enteredMail) && user.getPassword().equals(enteredPass)) {
                        Intent intent = new Intent(LoginActivity.this, DeviceSyncActivity.class);
                        startActivity(intent);
                    } else {
                        new CustomToastMessage(LoginActivity.this).showMessage(getResources().getString(R.string.wrongmailorpassword));
                    }
                }
                else
                {
                    new CustomToastMessage(LoginActivity.this).showMessage(getResources().getString(R.string.needsignin));
                    finish();
                }
            }
        });

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}