package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.content.Context;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.api.events.IEvent;
import com.intel.wearable.platform.timeiq.api.places.datatypes.TSOPlace;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.utils.TextUtil;

import java.util.ArrayList;

/*package*/ class ConflictingEventsAdapter extends ArrayAdapter<IEvent> {

    private final ArrayList<IEvent> mConflictingEventArrayList;
    private final Context m_context;

    /**
     *
     * @param context Context in which the adapter is running
     * @param conflictingEventArrayList {@link IEvent} array to represent in the ListView.
     */
    public ConflictingEventsAdapter(Context context, ArrayList<IEvent> conflictingEventArrayList) {
        super(context, R.layout.item_be_conflict, conflictingEventArrayList);
        mConflictingEventArrayList = conflictingEventArrayList;
        m_context = getContext();
    }


    @Override
    public View getView(final int position, View row, ViewGroup parent) {
        if (row == null) {
            LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            row = inflater.inflate(R.layout.item_be_conflict, null);
        }

        if (row != null) {
            row.setEnabled(false);
            row.setOnClickListener(null);
            IEvent event = mConflictingEventArrayList.get(position);
            if (event != null) {
                TextView mainTextView = (TextView) row.findViewById(R.id.be_conflict_main_text);
                String mainText = event.getSubject();
                if (TextUtils.isEmpty(mainText) && !m_context.getString(R.string.be_description).equals(event.getDescription())) {
                    mainText = event.getDescription();
                }

                if (!TextUtils.isEmpty(mainText)) {
                    mainTextView.setText(mainText);
                    mainTextView.setVisibility(View.VISIBLE);
                } else {
                    mainTextView.setVisibility(View.GONE);
                }

                TSOPlace location = event.getLocation();
                TextView subTextView = (TextView) row.findViewById(R.id.be_conflict_sub_text);
                if (location != null) {
                    subTextView.setText(location.getAddress());
                    subTextView.setVisibility(View.VISIBLE);
                } else {
                    subTextView.setVisibility(View.GONE);
                }

                String fromToText = TextUtil.getHourAsString(event.getArrivalTime()) + "-" + TextUtil.getHourAsString(event.getArrivalTime() + event.getDuration());
                TextView fromToTextView = (TextView) row.findViewById(R.id.be_conflict_from_to_text);
                fromToTextView.setText(fromToText);
            }
        }

        return row;
    }

}
