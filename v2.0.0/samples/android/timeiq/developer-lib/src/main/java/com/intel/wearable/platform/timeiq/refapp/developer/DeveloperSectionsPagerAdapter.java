package com.intel.wearable.platform.timeiq.refapp.developer;

import android.content.Context;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

import com.intel.wearable.platform.timeiq.refapp.developer.fragments.DeveloperDetectedPlaceFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.DeveloperFeedbackFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.DeveloperManualPlaceFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.DeveloperSemanticDetectedPlaceFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.DeveloperVipFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.DeveloperVisitsFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.StateHistory.DeveloperStateHistoryFragment;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.state.StateFragment;

import java.util.ArrayList;

public class DeveloperSectionsPagerAdapter extends FragmentPagerAdapter {

    private final ArrayList<String> mTitles = new ArrayList<>();

    public DeveloperSectionsPagerAdapter(FragmentManager fragmentManager, Context context) {
        super(fragmentManager);
        initTitles(context);
    }

    private void initTitles(Context context) {
        mTitles.add(context.getResources().getString(R.string.developer_fragment_feedback_title));
        mTitles.add(context.getResources().getString(R.string.developer_fragment_state_title));
        mTitles.add(context.getResources().getString(R.string.developer_fragment_visits_title));
        mTitles.add(context.getResources().getString(R.string.developer_fragment_manual_places_title));
        mTitles.add(context.getResources().getString(R.string.developer_fragment_detected_places_title));
        mTitles.add(context.getResources().getString(R.string.developer_fragment_vip_title));
        mTitles.add(context.getResources().getString(R.string.developer_fragment_semantic_detected_places_title));
        mTitles.add(context.getResources().getString(R.string.developer_fragment_state_history_title));
    }

        @Override
    public Fragment getItem(int position) {
        Fragment result = null;

        if (position >= 0 && position < mTitles.size()) {
            switch (position) {
                case 0:
                    result = new DeveloperFeedbackFragment();
                    break;
                case 1:
                    result = new StateFragment();
                    break;
                case 2:
                    result = new DeveloperVisitsFragment();
                    break;
                case 3:
                    result = new DeveloperManualPlaceFragment();
                    break;
                case 4:
                    result = new DeveloperDetectedPlaceFragment();
                    break;
                case 5:
                    result = new DeveloperVipFragment();
                    break;
                case 6:
                    result = new DeveloperSemanticDetectedPlaceFragment();
                    break;
                case 7:
                    result = new DeveloperStateHistoryFragment();
                    break;
            }
        }

        return result;
    }

    @Override
    public int getCount() {
        return mTitles.size();
    }

    @Override
    public CharSequence getPageTitle(int position) {
        CharSequence result = null;

        if (position >= 0 && position < mTitles.size()) {
            result = mTitles.get(position);
        }

        return result;
    }

}
