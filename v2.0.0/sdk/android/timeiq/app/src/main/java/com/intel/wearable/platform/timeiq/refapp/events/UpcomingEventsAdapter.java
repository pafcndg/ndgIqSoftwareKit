package com.intel.wearable.platform.timeiq.refapp.events;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.api.events.IEvent;
import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.apitoolbox.TimeIQEventsUtils;
import com.intel.wearable.platform.timeiq.refapp.utils.TextUtil;

import java.util.ArrayList;

/**
 * Created by adura on 10/22/2015.
 */


/**
 * Class using as the Events Adapter
 */
public class UpcomingEventsAdapter extends ArrayAdapter<IEvent> implements TimeIQEventsUtils.ITtlListener {

    private final ArrayList<IEvent> mEventArray;

    /**
     *
     * @param context Context in which the adapter is running
     * @param resource Resource ID for a layout file containing a TextView to use when
     *                 instantiating views.
     * @param eventsArray {@link IEvent} array to represent in the ListView.
     */
    public UpcomingEventsAdapter(Context context, int resource, ArrayList<IEvent> eventsArray) {
        super(context, resource, eventsArray);
        mEventArray = eventsArray;
    }


    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Context ctx = getContext();
        if (convertView == null) {
            LayoutInflater inflater = (LayoutInflater) ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.item_be_event, null);
        }

        if (convertView != null) {
            convertView.setEnabled(false);
            convertView.setOnClickListener(null);
            IEvent event = mEventArray.get(position);

            String beMainText = event.getSubject();
            String beLocationText = event.getLocation().getAddress();
            long from = event.getArrivalTime();
            long to = from + event.getDuration();

            ((TextView)convertView.findViewById(R.id.main_text)).setText(beMainText);
            ((TextView)convertView.findViewById(R.id.location_text)).setText(beLocationText);
            ((TextView)convertView.findViewById(R.id.from)).setText(TextUtil.getHourAsString(from));
            ((TextView)convertView.findViewById(R.id.to)).setText(TextUtil.getHourAsString(to));

            // Set temp text
            TextView ttlTextView = ((TextView)convertView.findViewById(R.id.ttl_text));
            ttlTextView.setText(getContext().getString(R.string.ttl_calculating));

            // Call getTtlOrEtaString to update
            TimeIQEventsUtils.getTtlOrEtaString(event, ttlTextView, getContext(), this);

        }

        return convertView;
    }

    @Override
    public void onTtlOrEtaStringReceived(Object extraData, String ttlOrEtaString) {
        if (extraData != null && ttlOrEtaString != null) {
            ((TextView)extraData).setText(ttlOrEtaString);
        }
    }

}
