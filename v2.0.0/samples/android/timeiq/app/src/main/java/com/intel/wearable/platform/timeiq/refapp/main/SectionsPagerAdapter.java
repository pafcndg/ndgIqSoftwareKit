package com.intel.wearable.platform.timeiq.refapp.main;

import android.content.Context;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

import com.intel.wearable.platform.timeiq.refapp.R;
import com.intel.wearable.platform.timeiq.refapp.bodyiq.BodyIQFragment;
import com.intel.wearable.platform.timeiq.refapp.bodyiq.BodyIQUtil;
import com.intel.wearable.platform.timeiq.refapp.events.UpcomingEventsFragment;
import com.intel.wearable.platform.timeiq.refapp.places.PlacesFragment;
import com.intel.wearable.platform.timeiq.refapp.reminders.RemindersFragment;

import java.util.ArrayList;

/**
 * A {@link FragmentPagerAdapter} that returns a fragment corresponding to
 * one of the sections/tabs/pages.
 */
/*package*/ class SectionsPagerAdapter extends FragmentPagerAdapter {

    private final ArrayList<String> mTitles = new ArrayList<>();

    public SectionsPagerAdapter(FragmentManager fragmentManager, Context context) {
        super(fragmentManager);
        initTitles(context);
    }

    private void initTitles(Context context) {
        mTitles.add(context.getResources().getString(R.string.reminders_tab_title));
        mTitles.add(context.getResources().getString(R.string.upcoming_events_tab_title));
        mTitles.add(context.getResources().getString(R.string.places_tab_title));
        if(BodyIQUtil.USE_BODY_IQ) {
            mTitles.add(context.getResources().getString(R.string.bodyiq_tab_title));
        }
    }

    @Override
    public Fragment getItem(int position) {
        Fragment result = null;

        if (position >= 0 && position < mTitles.size()) {
            switch (position) {
                case 0:
                    result = new RemindersFragment();
                    break;
                case 1:
                    result = new UpcomingEventsFragment();
                    break;
                case 2:
                    result = new PlacesFragment();
                    break;
                case 3:
                    result = new BodyIQFragment();
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
