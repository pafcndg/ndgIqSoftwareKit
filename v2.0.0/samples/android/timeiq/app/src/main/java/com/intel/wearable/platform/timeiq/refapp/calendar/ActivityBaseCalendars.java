package com.intel.wearable.platform.timeiq.refapp.calendar;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.CheckedTextView;
import android.widget.ListView;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.api.common.calendar.CalendarDetails;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.ResultObject;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQCalendarUtils;

import java.util.ArrayList;
import java.util.List;

public abstract class ActivityBaseCalendars extends FragmentActivity {

    private final boolean IS_MULTIPLE_CHOICE;
    private final int TITLE_RESOURCE;
    private ArrayList<CalendarDetails> mCalendars;
    private CalendarsArrayAdapter mCalendarsArrayAdapter;

    protected ActivityBaseCalendars(boolean isMultipleChoice, int titleResource) {
        IS_MULTIPLE_CHOICE = isMultipleChoice;
        TITLE_RESOURCE = titleResource;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ResultObject<List<CalendarDetails>> availableCalendarsResultData = TimeIQCalendarUtils.getAvailableCalendars();
        if (availableCalendarsResultData.isSuccess()) {
            mCalendars = new ArrayList<>(availableCalendarsResultData.getData());
            if (mCalendars.size() > 0) {
                AlertDialog.Builder builder = getCalendarsAlertDialogBuilder();
                int listItemResource = IS_MULTIPLE_CHOICE ? android.R.layout.simple_list_item_multiple_choice : android.R.layout.simple_list_item_single_choice;
                mCalendarsArrayAdapter = new CalendarsArrayAdapter(this, R.id.calendarsListView, listItemResource, mCalendars, getSelected());
                builder.setOnDismissListener(new DialogInterface.OnDismissListener() {
                    @Override
                    public void onDismiss(DialogInterface dialogInterface) {
                        finish();
                    }
                });

                LayoutInflater factory = LayoutInflater.from(this);
                View content = factory.inflate(R.layout.dialog_calendar, null);
                ListView alertDialogListView = (ListView) content.findViewById(R.id.calendarsListView);
                alertDialogListView.setAdapter(mCalendarsArrayAdapter);
                alertDialogListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                    @Override
                    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                        CheckedTextView checkedTextView = (CheckedTextView) view;
                        if (IS_MULTIPLE_CHOICE) {
                            mCalendarsArrayAdapter.toggleItemAt(position);
                            mCalendarsArrayAdapter.notifyDataSetChanged();
                        } else {
                            if (!checkedTextView.isChecked()) {
                                mCalendarsArrayAdapter.selectSingle(position);
                                mCalendarsArrayAdapter.notifyDataSetChanged();
                            } // else do nothing (already checked)
                        }
                    }
                });

                builder.setView(content);
                AlertDialog alertDialog = builder.create();
                alertDialog.show();
            } else {
                Toast.makeText(getApplicationContext(), R.string.error_no_calendars_to_choose_from, Toast.LENGTH_LONG).show();
            }
        } else {
            Toast.makeText(getApplicationContext(), availableCalendarsResultData.getMsg(), Toast.LENGTH_LONG).show();
        }
    }

    private AlertDialog.Builder getCalendarsAlertDialogBuilder() {
        return  new  AlertDialog.Builder(this)
                .setTitle(getResources().getString(TITLE_RESOURCE))
                .setPositiveButton(getResources().getString(android.R.string.yes), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                List<String> selectedCalendars = new ArrayList<>();
                                for (int i = 0; i < mCalendars.size(); i++) {
                                    if (((CheckedTextView) mCalendarsArrayAdapter.getView(i, null, null)).isChecked()) {
                                        selectedCalendars.add(mCalendars.get(i).getCalendarId());
                                    }
                                }

                                if (setCalendars(selectedCalendars)) {
                                    dialog.dismiss();
                                }
                            }
                        }
                )
                .setNegativeButton(getResources().getString(android.R.string.no), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                dialog.dismiss();
                            }
                        }
                );
    }

    private boolean[] getSelected() {
        boolean[] selectedCalendarsResult = new boolean[mCalendars.size()];

        ResultObject<List<CalendarDetails>> selectedCalendarsResultData = getSelectedCalendars();
        if (selectedCalendarsResultData != null && selectedCalendarsResultData.isSuccess() && selectedCalendarsResultData.getData() != null && !selectedCalendarsResultData.getData().isEmpty()) {
            List<CalendarDetails> selectedCalendars = selectedCalendarsResultData.getData();
            for (int i=0 ; i<selectedCalendars.size() ; ++i) {
                if (mCalendars.contains(selectedCalendars.get(i))) {
                    selectedCalendarsResult[mCalendars.indexOf(selectedCalendars.get(i))] = true;
                }
            }
        }

        return selectedCalendarsResult;
    }

    protected abstract boolean setCalendars(List<String> selectedCalendars);
    protected abstract ResultObject<List<CalendarDetails>> getSelectedCalendars();

}