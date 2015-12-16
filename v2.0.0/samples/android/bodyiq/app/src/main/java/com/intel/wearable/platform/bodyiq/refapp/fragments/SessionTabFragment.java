package com.intel.wearable.platform.bodyiq.refapp.fragments;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.bodymanager.BodyIQInfo;
import com.intel.wearable.platform.bodyiq.refapp.listeners.BodyIQActivitiesListener;
import com.intel.wearable.platform.core.device.WearableToken;

import java.util.ArrayList;

/**
 * Created by fissaX on 12/2/15.
 */
public class SessionTabFragment extends Fragment {

    private ViewGroup   containerView;
    TextView            emptyListText = null;
    Button              sessionButton = null;
    ArrayList<WearableToken> foundDevices = new ArrayList<>();


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.sessionstabfragment, container, false);
        containerView = (ViewGroup) view.findViewById(R.id.container);
        emptyListText = (TextView)view.findViewById(R.id.empty);

        sessionButton = (Button)view.findViewById(R.id.sessionbutton);
        sessionButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (BodyIQAPIManager.getInstance().isSubscribedToBodyActivities()) {
                    BodyIQAPIManager.getInstance().unsubscribeToBodyActivities();
                    sessionButton.setBackgroundResource(R.drawable.start_session);
                } else {
                    BodyIQAPIManager.getInstance().subscribeToBodyActivities(new BodyIQActivitiesListener(SessionTabFragment.this));
                    sessionButton.setBackgroundResource(R.drawable.stop_session);
                }
            }
        });

        return view;
    }

    public void addItem(final BodyIQInfo bodyInfo) {

        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                final ViewGroup newView = (ViewGroup) LayoutInflater.from(getContext()).inflate(
                        R.layout.sessionrow, containerView, false);

                if (emptyListText.getVisibility() == View.VISIBLE)
                    emptyListText.setVisibility(View.GONE);

                ((TextView) newView.findViewById(R.id.type)).setText(bodyInfo.type);
                ((TextView) newView.findViewById(R.id.timelong)).setText(bodyInfo.status);
                ((TextView) newView.findViewById(R.id.distance)).setText(bodyInfo.distance+" m");
                ((TextView) newView.findViewById(R.id.timeshort)).setText(bodyInfo.duration);

                if(bodyInfo.type.equals("Biking"))
                    (newView.findViewById(R.id.activityicon)).setBackgroundResource(R.drawable.biking);
                else {
                    if (bodyInfo.type.equals("Running"))
                        (newView.findViewById(R.id.activityicon)).setBackgroundResource(R.drawable.running);
                    else
                        (newView.findViewById(R.id.activityicon)).setBackgroundResource(R.drawable.walking);
                }

                containerView.addView(newView, 0);
            }
        });

    }

}
