package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderBuildException;
import com.intel.wearable.platform.timeiq.api.reminders.doReminder.DoReminder;
import com.intel.wearable.platform.timeiq.api.triggers.ITrigger;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * A placeholder fragment containing a simple view.
 */
public class FragmentEditDo extends BaseFragmentEditReminder {


    private static final String TAG = "FragmentEditDo";
    private String mAction;

    /**
     * Returns a new instance of this fragment for the given section
     * number.
     */
    public static FragmentEditDo newInstance(IFragmentFormDataChangedListener dataChangedListener) {
        FragmentEditDo fragment = new FragmentEditDo();
        fragment.setDataChangeListener(dataChangedListener);
        return fragment;
    }

    public FragmentEditDo() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = super.onCreateView(inflater, container, savedInstanceState);
        if (mAction != null) {
            mFragmentTitleTextEdit.setText(mAction);
        }

        mFragmentTitleTextEdit.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                if (mDataChangedListener != null) {
                    mDataChangedListener.setCreateReminderIcon(isOkToCreateReminder());
                }
            }
        });

        return view;
    }


    @Override
    protected String getHint() {
        return getString(R.string.hint_do);
    }

    @Override
    protected boolean isShowSms() {
        return false;
    }


    @Override
    protected ResultObject<IReminder> createReminder() {
        ResultObject<IReminder> reminderResult;
        String doAction = mFragmentTitleTextEdit.getText().toString();
        if( ! TextUtils.isEmpty(doAction)) {
            ResultObject<ITrigger> triggerResult = getTrigger();
            if (triggerResult.isSuccess()) {
                try {
                    IReminder reminder = new DoReminder.DoReminderBuilder(triggerResult.getData(), doAction).build();
                    reminderResult = new ResultObject<>(true, null, reminder);
                } catch (ReminderBuildException e) {
                    Log.e(TAG, "Exception at createReminder: ", e);
                    reminderResult = new ResultObject<>(false, e.getReminderBuildExceptionType().toString(), null);
                }
            }
            else {
                reminderResult = new ResultObject<>(false, triggerResult.getMsg(), null);
            }
        }
        else{
            reminderResult = new ResultObject<>(false, getString(R.string.reminder_error_missing_do_action), null);
        }
        return reminderResult;
    }

    @Override
    protected ArrayList<TriggerType> getTriggerTypesShown() {
        ArrayList<TriggerType> triggerTypes = new ArrayList<>();
        triggerTypes.addAll(Arrays.asList(TriggerType.values()));
        return triggerTypes;
    }

    @Override
    public String isOkToCreateReminder() {
        String validationData = getValidationData();
        if ( TextUtils.isEmpty(validationData)){
            CharSequence doAction = mFragmentTitleTextEdit.getText();
            if ( TextUtils.isEmpty(doAction) ) {
                validationData = getString(R.string.reminder_error_missing_do_action);
            }
        }

        return validationData;
    }

    public void setAction(String action) {
        mAction = action;
    }

}
