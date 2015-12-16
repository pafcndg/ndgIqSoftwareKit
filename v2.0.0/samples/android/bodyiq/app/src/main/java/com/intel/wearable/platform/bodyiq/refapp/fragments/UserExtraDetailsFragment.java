/*
 * Copyright 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.wearable.platform.bodyiq.refapp.fragments;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.activities.UserDetailsActivity;
import com.intel.wearable.platform.bodyiq.refapp.customui.CustomNumber;


public class UserExtraDetailsFragment extends Fragment {

    static final String ARG_PAGE = "page";
    private ViewGroup rootView = null;

    private int pageNumber;


    public static UserExtraDetailsFragment create(int pageNumber) {
        UserExtraDetailsFragment fragment = new UserExtraDetailsFragment();
        Bundle args = new Bundle();
        args.putInt(ARG_PAGE, pageNumber);
        fragment.setArguments(args);
        return fragment;
    }

    public UserExtraDetailsFragment() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        pageNumber = getArguments().getInt(ARG_PAGE);
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        // Inflate the layout containing a title and body text.
        rootView = (ViewGroup) inflater
                .inflate(R.layout.userextradetails, container, false);

        switch (getPageNumber())
        {

            case UserDetailsActivity.WEIGHT_FRAGMENT:
                ((TextView) rootView.findViewById(R.id.unit)).setText(R.string.weightunit);
                ((TextView) rootView.findViewById(R.id.description)).setText(R.string.hintweight);
                break;
            case UserDetailsActivity.HEIGHT_FRAGMENT:
                ((TextView) rootView.findViewById(R.id.unit)).setText(R.string.heightunit);
                 ((TextView) rootView.findViewById(R.id.description)).setText(R.string.hintheight);
                break;

        }

        setHasOptionsMenu(true);
        return rootView;
    }

    public int getCustomNumberValue()
    {
        return ((CustomNumber)rootView.findViewById(R.id.customnumber)).getCustomNumberValue();

    }

    public int getPageNumber() {
        return pageNumber;
    }


}
