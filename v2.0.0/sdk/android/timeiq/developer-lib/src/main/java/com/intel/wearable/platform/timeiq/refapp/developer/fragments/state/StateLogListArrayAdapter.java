package com.intel.wearable.platform.timeiq.refapp.developer.fragments.state;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.refapp.developer.R;

import java.util.ArrayList;

/**
 * Created by mleib on 21/07/2015.
 */
public class StateLogListArrayAdapter extends ArrayAdapter<StateChangeData> {

    private ArrayList<StateChangeData> m_data;
    private Context m_context;

    public StateLogListArrayAdapter(Context context, int resource, ArrayList<StateChangeData> data) {
        super(context, resource, data);
        m_data = data;
        m_context = getContext();
    }

    @Override
    public View getView(final int position, View row, ViewGroup parent) {
        if (row == null) {
            LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            row = inflater.inflate(R.layout.layout_state_change, null);
        }

        if (row != null) {
            row.setEnabled(false);
            row.setOnClickListener(null);
            StateChangeData stateChangeData = m_data.get(position);
            if (stateChangeData != null) {
                TextView timeTextView = (TextView) row.findViewById(R.id.state_change_date_and_time);
                TextView typeTextView = (TextView) row.findViewById(R.id.state_change_type);
                TextView oldStateTextView = (TextView) row.findViewById(R.id.state_change_old_state);
                TextView arrowTextView = (TextView) row.findViewById(R.id.state_change_arrow);
                TextView newStateTextView = (TextView) row.findViewById(R.id.state_change_new_state);

                timeTextView.setText(stateChangeData.time.replace(' ', '\n'));
                typeTextView.setText(stateChangeData.type.name());

                if (stateChangeData.oldState.trim().isEmpty() && stateChangeData.newState.trim().isEmpty()) {
                    oldStateTextView.setText(m_context.getString(R.string.developer_fragment_state_unknown_state));
                    arrowTextView.setText("");
                    newStateTextView.setText("");
                } else {
                    if (stateChangeData.oldState.trim().isEmpty()) {
                        oldStateTextView.setText(m_context.getString(R.string.developer_fragment_state_no_state));
                    } else {
                        oldStateTextView.setText(stateChangeData.oldState.replace(" (", "\n("));
                    }

                    if (stateChangeData.newState != null) {
                        newStateTextView.setVisibility(View.VISIBLE);
                        arrowTextView.setVisibility(View.VISIBLE);
                        arrowTextView.setText(m_context.getString(R.string.developer_fragment_state_arrow));
                        if (stateChangeData.newState.trim().isEmpty()) {
                            newStateTextView.setText(m_context.getString(R.string.developer_fragment_state_no_state));
                        } else {
                            newStateTextView.setText(stateChangeData.newState.replace(" (", "\n("));
                        }
                    } else {
                        arrowTextView.setVisibility(View.GONE);
                        newStateTextView.setVisibility(View.GONE);
                    }
                }
            }
        }

        return row;
    }

}
