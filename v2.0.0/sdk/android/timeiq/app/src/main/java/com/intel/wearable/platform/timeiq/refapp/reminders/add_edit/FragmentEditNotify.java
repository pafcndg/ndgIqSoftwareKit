package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.reminders.ReminderBuildException;
import com.intel.wearable.platform.timeiq.api.reminders.phoneBasedReminder.notificationReminder.NotificationReminder;
import com.intel.wearable.platform.timeiq.api.triggers.ITrigger;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.auth.AuthUtil;

import java.util.ArrayList;

/**
 * A placeholder fragment containing a simple view.
 */
public class FragmentEditNotify extends BaseFragmentEditReminder {

    private final TextWatcher mTextWatcher = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) { }

        @Override
        public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) { }

        @Override
        public void afterTextChanged(Editable editable) {
            mSmsMessage = editable.toString();
        }
    };

    /**
     * Returns a new instance of this fragment for the given section
     * number.
     */
    public static FragmentEditNotify newInstance(IFragmentFormDataChangedListener dataChangedListener) {
        FragmentEditNotify fragment = new FragmentEditNotify();
        fragment.setDataChangeListener(dataChangedListener);
        return fragment;
    }

    public FragmentEditNotify() {
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

        mSmsTextEdit.addTextChangedListener(mTextWatcher);

        if (mTargetContactInfo != null) {
            setContactInfoToView();
        }

        if (mSmsMessage != null) {
            setSmsMessageToView();
        }

        return view;
    }

    @Override
    protected String getHint() {
        return getString(R.string.hint_notify);
    }

    @Override
    protected boolean isShowSms() {
        return true;
    }

    @Override
    protected ResultObject<IReminder> createReminder() {

        ResultObject<IReminder> reminderResult;

        ResultObject<ITrigger> triggerResult = getTrigger();
        if(triggerResult.isSuccess()) {
            try {
                IReminder notificationReminder = new NotificationReminder.NotificationReminderBuilder(triggerResult.getData(), mTargetContactInfo, mSmsMessage).build();
                reminderResult = new ResultObject<>(true, null, notificationReminder);
            } catch (ReminderBuildException e) {
                reminderResult = new ResultObject<>(false, e.getReminderBuildExceptionType().name(), null);
            }
        } else {
            reminderResult = new ResultObject<>(false, triggerResult.getMsg(), null);
        }
        return reminderResult;

    }

    @Override
    public String isOkToCreateReminder() {
        String validationData = getValidationData();
        if (TextUtils.isEmpty(validationData)) {
            if ( mTargetContactInfo == null ) {
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
        triggerTypes.add(TriggerType.Arrive);
        triggerTypes.add(TriggerType.Leave);
        return triggerTypes;
    }

    @Override
    protected boolean handlePlacePicked(Intent data, int placeType) {
        boolean success = super.handlePlacePicked(data, placeType);
        if (success) {
            String placeName, userName;
            switch (placeType) {
                case RESULT_PICK_ARRIVE_PLACE:
                    placeName = mTargetArrivePlace.getName();
                    userName = AuthUtil.getAuthProvider(getActivity()).getUserInfo().getUserName();
                    mSmsMessage = getString(R.string.notification_reminder_sms_default_text_for_arrive, placeName, userName);
                    setSmsMessageToView();
                    break;
                case RESULT_PICK_LEAVE_PLACE:
                    placeName = mTargetLeavePlace.getName();
                    userName = AuthUtil.getAuthProvider(getActivity()).getUserInfo().getUserName();
                    mSmsMessage = getString(R.string.notification_reminder_sms_default_text_for_leave, placeName, userName);
                    setSmsMessageToView();
                    break;
            }
        }

        return success;
    }

    private void setContactInfoToView() {
        String name = mTargetContactInfo.getName();
        String phoneNo = mTargetContactInfo.getPreferredPhoneNumber().getCleanPhoneNumber();
        String titleText = getString(R.string.reminder_action_notify, name, phoneNo);
        mFragmentTitleTextEdit.setText(titleText);
    }

    private void setSmsMessageToView() {
        mSmsTextEdit.removeTextChangedListener(mTextWatcher);
        mSmsTextEdit.setText(mSmsMessage);
        mSmsTextEdit.addTextChangedListener(mTextWatcher);
    }

}
