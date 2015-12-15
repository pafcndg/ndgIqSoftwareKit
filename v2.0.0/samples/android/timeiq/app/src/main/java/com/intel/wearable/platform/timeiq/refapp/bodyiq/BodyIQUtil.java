package com.intel.wearable.platform.timeiq.refapp.bodyiq;

import android.content.Context;
import android.content.SharedPreferences;
import android.text.format.DateUtils;
import android.util.Log;

import com.intel.wearable.platform.body.Body;
import com.intel.wearable.platform.body.listen.ActivityIntervalListener;
import com.intel.wearable.platform.body.model.ActivityInterval;
import com.intel.wearable.platform.body.model.ActivityStepInterval;
import com.intel.wearable.platform.body.model.BiologicalSex;
import com.intel.wearable.platform.body.model.Profile;
import com.intel.wearable.platform.body.model.TimeSeries;
import com.intel.wearable.platform.body.persistence.ActivityIntervalDao;
import com.intel.wearable.platform.body.persistence.BodyDataStore;
import com.intel.wearable.platform.core.body.WearableBodyActivity;
import com.intel.wearable.platform.core.model.datastore.UserIdentity;
import com.intel.wearable.platform.timeiq.refapp.coreiq.CoreIQUtil;

import org.joda.time.DateTime;

import java.util.Calendar;
import java.util.List;

/**
 * Created by smoradof on 12/3/2015.
 */
public class BodyIQUtil implements ActivityIntervalListener, CoreIQUtil.IConnectionListener {

    public static final boolean USE_BODY_IQ = false;

    private static final String TAG = BodyIQUtil.class.getSimpleName();

    private static final String BODY_IQ_TOKEN_PREFS_FILE = "bodyIqTokenPrefs";
    private static final String STEPS_TOKEN_KEY = "stepsTokenKey";
    private static final String METERS_TOKEN_KEY = "metersTokenKey";
    private static final String LAST_UPDATE_TOKEN_KEY = "lastUpdateTokenKey";


    private static BodyIQUtil mBodyIQUtil = new BodyIQUtil();
    private boolean mIsInitialized;
    private Context mContext;
    private int mTotalSteps;
    private double mTotalMeters;
    private long mLastUpdateTime;
    private WearableBodyActivity.ActivityType mCurrentType;
    private long mCurrentStartTime;
    private int mCurrentStepCount;
    private double mCurrentMetersTraveled;
    private boolean mCurrentlyActive;
    private IBodyIqListener mBodyIqListener;
    private boolean mIsConnected;

    public static BodyIQUtil getInstance(){
        return mBodyIQUtil;
    }

    public BodyIQUtil(){
        Log.d(TAG, "constructor");
    }

    public interface IBodyIqListener{
        public void onUpdate();
    }

    public void setBodyIqListener(IBodyIqListener listener){
        mBodyIqListener = listener;
    }

    public void init(Context context) {
        Log.d(TAG, "init");
        if (!mIsInitialized) {
            mIsInitialized = true;
            Log.d(TAG, "init: initializing");

            SharedPreferences sharedPreferences = context.getSharedPreferences(BODY_IQ_TOKEN_PREFS_FILE, Context.MODE_PRIVATE);
            mTotalSteps = sharedPreferences.getInt(STEPS_TOKEN_KEY, 0);
            mTotalMeters = sharedPreferences.getLong(METERS_TOKEN_KEY, 0);
            mLastUpdateTime = sharedPreferences.getLong(LAST_UPDATE_TOKEN_KEY, 0);

            byte securityKey[] = new byte[64];
            Body.init(context,securityKey);

            mContext = context;
            if ( hasProfile() ) {
                Log.d(TAG, "Found Profile "+ BodyDataStore.getCurrentProfile().getUuid());
            } else {
                initProfile();
            }
            CoreIQUtil.getInstance().setConnectionListener(this);
        }
    }

    @Override
    public void onConnect() {
        final boolean subscriptionSuccess = Body.addActivityListener(this);
        Log.d(TAG, "onConnect: subscribed to bodyEvents: "+subscriptionSuccess);
        resetStepsAndMeters();
        reCalcActivitySeriesForToday();
        mIsConnected = true;
    }

    @Override
    public void onDisconnecting() {
        mIsConnected = false;
        final boolean unSubscriptionSuccess = Body.removeActivityListener(this);
        Log.d(TAG, "onDisconnecting: unSubscribed to bodyEvents: "+unSubscriptionSuccess);
    }

    private void initProfile() {
        final UserIdentity identity = new UserIdentity(
                null, // UUID is set on save
                "External Service Id",
                "First Name",
                "Last Name",
                "Email Address",
                "Phone Number"
        );
        BodyDataStore.setCurrentUserIdentity(identity);
        BodyDataStore.setBiologicalSex(BiologicalSex.FEMALE);
        BodyDataStore.setHeight(168);
        BodyDataStore.setWeight(56);
    }

    private boolean hasProfile() {
        final Profile profile = BodyDataStore.getCurrentProfile();
        return profile.height != Profile.UNKNOWN && profile.weight != Profile.UNKNOWN;
    }

    @Override
    public void onActivityInterval(ActivityInterval interval) {
        mCurrentType = interval.type;
        DateTime startTime = interval.start;
        // DateTime endTime = interval.end;
        WearableBodyActivity.ActivityStatus intervalStatus = interval.status;
        Log.d(TAG, "onActivityInterval intervalStatus: " + intervalStatus.name());
        if(intervalStatus.equals(WearableBodyActivity.ActivityStatus.STARTED)){
            mCurrentStartTime = startTime.getMillis();
            mCurrentlyActive = true;
        }
        else if(interval instanceof ActivityStepInterval){
            ActivityStepInterval stepInterval = (ActivityStepInterval) interval;
            //double caloriesBurned = stepInterval.caloriesBurned;
            double metersTraveled = stepInterval.metersTraveled;
            int stepCount = stepInterval.stepCount;
            boolean isFinished  = intervalStatus.equals(WearableBodyActivity.ActivityStatus.FINISHED);
            boolean isOngoing = intervalStatus.equals(WearableBodyActivity.ActivityStatus.ONGOING);
            Log.d(TAG, "onActivityInterval stepInterval: "+stepInterval+"metersTraveled: "+metersTraveled );
            if( isFinished || isOngoing){
                updateInterval(stepCount, metersTraveled);
                if(isFinished){
                    mCurrentlyActive = false;
                }
            }
        }
        if(mBodyIqListener != null){
            mBodyIqListener.onUpdate();
        }
    }

    private void updateInterval(int stepCount, double metersTraveled) {
        if( ! DateUtils.isToday(mLastUpdateTime)){
            resetStepsAndMeters();
        }
        mCurrentStepCount += stepCount;
        mCurrentMetersTraveled += metersTraveled;
        mLastUpdateTime = System.currentTimeMillis();

        mTotalSteps += stepCount;
        mTotalMeters += metersTraveled;

        SharedPreferences sharedPreferences = mContext.getSharedPreferences(BODY_IQ_TOKEN_PREFS_FILE, Context.MODE_PRIVATE);
        SharedPreferences.Editor edit = sharedPreferences.edit();
        edit.putInt(STEPS_TOKEN_KEY, mTotalSteps);
        edit.putLong(METERS_TOKEN_KEY, (long) mTotalMeters);
        edit.putLong(LAST_UPDATE_TOKEN_KEY, mLastUpdateTime);
        edit.commit();
    }

    private void resetStepsAndMeters(){
        mCurrentStepCount  = 0;
        mCurrentMetersTraveled = 0;
        mTotalSteps = 0;
        mTotalMeters = 0;
    }

    public void reCalcActivitySeriesForToday(){
        // reset the daily count:
        if(CoreIQUtil.getInstance().tryToReconnectIfNeeded()) { // do something only if connected
            mTotalSteps = 0;
            mTotalMeters = 0;
            Log.d(TAG, "reCalcActivitySeriesForToday: connected");
            // get all intervals for today:
            long seriesStart = getStartOfDayInMillis();
            long seriesEnd = System.currentTimeMillis();
            final ActivityIntervalDao intervalDao = new ActivityIntervalDao(Body.context());
            final List<TimeSeries<ActivityInterval>> series = intervalDao.fetchSeries(seriesStart, seriesEnd);
            for (TimeSeries<ActivityInterval> intervals : series) {
                Log.d(TAG, "reCalcActivitySeriesForToday: new TimeSeries");
                for (ActivityInterval activityInterval : intervals) {
                    Log.d(TAG, "reCalcActivitySeriesForToday: new activityInterval status:" + activityInterval.status.name());
                    if (activityInterval instanceof ActivityStepInterval) {
                        ActivityStepInterval stepInterval = (ActivityStepInterval) activityInterval;
                        double metersTraveled = stepInterval.metersTraveled;
                        int stepCount = stepInterval.stepCount;
                        // add counts of this interval to  total
                        mTotalSteps += stepCount;
                        mTotalMeters += metersTraveled;
                        Log.d(TAG, "reCalcActivitySeriesForToday: stepCount: " + stepCount + " metersTraveled: " + metersTraveled);
                    }
                }
            }
            // add the current activity, and save it all
            Log.d(TAG, "reCalcActivitySeriesForToday: adding current: steps: " + mCurrentStepCount + " meters: " + mCurrentMetersTraveled);
            int currentSteps = mCurrentStepCount;
            double currentMeters = mCurrentMetersTraveled;
            mCurrentStepCount = 0;
            mCurrentMetersTraveled = 0;
            updateInterval(currentSteps, currentMeters);
        }
        else{
            Log.d(TAG, "reCalcActivitySeriesForToday: not connected. we tried to connect. when connection is success - we will call reCalcActivitySeriesForToday again");
        }
    }

    public long getStartOfDayInMillis() {
        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        return calendar.getTimeInMillis();
    }


    public int getTotalSteps() {
        return mTotalSteps;
    }

    public double getTotalMeters() {
        return mTotalMeters;
    }

    public WearableBodyActivity.ActivityType getCurrentType() {
        return mCurrentType;
    }

    public long getCurrentStartTime() {
        return mCurrentStartTime;
    }

    public int getCurrentStepCount() {
        return mCurrentStepCount;
    }

    public double getCurrentMetersTraveled() {
        return mCurrentMetersTraveled;
    }

    public boolean isCurrentlyActive() {
        return mCurrentlyActive;
    }

    public boolean isConnected(){
        return mIsConnected;
    }
}
