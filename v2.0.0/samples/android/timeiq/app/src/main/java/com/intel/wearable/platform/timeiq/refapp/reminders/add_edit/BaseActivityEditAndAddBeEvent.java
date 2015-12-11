package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.content.Context;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.internal.view.menu.ActionMenuItemView;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.refapp.ColoredStatusBarActivity;
import com.intel.wearable.platform.timeiq.refapp.R;

public abstract class BaseActivityEditAndAddBeEvent extends ColoredStatusBarActivity implements IFragmentFormDataChangedListener {

    /*package*/ Context mContext;
    private Toolbar mToolbar;
    private String mValidationData;
    private boolean mIsOkToCreateReminder;

    /*package*/ void createContentView(int activityResource, int titleResource) {
        setContentView(activityResource);

        mContext = getApplicationContext();

        mToolbar = (Toolbar) findViewById(R.id.toolbar);
        mToolbar.setTitle(titleResource);
        mToolbar.inflateMenu(R.menu.refapp_activity_title_action_bar);
        mToolbar.setNavigationIcon(R.drawable.ra_ic_action_cancel);
        mToolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });

        mToolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int id = item.getItemId();
                if (id == R.id.edit_ok){
                    if ( mIsOkToCreateReminder) {
                        actionOnOk();
                        return true;
                    } else {
                        Toast.makeText(mContext, mValidationData, Toast.LENGTH_LONG).show();
                    }
                }

                return false;
            }
        });
    }

    protected abstract void actionOnOk();

    @Override
    public void setCreateReminderIcon(String validationData) {

        mValidationData = validationData;

        ActionMenuItemView viewCreateReminder = (ActionMenuItemView) mToolbar.findViewById(R.id.edit_ok);

        if (TextUtils.isEmpty(validationData)) {
            mIsOkToCreateReminder = true;
            viewCreateReminder.setIcon(getDrawable(R.drawable.ra_ic_action_ok));
        } else {
            mIsOkToCreateReminder = false;
            viewCreateReminder.setIcon(getDrawable(R.drawable.ra_ic_action_disable));
        }
    }

}