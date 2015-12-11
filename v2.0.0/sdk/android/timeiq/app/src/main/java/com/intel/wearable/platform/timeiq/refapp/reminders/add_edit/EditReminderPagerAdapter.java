package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

import android.content.Context;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.ArrayList;

/**
 * A {@link FragmentPagerAdapter} that returns a fragment corresponding to
 * one of the sections/tabs/pages.
 */
public class EditReminderPagerAdapter extends FragmentPagerAdapter {

    private final ArrayList<String> mTitles = new ArrayList<>();
    private final IFragmentFormDataChangedListener mDataChangedListener;

    public enum EDIT_REMINDER_FRAGMENTS {
        FRAGMENT_EDIT_DO     ,
        FRAGMENT_EDIT_CALL   ,
        FRAGMENT_EDIT_NOTIFY ,
        FRAGMENT_EDIT_BE
    }

    public EditReminderPagerAdapter(FragmentManager fragmentManager, Context context, IFragmentFormDataChangedListener dataChangedListener) {
        super(fragmentManager);
        initTitles(context);
        mDataChangedListener = dataChangedListener;
    }

    private void initTitles(Context context) {
        mTitles.add(context.getResources().getString(R.string.tab_title_do));
        mTitles.add(context.getResources().getString(R.string.tab_title_call));
        mTitles.add(context.getResources().getString(R.string.tab_title_notify));
        mTitles.add(context.getResources().getString(R.string.tab_title_be));
    }

        @Override
        public Fragment getItem(int position) {
            Fragment result = null;
            // getItem is called to instantiate the fragment for the given page.
            // Return a PlaceholderFragment (defined as a static inner class below).
            EDIT_REMINDER_FRAGMENTS fragmentIndex = EDIT_REMINDER_FRAGMENTS.values()[position];
            switch (fragmentIndex) {
                case FRAGMENT_EDIT_DO:
                    result = FragmentEditDo.newInstance(mDataChangedListener);
                    break;
                case FRAGMENT_EDIT_CALL:
                    result = FragmentEditCall.newInstance(mDataChangedListener);
                    break;
                case FRAGMENT_EDIT_NOTIFY:
                    result = FragmentEditNotify.newInstance(mDataChangedListener);
                    break;
                case FRAGMENT_EDIT_BE:
                    result = FragmentEditBe.newInstance(mDataChangedListener);
                    break;
            }

            return result;
        }

    @Override
    public int getCount() {
        return EDIT_REMINDER_FRAGMENTS.values().length;
    }

    @Override
    public CharSequence getPageTitle(int position) {
        CharSequence retTitle = null;

        if (position >= 0 && position < mTitles.size()) {
            retTitle = mTitles.get(position);
        }

        return retTitle;
    }

    public int getPageIcon(int position) {
        int retIcon;

        EDIT_REMINDER_FRAGMENTS fragmentItem = EDIT_REMINDER_FRAGMENTS.values()[position];
        switch (fragmentItem) {
            case FRAGMENT_EDIT_DO:
                retIcon = R.drawable.tab_do;
                break;
            case FRAGMENT_EDIT_CALL:
                retIcon = R.drawable.tab_call;
                break;
            case FRAGMENT_EDIT_NOTIFY:
                retIcon = R.drawable.tab_notify;
                break;
            case FRAGMENT_EDIT_BE:
                retIcon = R.drawable.tab_be;
                break;
            default:
                retIcon = -1; // ERROR
        }

        return retIcon;
    }
}
