package com.intel.wearable.platform.timeiq.refapp.coreiq;

import android.content.Context;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.intel.wearable.platform.core.device.WearableToken;
import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.ArrayList;

/**
 * Created by smoradof on 11/16/2015.
 */
public class DevicesListAdapter extends ArrayAdapter {

    private final Context mContext;
    private ArrayList<WearableToken> mDevicesArray;
    private boolean mEnableSelecting;

    public DevicesListAdapter(Context context, int resource, ArrayList<WearableToken> devicesArray) {
        super(context, resource, devicesArray);
        mDevicesArray = devicesArray;
        mContext = context;
    }


    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Context ctx = getContext();
        if (convertView == null) {
            LayoutInflater inflater = (LayoutInflater) ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.device_item, null);
        }

        if (convertView != null) {
            WearableToken wearableToken = mDevicesArray.get(position);
            TextView deviceNameTextView = (TextView) convertView.findViewById(R.id.device_name_TextView);
            TextView deviceIDTextView = (TextView) convertView.findViewById(R.id.device_id_TextView);
            String displayName = wearableToken.getDisplayName();
            deviceNameTextView.setText(displayName);
            deviceIDTextView.setText(wearableToken.getAddress());
            if(TextUtils.isEmpty(displayName)){
                deviceIDTextView.setTextAppearance(mContext, R.style.italicText);
            }
            else{
                deviceIDTextView.setTextAppearance(mContext, R.style.normalText);
            }
        }

        return convertView;
    }
}
