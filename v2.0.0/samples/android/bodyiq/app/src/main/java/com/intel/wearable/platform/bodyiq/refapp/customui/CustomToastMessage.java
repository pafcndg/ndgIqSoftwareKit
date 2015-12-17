package com.intel.wearable.platform.bodyiq.refapp.customui;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.core.util.Logger;

/**
 * Created by fissaX on 12/10/15.
 */
public class CustomToastMessage {

    Context context = null;
    public CustomToastMessage(Context context)
    {
        this.context = context;
    }

    public void showMessage(String message)
    {
        try {
            LayoutInflater inflater = LayoutInflater.from(context);

            View mainLayout = inflater.inflate(R.layout.customtoast, null);
            View rootLayout = mainLayout.findViewById(R.id.toast_layout);


            TextView text = (TextView) mainLayout.findViewById(R.id.message);
            text.setText(message);

            Toast toast = new Toast(context);
            toast.setDuration(Toast.LENGTH_LONG);

            toast.setView(rootLayout);
            toast.show();
        }
        catch(Exception ex) {
            Logger.e("CustomToastMessage failed: "+ex.getMessage());
        }
    }


}
