package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;

import com.google.gson.Gson;
import com.intel.wearable.platform.timeiq.api.common.contact.ContactInfo;
import com.intel.wearable.platform.timeiq.api.common.protocol.datatypes.places.PlaceID;
import com.intel.wearable.platform.timeiq.api.common.protocol.enums.MotType;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.api.reminders.IReminder;
import com.intel.wearable.platform.timeiq.api.triggers.ITrigger;
import com.intel.wearable.platform.timeiq.api.triggers.TriggerBuildException;
import com.intel.wearable.platform.timeiq.api.triggers.charge.ChargeTrigger;
import com.intel.wearable.platform.timeiq.api.triggers.charge.ChargeTriggerType;
import com.intel.wearable.platform.timeiq.api.triggers.mot.MotTransition;
import com.intel.wearable.platform.timeiq.api.triggers.mot.MotTrigger;
import com.intel.wearable.platform.timeiq.api.triggers.place.PlaceTrigger;
import com.intel.wearable.platform.timeiq.api.triggers.place.PlaceTriggerType;
import com.intel.wearable.platform.timeiq.api.triggers.time.TimeTrigger;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQPlacesUtils;
import com.intel.wearable.platform.timeiq.refapp.places.add_edit.ActivityPlacePicker;
import com.intel.wearable.platform.timeiq.refapp.utils.TextUtil;
import com.intel.wearable.platform.timeiq.refapp.utils.TimeAndDatePicker;

import java.util.ArrayList;

/**
 * Created by adura on 11/10/2015.
 */

/**
 * Base class for all reminder which are not of type BE reminder
 */
public abstract class BaseFragmentEditReminder extends Fragment implements IReminderFragment {

    private static final String TAG = BaseFragmentEditReminder.class.getSimpleName();
    private TriggerType mSelectedTriggerType = null;
    /*package*/ static IFragmentFormDataChangedListener mDataChangedListener;

    private ArrayAdapter<String> mAdapterListRemindersType;

    private final int RESULT_PICK_CONTACT         = 1;
    /*package*/ final int RESULT_PICK_ARRIVE_PLACE    = 2;
    /*package*/ final int RESULT_PICK_LEAVE_PLACE     = 3;

    /*package*/ EditText mFragmentTitleTextEdit;
    /*package*/ EditText mSmsTextEdit;
    private ListView mReminderTypeListView;
    private boolean mDeleteEnabled = false;
    private View.OnClickListener mOnDeleteClickedListener;

    private Long mReminderTime = null;
    /*package*/ ContactInfo mTargetContactInfo = null;
    /*package*/ String mSmsMessage = null;
    /*package*/ TSOPlace mTargetArrivePlace = null;
    /*package*/ TSOPlace mTargetLeavePlace = null;
    private ArrayList<TriggerType> mTriggerTypesShown;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        mTriggerTypesShown = getTriggerTypesShown();
        View rootView = inflater.inflate(R.layout.fragment_edit_reminder, container, false);
        mFragmentTitleTextEdit = (EditText) rootView.findViewById(R.id.fragment_input_text);

        String hintText = getHint();
        mFragmentTitleTextEdit.setHint(hintText);

        View smsLayout = rootView.findViewById(R.id.notification_reminder_layout);
        if (isShowSms()) {
            smsLayout.setVisibility(View.VISIBLE);
            mSmsTextEdit = (EditText) rootView.findViewById(R.id.notification_reminder_sms_text);
        } else {
            smsLayout.setVisibility(View.GONE);
        }

        if (mDeleteEnabled && mOnDeleteClickedListener != null) {
            Button deleteButton = (Button) rootView.findViewById(R.id.delete_button);
            deleteButton.setVisibility(View.VISIBLE);
            deleteButton.setOnClickListener(mOnDeleteClickedListener);
        }

        setListView(rootView);

        return rootView;
    }

    protected abstract String getHint();
    protected abstract boolean isShowSms();
    protected abstract ResultObject<IReminder> createReminder();
    protected abstract ArrayList<TriggerType> getTriggerTypesShown();

    /*package*/ void onContactPicked() {}
    private void onPlacePicked() {}


    private ArrayList<String> getReminderTypeArray()
    {
        ArrayList<String> myArrayList = new ArrayList<>();

        if (mTriggerTypesShown.contains(TriggerType.Arrive)) {
            if (mTargetArrivePlace == null) {
                myArrayList.add(getString(R.string.reminder_type_arrive));
            } else {
                myArrayList.add(getString(R.string.reminder_type_arrive_to, mTargetArrivePlace.getName()));
            }
        }

        if (mTriggerTypesShown.contains(TriggerType.Leave)) {
            if (mTargetLeavePlace == null) {
                myArrayList.add(getString(R.string.reminder_type_leave));
            } else {
                myArrayList.add(getString(R.string.reminder_type_leave_from, mTargetLeavePlace.getName()));
            }
        }

        if (mTriggerTypesShown.contains(TriggerType.NextDrive)) {
            myArrayList.add(getString(R.string.reminder_type_next_drive));
        }

        if (mTriggerTypesShown.contains(TriggerType.Battery)) {
            myArrayList.add(String.format(getString(R.string.reminder_type_battery), "15%"));
        }

        if (mTriggerTypesShown.contains(TriggerType.Time)) {
            if (mReminderTime != null)
                myArrayList.add(TextUtil.getReminderDateString(getActivity(), mReminderTime));
            else
                myArrayList.add(getString(R.string.reminder_type_time));
        }

        return myArrayList;
    }

    /*package*/ String getValidationData(){
        String validationData = null;
        if (mSelectedTriggerType != null) {
            switch (mSelectedTriggerType) {
                case Arrive:
                    if (mTargetArrivePlace == null) {
                        validationData = getString(R.string.reminder_error_missing_arrive_place);
                    }
                    break;
                case Leave:
                    if (mTargetLeavePlace == null) {
                        validationData = getString(R.string.reminder_error_missing_leave_place);
                    }
                    break;
                case NextDrive:
                    break;
                case Battery:
                    break;
                case Time:
                    //on specific time:
                    if (mReminderTime == null) {
                        validationData = getString(R.string.reminder_error_missing_reminder_time);
                    }
                    break;
                default:
                    validationData = getString(R.string.reminder_error_nothing_selected);
                    break;
            }
        }

        return validationData;
    }

    /*package*/ ResultObject<ITrigger> getTrigger(){
        ResultObject<ITrigger> triggerResultObject = new ResultObject<>(false, getString(R.string.trigger_was_not_selected), null);
        if (mSelectedTriggerType != null) {
            ITrigger trigger;
            switch (mSelectedTriggerType) {
                case Arrive:
                    //when you arrive to a location:
                    if (mTargetArrivePlace != null) {
                        try {
                            trigger = new PlaceTrigger.PlaceTriggerBuilder(PlaceTriggerType.ARRIVE, mTargetArrivePlace.getPlaceId()).build();
                            triggerResultObject = new ResultObject<>(true, null, trigger);
                        } catch (TriggerBuildException e) {
                            triggerResultObject = new ResultObject<>(false, e.getAlertBuildExceptionType().name(), null);
                        }
                    } else {
                        triggerResultObject = new ResultObject<>(false, getString(R.string.reminder_error_missing_arrive_place), null);
                    }
                    break;
                case Leave:
                    //when you leave a location:
                    if (mTargetLeavePlace != null) {
                        try {
                            trigger = new PlaceTrigger.PlaceTriggerBuilder(PlaceTriggerType.LEAVE, mTargetLeavePlace.getPlaceId()).build();
                            triggerResultObject = new ResultObject<>(true, null, trigger);
                        } catch (TriggerBuildException e) {
                            triggerResultObject = new ResultObject<>(false, e.getAlertBuildExceptionType().name(), null);
                        }
                    } else {
                        triggerResultObject = new ResultObject<>(false, getString(R.string.reminder_error_missing_leave_place), null);
                    }
                    break;
                case NextDrive:
                    //on your next drive:
                    try {
                        trigger = new MotTrigger.MotTriggerBuilder(MotType.CAR, MotTransition.START).build();
                        triggerResultObject = new ResultObject<>(true, null, trigger);
                    } catch (TriggerBuildException e) {
                        triggerResultObject = new ResultObject<>(false, e.getAlertBuildExceptionType().name(), null);
                    }
                    break;
                case Battery:
                    //when battery is lower than 15%:
                    try {
                        trigger = new ChargeTrigger.ChargeTriggerBuilder(ChargeTriggerType.BATTERY_OK).setPercent(15).build();
                        triggerResultObject = new ResultObject<>(true, null, trigger);
                    } catch (TriggerBuildException e) {
                        triggerResultObject = new ResultObject<>(false, e.getAlertBuildExceptionType().name(), null);
                    }
                    break;
                case Time:
                    //on specific time:
                    if (mReminderTime != null) {
                        try {
                            trigger = new TimeTrigger.TimeTriggerBuilder(mReminderTime).build();
                            triggerResultObject = new ResultObject<>(true, null, trigger);
                        } catch (TriggerBuildException e) {
                            triggerResultObject = new ResultObject<>(false, e.getAlertBuildExceptionType().name(), null);
                        }
                    } else {
                        triggerResultObject = new ResultObject<>(false, getString(R.string.reminder_error_missing_reminder_time), null);
                    }
                    break;
            }
        }

        return triggerResultObject;
    }

    private void setListView(View rootView) {
        mAdapterListRemindersType = new ArrayAdapter<>(getActivity(),
                android.R.layout.simple_list_item_multiple_choice, getReminderTypeArray());

        mReminderTypeListView = (ListView) rootView.findViewById(R.id.reminderTypeListView);
        mReminderTypeListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
        mReminderTypeListView.setAdapter(mAdapterListRemindersType);
        setSelectionToView();

        mReminderTypeListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                mSelectedTriggerType = mTriggerTypesShown.get(position);
                switch (mSelectedTriggerType) {
                    case Arrive:
                        Log.d(TAG, "REMINDER_TYPE_ARRIVE	 ");
                        Intent arriveIntentActivity = new Intent(getActivity(), ActivityPlacePicker.class);
                        startActivityForResult(arriveIntentActivity, RESULT_PICK_ARRIVE_PLACE);
                        break;
                    case Leave:
                        Log.d(TAG, "REMINDER_TYPE_LEAVE");
                        Intent leaveIntentActivity = new Intent(getActivity(), ActivityPlacePicker.class);
                        startActivityForResult(leaveIntentActivity, RESULT_PICK_LEAVE_PLACE);
                        break;
                    case NextDrive:
                        Log.d(TAG, "REMINDER_TYPE_NEXT_DRIVE ");
                        break;
                    case Battery:
                        Log.d(TAG, "REMINDER_TYPE_BATTERY	 ");
                        break;
                    case Time:
                        Log.d(TAG, "REMINDER_TYPE_TIME		 ");
                        TimeAndDatePicker timeAndDatePicker = new TimeAndDatePicker();
                        timeAndDatePicker.handleTimePicker(getActivity(), new TimeAndDatePicker.TimePickerListener() {
                            @Override
                            public void onTimePicked(long time) {
                                mReminderTime = time;
                                refreshList();
                            }

                            @Override
                            public void onCancel() {
                                mSelectedTriggerType = null;
                                refreshList();
                            }
                        });
                        break;
                }

                refreshList();
            }
        });
    }

    private void setSelectionToView() {
        if (mSelectedTriggerType != null) {
            mReminderTypeListView.setItemChecked(mSelectedTriggerType.ordinal(), true);
        } else {
            int checkedItemPosition = mReminderTypeListView.getCheckedItemPosition();
            if (checkedItemPosition != ListView.INVALID_POSITION) {
                mReminderTypeListView.setItemChecked(checkedItemPosition, false);
            }
        }
    }

    /*package*/ void pickContact()
    {
        Intent contactPickerIntent = new Intent(Intent.ACTION_PICK,
                ContactsContract.CommonDataKinds.Phone.CONTENT_URI);
        startActivityForResult(contactPickerIntent, RESULT_PICK_CONTACT);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (resultCode == Activity.RESULT_OK) {
            switch (requestCode) {
                case RESULT_PICK_CONTACT:
                    handleContactPicked(data);
                    break;
                case RESULT_PICK_ARRIVE_PLACE:
                    handlePlacePicked(data, RESULT_PICK_ARRIVE_PLACE);
                    break;
                case RESULT_PICK_LEAVE_PLACE:
                    handlePlacePicked(data, RESULT_PICK_LEAVE_PLACE);
                    break;
            }
        } else {
            switch (requestCode) {
                case RESULT_PICK_CONTACT:
                    break;
                case RESULT_PICK_ARRIVE_PLACE:
                case RESULT_PICK_LEAVE_PLACE:
                    mSelectedTriggerType = null;
                    break;
            }
        }

        refreshList();
    }

    private void refreshList()
    {
        setSelectionToView();
        mAdapterListRemindersType.clear();
        mAdapterListRemindersType.addAll(getReminderTypeArray());
        mAdapterListRemindersType.notifyDataSetChanged();

        if (mDataChangedListener != null) {
            mDataChangedListener.setCreateReminderIcon(isOkToCreateReminder());
        }
    }

    /*package*/ boolean handlePlacePicked(Intent data, int placeType) {
        boolean success = false;
        if (data.getExtras().containsKey(FragmentEditBe.SELECT_PLACE_ID_DATA_KEY)) {
            String stringExtra = data.getStringExtra(FragmentEditBe.SELECT_PLACE_ID_DATA_KEY);
            PlaceID placeId = new Gson().fromJson(stringExtra, PlaceID.class);
            TSOPlace place = TimeIQPlacesUtils.getPlace(placeId);
            if (place != null) {
                switch (placeType) {
                    case RESULT_PICK_ARRIVE_PLACE:
                        mTargetArrivePlace = place;
                        success = true;
                        break;
                    case RESULT_PICK_LEAVE_PLACE:
                        mTargetLeavePlace = place;
                        success = true;
                        break;
                }
            }

            onPlacePicked();
        }

        return success;
    }

    private void handleContactPicked(Intent data) {
        try {
            Uri uri = data.getData();

            Cursor cursor = getActivity().getContentResolver().query(uri, null, null, null, null);
            if(cursor != null) {
                cursor.moveToFirst();

                int nameIndex = cursor.getColumnIndex(ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME);
                int phoneIndex = cursor.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER);
                int idIndex = cursor.getColumnIndex(ContactsContract.Contacts._ID);

                String contactId = cursor.getString(idIndex);
                String phoneNo = cursor.getString(phoneIndex);
                String name = cursor.getString(nameIndex);

                mTargetContactInfo = new ContactInfo(contactId, name, null, phoneNo);
                mTargetContactInfo.setPreferredPhoneNumber(phoneNo);

                onContactPicked();
                cursor.close();
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /*package*/ void enableDeleteAndRegister(View.OnClickListener listener) {
        mDeleteEnabled = true;
        mOnDeleteClickedListener = listener;
    }

    public void onDeleteButtonClicked(View view) {
        if (mOnDeleteClickedListener != null) {
            mOnDeleteClickedListener.onClick(view);
        }
    }

// Setters for editing
    public void setContactInfo(ContactInfo contactInfo) {
        this.mTargetContactInfo = contactInfo;
    }

    public void setNotificationMessage(String notificationMessage) {
        this.mSmsMessage = notificationMessage;
    }

    public void setTriggerToArrivePlace(PlaceID arrivePlace) {
        this.mTargetArrivePlace = TimeIQPlacesUtils.getPlace(arrivePlace);
        mSelectedTriggerType = TriggerType.Arrive;
    }

    public void setTriggerToLeavePlace(PlaceID leavePlace) {
        this.mTargetLeavePlace = TimeIQPlacesUtils.getPlace(leavePlace);
        mSelectedTriggerType = TriggerType.Leave;
    }

    public void setTriggerToNextDrive() {
        mSelectedTriggerType = TriggerType.NextDrive;
    }

    public void setTriggerToBattery() {
        mSelectedTriggerType = TriggerType.Battery;
    }

    public void setTriggerToReminderTime(Long reminderTime) {
        this.mReminderTime = reminderTime;
        mSelectedTriggerType = TriggerType.Time;
    }

    @Override
    public void setDataChangeListener(IFragmentFormDataChangedListener dataChangeListener) {
        mDataChangedListener = dataChangeListener;
    }

}
