package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderBuildException;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.callReminder.CallReminder;
import com.intel.wearable.platform.timeiq.api.triggers.ITrigger;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * A placeholder fragment containing a simple view.
 */
public class FragmentEditCall extends BaseFragmentEditReminder {

    private static final String TAG = "FragmentEditCall";

    /**
     * Returns a new instance of this fragment for the given section
     * number.
     */
    public static FragmentEditCall newInstance(IFragmentFormDataChangedListener dataChangedListener) {
        FragmentEditCall fragment = new FragmentEditCall();
        fragment.setDataChangeListener(dataChangedListener);
        return fragment;
    }

    public FragmentEditCall() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = super.onCreateView(inflater, container, savedInstanceState);
        mFragmentTitleTextEdit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                pickContact();
            }
        });

        if (mTargetContactInfo != null) {
            setContactInfoToView();
        }

        return view;
    }

    @Override
    protected String getHint() {
        return getString(R.string.hint_call);
    }

    @Override
    protected boolean isShowSms() {
        return false;
    }

    @Override
    protected ResultObject<IReminder> createReminder() {
        ResultObject<IReminder> reminderResult;

        ResultObject<ITrigger> triggerResult = getTrigger();
        if(triggerResult.isSuccess()) {
            try {
                IReminder callReminder = new CallReminder.CallReminderBuilder(triggerResult.getData(), mTargetContactInfo).build();
                reminderResult = new ResultObject<>(true, null, callReminder);
            } catch (ReminderBuildException e) {
                reminderResult = new ResultObject<>(false, e.getReminderBuildExceptionType().name(), null);
            }
        }
        else{
            reminderResult = new ResultObject<>(false, triggerResult.getMsg(), null);
        }
        return reminderResult;
    }

    @Override
    public String isOkToCreateReminder() {
        String validationData = getValidationData();

        if (TextUtils.isEmpty(validationData)) {
            if (mTargetContactInfo == null) {
                validationData = getString(R.string.reminder_error_missing_target_contact);
            }
        }

        return validationData;
    }


    @Override
    protected void onContactPicked( ) {
        setContactInfoToView();
    }

    @Override
    protected ArrayList<TriggerType> getTriggerTypesShown() {
        ArrayList<TriggerType> triggerTypes = new ArrayList<>();
        triggerTypes.addAll(Arrays.asList(TriggerType.values()));
        return triggerTypes;
    }

    private void setContactInfoToView() {
        if (mTargetContactInfo == null ) {
            Log.e(TAG, "missing user info ");
            return;
        }

        String name     = mTargetContactInfo.getName();
        String phoneNo  = mTargetContactInfo.getPreferredPhoneNumber().getCleanPhoneNumber();
        String titleText = getString(R.string.reminder_action_call, name, phoneNo);
        mFragmentTitleTextEdit.setText(titleText);

        if (mDataChangedListener != null) {
            mDataChangedListener.setCreateReminderIcon(isOkToCreateReminder());
        }
    }

}
