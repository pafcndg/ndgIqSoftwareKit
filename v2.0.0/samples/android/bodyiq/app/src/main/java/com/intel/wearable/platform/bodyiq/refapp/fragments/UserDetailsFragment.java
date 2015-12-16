

package com.intel.wearable.platform.bodyiq.refapp.fragments;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.activities.UserDetailsActivity;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.customui.CustomToastMessage;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUser;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUserStorageManager;


public class UserDetailsFragment extends Fragment {

    private final int MINIMUM_PASSWORD_CHARACHTERS = 5;
    private ViewGroup rootView = null;

    public static UserDetailsFragment create() {
        UserDetailsFragment fragment = new UserDetailsFragment();
        return fragment;
    }

    public UserDetailsFragment() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {

        rootView = (ViewGroup) inflater
                .inflate(R.layout.userdetails, container, false);

        String[] genders = getResources(). getStringArray(R.array.genderoptions);
        ArrayAdapter<String> adapter = new ArrayAdapter<>(getActivity(), R.layout.spinner, genders);
        ((Spinner) rootView.findViewById(R.id.spin1)).setAdapter(adapter);


        rootView.findViewById(R.id.submit).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            // Check Fields
            if(checkUserDetails()) // Passed all checks
                ((UserDetailsActivity)getActivity()).pager.setCurrentItem(1);

            }
        });

        setHasOptionsMenu(true);
        return rootView;
    }

    boolean isEmailValid(CharSequence email) {
        return android.util.Patterns.EMAIL_ADDRESS.matcher(email).matches();
    }


    public boolean checkUserDetails()
    {
        BodyIQUser user = new BodyIQUser();

        boolean result = true;
        String name[] = ((EditText)rootView.findViewById(R.id.name)).getText().toString().split(" ");
        if(name==null || name.length<2) { // first and last name exist
            result = false;
            new CustomToastMessage(getContext()).showMessage(getResources().getString(R.string.firstandlast));
        }
        else
        {
            user.setName(((EditText)rootView.findViewById(R.id.name)).getText().toString());
            if(!isEmailValid( ((EditText)getView().findViewById(R.id.email)).getText()) )
            {
                result = false;
                new CustomToastMessage(getContext()).showMessage(getResources().getString(R.string.validemail));
            }
            else {
                user.setEmail(((EditText)rootView.findViewById(R.id.email)).getText().toString());

                if (((EditText) getView().findViewById(R.id.password)).getText().toString().length() < MINIMUM_PASSWORD_CHARACHTERS) {
                    result = false;
                    new CustomToastMessage(getContext()).showMessage(getResources().getString(R.string.minpassword));
                }
                else {
                    user.setPassword(((EditText) rootView.findViewById(R.id.password)).getText().toString());

                    switch (((Spinner)rootView.findViewById(R.id.spin1)).getSelectedItemPosition()) {

                        case BodyIQUser.MALE:
                            user.setGender(BodyIQUser.MALE);
                            break;
                        case BodyIQUser.FEMALE:
                            user.setGender(BodyIQUser.FEMALE);
                            break;
                        case BodyIQUser.UNKNOWN:
                            user.setGender(BodyIQUser.UNKNOWN);
                            break;

                    }
                    BodyIQAPIManager.getInstance().saveUser(user);

                }
            }
        }

        return result;

    }


}
