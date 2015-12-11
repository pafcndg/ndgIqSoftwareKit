package com.intel.wearable.platform.timeiq.refapp.developer.fragments.StateHistory;

import com.intel.wearable.platform.timeiq.api.userstate.IUserStateChangeListener;
import com.intel.wearable.platform.timeiq.api.userstate.UserState;
import com.intel.wearable.platform.timeiq.api.userstate.UserStateChanges;
import com.intel.wearable.platform.timeiq.api.userstate.UserStateData;
import com.intel.wearable.platform.timeiq.api.userstate.UserStateType;
import com.intel.wearable.platform.timeiq.api.userstate.VisitedPlaces;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.common.logger.placeslogger.ITSOLoggerPlaces;
import com.intel.wearable.platform.timeiq.common.logger.placeslogger.LOG_LEVEL;
import com.intel.wearable.platform.timeiq.common.timer.ITSOAlarmListener;
import com.intel.wearable.platform.timeiq.common.timer.ITSOAlarmManager;
import com.intel.wearable.platform.timeiq.dblayer.DaoFactory;
import com.intel.wearable.platform.timeiq.dblayer.interfaces.IGenericDaoImpl;
import com.intel.wearable.platform.timeiq.exception.TSODBException;
import com.intel.wearable.platform.timeiq.refapp.developer.fragments.state.DeveloperHistoryManager;
import com.intel.wearable.platform.timeiq.userstate.IUserStateManagerModule;
import com.intel.wearable.platform.timeiq.userstate.audit.UserStateDataVisitsAudit;
import com.intel.wearable.platform.timeiq.userstate.audit.UserStateHistory;

import java.util.Calendar;
import java.util.Iterator;
import java.util.List;

/**
 * Created by gilsharo on 11/26/15.
 */
public class UserStateHistoryCollector implements IUserStateChangeListener, ITSOAlarmListener {

    private static final String TAG = UserStateHistoryCollector.class.getSimpleName();
    private static final String ALARM_ID = "TRIM_USER_STATE_HISTORY";
    private static final int KEEP_DAYS = 7;

    private final DeveloperHistoryManager m_developerHistoryManager;
    private final ITSOLoggerPlaces m_logger;
    private final ITSOAlarmManager m_alarmManager;
    private final IUserStateManagerModule m_userStateManager;
    private final IGenericDaoImpl<UserStateHistory> m_userStateHistoryDao;

    public UserStateHistoryCollector(DeveloperHistoryManager developerHistoryManager) {
        this(developerHistoryManager, ClassFactory.getInstance());
    }

    public UserStateHistoryCollector(DeveloperHistoryManager developerHistoryManager, ClassFactory classFactory) {
        this(   developerHistoryManager,
                classFactory.resolve(ITSOLoggerPlaces.class),
                classFactory.resolve(ITSOAlarmManager.class),
                DaoFactory.getDaoBySourceType(UserStateHistory.class),
                classFactory.resolve(IUserStateManagerModule.class));
    }

    public UserStateHistoryCollector(final DeveloperHistoryManager developerHistoryManager,
                                     final ITSOLoggerPlaces logger,
                                     final ITSOAlarmManager alarmManager,
                                     final IGenericDaoImpl<UserStateHistory> userStateHistoryDao,
                                     final IUserStateManagerModule userStateManager) {
        m_developerHistoryManager = developerHistoryManager;
        m_logger = logger;
        m_alarmManager = alarmManager;
        m_userStateHistoryDao = userStateHistoryDao;
        m_userStateManager = userStateManager;
    }

    public void start() {
        UserState state = m_userStateManager.registerForChanges(this);
        if (state != null) {
            collect(true, state.getVisits());
        }
        scheduleTrimAlarm();
    }

    public void stop() {
        m_alarmManager.cancelAlarm(this, ALARM_ID);
        m_userStateManager.unregisterForChanges(this);
    }

    @Override
    public void onStateChanged(UserState oldState, UserState newState, UserStateChanges changes) {
        if (changes.isChanged(UserStateType.VISIT)) {
            collect(false, newState.getVisits());
        }
    }

    private void collect(boolean initialState, UserStateData<VisitedPlaces> vips) {
        UserStateDataVisitsAudit vipsAudit = (vips!=null)? new UserStateDataVisitsAudit(vips) : null;
        UserStateHistory userStateHistory = new UserStateHistory(initialState, vipsAudit);
        try {
            m_userStateHistoryDao.addObject(userStateHistory);
            notifyDataChanged();
        } catch (TSODBException e) {
            e.printStackTrace();
           m_logger.addToLog(LOG_LEVEL.ERROR, TAG, "Exception while adding StateHistory object");
        }
    }

    private void scheduleTrimAlarm() {
        Calendar c = Calendar.getInstance();
        long now = c.getTimeInMillis();
        c.add(Calendar.DATE, 1);
        c.set(Calendar.HOUR_OF_DAY, 0);
        c.set(Calendar.MINUTE, 0);
        c.set(Calendar.SECOND, 0);
        c.set(Calendar.MILLISECOND, 0);
        long millisecondsUntilMidnight = c.getTimeInMillis() - now;

        m_alarmManager.setApproximateAlarm(this, ALARM_ID, ALARM_ID, null, millisecondsUntilMidnight);
    }

    @Override
    public void onAlarm(String action, String id, String data) {
        trimData();
        scheduleTrimAlarm();
    }

    private void trimData() {
        Calendar c = Calendar.getInstance();
        c.add(Calendar.DATE, -KEEP_DAYS);
        c.set(Calendar.HOUR_OF_DAY, 0);
        c.set(Calendar.MINUTE, 0);
        c.set(Calendar.SECOND, 0);
        c.set(Calendar.MILLISECOND, 0);
        long keepTime = c.getTimeInMillis();
        try {
            List<UserStateHistory> allObjs = m_userStateHistoryDao.getAllObjectsByUserId(null);
            Iterator<UserStateHistory> it = allObjs.iterator();
            while( it.hasNext() ) {
                UserStateHistory obj = it.next();
                if( obj.getCreationTime() > keepTime) {
                    //remove all newer objects from list
                    it.remove();
                }
            }
            if (allObjs.size() > 0) {
                //delete all objects in list
                m_userStateHistoryDao.deleteObjects(allObjs);
                notifyDataChanged();
            }
        } catch (TSODBException e) {
            e.printStackTrace();
        }
    }

    private void notifyDataChanged() {
        m_developerHistoryManager.dataHasChanged();
    }
}
