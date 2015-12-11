package com.intel.wearable.platform.timeiq.refapp.googleAnalytics;

import android.content.Context;

import com.google.android.gms.analytics.GoogleAnalytics;
import com.google.android.gms.analytics.HitBuilders;
import com.google.android.gms.analytics.Tracker;
import com.intel.wearable.platform.timeiq.refapp.R;

import java.util.HashMap;
import java.util.Map;

public class GoogleAnalyticsTrackers {

    public enum Target {
        APP
    }
// TODO - decide on a upload time under app_tracker.xml --> ga_dispatchPeriod & ga_sampleFrequency
// TODO - try to make app_tracker.xml --> ga_reportUncaughtExceptions work
    private static final Object mLock = new Object();
    private static GoogleAnalyticsTrackers mInstance;
    private final Map<Target, Tracker> mTrackers = new HashMap<>();
    private final Context mContext;

    public static synchronized void initialize(Context context) {
        if (mInstance == null) {
            synchronized (mLock) {
                if (mInstance == null) {
                    mInstance = new GoogleAnalyticsTrackers(context);
                }
            }
        }
    }

    public static synchronized GoogleAnalyticsTrackers getInstance() {
        if (mInstance == null) {
            throw new IllegalStateException("Call initialize() before getInstance()");
        }

        return mInstance;
    }

    private GoogleAnalyticsTrackers(Context context) {
        mContext = context.getApplicationContext();
    }

    public void trackEvent(int categoryResource, int actionResource, int labelResource) {
        String category = mContext.getString(categoryResource);
        String action = mContext.getString(actionResource);
        String label = mContext.getString(labelResource);
        trackEvent(category, action, label);
    }

    public void trackEvent(int categoryResource, int actionResource, String label) {
        String category = mContext.getString(categoryResource);
        String action = mContext.getString(actionResource);
        trackEvent(category, action, label);
    }

    public void trackEvent(String category, String action, String label) {
        Tracker tracker = getTracker(GoogleAnalyticsTrackers.Target.APP);
        tracker.send(new HitBuilders.EventBuilder().setCategory(category).setAction(action).setLabel(label).build());
    }

    private synchronized Tracker getTracker(Target target) {
        if (!mTrackers.containsKey(target)) {
            Tracker tracker;
            switch (target) {
                case APP:
                    tracker = GoogleAnalytics.getInstance(mContext).newTracker(R.xml.app_tracker);
                    break;
                default:
                    throw new IllegalArgumentException("Unhandled analytics target " + target);
            }
            mTrackers.put(target, tracker);
        }

        return mTrackers.get(target);
    }

}
