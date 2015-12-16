package com.intel.wearable.platform.bodyiq.refapp.activities;

import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import com.intel.wearable.platform.bodyiq.refapp.R;
import com.intel.wearable.platform.bodyiq.refapp.apimanager.BodyIQAPIManager;
import com.intel.wearable.platform.bodyiq.refapp.customui.CustomToastMessage;
import com.intel.wearable.platform.bodyiq.refapp.fragments.UserDetailsFragment;
import com.intel.wearable.platform.bodyiq.refapp.fragments.UserExtraDetailsFragment;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUser;
import com.intel.wearable.platform.bodyiq.refapp.usermanager.BodyIQUserStorageManager;

/**
 *
 * @see UserDetailsFragment and UserExtraDetailsFragment
 */
public class UserDetailsActivity extends AppCompatActivity {
    /**
     * The number of login steps
     */
    private static final int NUM_PAGES = 3;
    public static final int USER_DETAILS_FRAGMENT = 0;
    public static final int WEIGHT_FRAGMENT = 1;
    public static final int HEIGHT_FRAGMENT = 2;

    public ViewPager pager;

    private PagerAdapter pagerAdapter;
    private UserDetailsFragment userDetailsFragment = null;
    private UserExtraDetailsFragment weightFragment = null;
    private UserExtraDetailsFragment heightFragment = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.userdetailsslide);

        ColorDrawable colorDrawable = new ColorDrawable(ContextCompat.getColor(this, R.color.blue));
        getSupportActionBar().setBackgroundDrawable(colorDrawable);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setTitle(R.string.createaccount);

        // Instantiate a ViewPager and a PagerAdapter.
        pager = (ViewPager) findViewById(R.id.pager);


        pagerAdapter = new ScreenSlidePagerAdapter(getSupportFragmentManager());
        pager.setAdapter(pagerAdapter);
        pager.addOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener() {
            @Override
            public void onPageSelected(int position) {
                switch (position) {
                    case USER_DETAILS_FRAGMENT:
                        getSupportActionBar().setTitle(R.string.createaccount);
                        break;
                    case WEIGHT_FRAGMENT:
                        if(!userDetailsFragment.checkUserDetails())
                            pager.setCurrentItem(USER_DETAILS_FRAGMENT);
                        getSupportActionBar().setTitle(R.string.currentweight);
                        break;
                    case HEIGHT_FRAGMENT:
                        getSupportActionBar().setTitle(R.string.currentheight);
                        break;
                }

            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.userdetails, menu);

        MenuItem item = menu.findItem(R.id.done);
        if(pager.getCurrentItem() != (NUM_PAGES-1))
        {
            item.setVisible(false);
        }
        else
            item.setVisible(true);

        return true;
    }



    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                if(pager.getCurrentItem()==0)
                    onBackPressed();
                else
                    pager.setCurrentItem(pager.getCurrentItem()-1);

                return true;
            case R.id.done:
                // check weight and height values

                if (weightFragment.getCustomNumberValue() == 0)  // Weight value is bad, go to weight page
                {
                    new CustomToastMessage(this).showMessage(getResources().getString(R.string.invalidweight));
                    pager.setCurrentItem(WEIGHT_FRAGMENT);
                } else {
                    if (heightFragment.getCustomNumberValue() == 0) // Height value is bad, go to height page
                    {
                        new CustomToastMessage(this).showMessage(getResources().getString(R.string.invalidheight));
                        pager.setCurrentItem(HEIGHT_FRAGMENT);
                    } else {
                        // All values are good
                        BodyIQUser user = BodyIQAPIManager.getInstance().getUser();
                        user.setWeight(weightFragment.getCustomNumberValue());
                        user.setHeight(heightFragment.getCustomNumberValue());
                        BodyIQAPIManager.getInstance().saveUser();
                        Intent intent = new Intent(UserDetailsActivity.this, DeviceSyncActivity.class);
                        startActivity(intent);
                    }

                }

                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }


    /**
     * A simple pager adapter that represents 5 {@link UserDetailsFragment} objects, in
     * sequence.
     */
    private class ScreenSlidePagerAdapter extends FragmentStatePagerAdapter {
        public ScreenSlidePagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int position) {

            Fragment result = null;
            switch (position)
            {
                case USER_DETAILS_FRAGMENT:
                    result = userDetailsFragment = UserDetailsFragment.create();
                    break;
                case WEIGHT_FRAGMENT:
                    result = weightFragment = UserExtraDetailsFragment.create(position);
                    break;
                case HEIGHT_FRAGMENT:
                    result = heightFragment = UserExtraDetailsFragment.create(position);
                    break;

            }
            return result;
        }

        @Override
        public int getCount() {
            return NUM_PAGES;
        }
    }
}
