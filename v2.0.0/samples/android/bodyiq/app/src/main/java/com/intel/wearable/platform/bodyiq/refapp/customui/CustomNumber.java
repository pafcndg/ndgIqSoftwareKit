package com.intel.wearable.platform.bodyiq.refapp.customui;

import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.AttributeSet;
import android.view.View;
import android.widget.EditText;
import android.widget.LinearLayout;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.core.util.Logger;


/**
 * Created by fissaX on 12/8/15.
 */
public class CustomNumber extends LinearLayout {

    EditText hundredsDigit = null;
    EditText tensDigit = null;
    EditText onesDigit = null;


    public CustomNumber(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
        View view = inflate(context, R.layout.customnumber, null);
        this.addView(view);

        hundredsDigit = (EditText) view.findViewById(R.id.hundreds);
        tensDigit = (EditText) view.findViewById(R.id.tens);
        onesDigit = (EditText) view.findViewById(R.id.ones);
        hundredsDigit.addTextChangedListener(new OneCharacterListener());
        tensDigit.addTextChangedListener(new OneCharacterListener());

    }


    class OneCharacterListener implements TextWatcher
    {

        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {

        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {

        }

        @Override
        public void afterTextChanged(Editable s) {
            if (s.length() == 1) {
                if(hundredsDigit.getEditableText()==s)
                    tensDigit.requestFocus();
                else {
                    if (tensDigit.getEditableText() == s)
                        onesDigit.requestFocus();
                }
            }
        }
    }


    public int getCustomNumberValue()
    {
        int res = 0;

        try {
            if( onesDigit.getText().toString().length()>0) // minimum one digit filled
            {
                if( (tensDigit.getText().toString().length()==0) && (hundredsDigit.getText().toString().length()==0) )  // case 001
                {
                    res += Integer.parseInt(onesDigit.getText().toString());
                }
                else
                {
                    if( (tensDigit.getText().toString().length()>0) && (hundredsDigit.getText().toString().length()==0) )  // case 011
                    {
                        res += Integer.parseInt(onesDigit.getText().toString());
                        res += (Integer.parseInt(tensDigit.getText().toString())*10);
                    }
                    else
                    {
                        if( (tensDigit.getText().toString().length()>0) && (hundredsDigit.getText().toString().length()>0) ) // case 111
                        {
                            res += Integer.parseInt(onesDigit.getText().toString());
                            res += (Integer.parseInt(tensDigit.getText().toString())*10);
                            res += (Integer.parseInt(hundredsDigit.getText().toString())*100);
                        }
                    }
                }
            }

        }catch (NumberFormatException e)
        {
            Logger.e("Height/Weight bad characters " + e.getMessage());
            res = 0;
        }

        return res;
    }

}
