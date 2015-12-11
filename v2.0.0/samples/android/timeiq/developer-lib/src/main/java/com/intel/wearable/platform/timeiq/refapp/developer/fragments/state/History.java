package com.intel.wearable.platform.timeiq.refapp.developer.fragments.state;

import com.intel.wearable.platform.timeiq.api.common.protocol.interfaces.IMappable;
import com.intel.wearable.platform.timeiq.dbobjects.abstracts.ATSOBaseDBObject;
import com.intel.wearable.platform.timeiq.dbobjects.abstracts.ATSOSyncDbObject;

import org.apache.commons.collections.buffer.CircularFifoBuffer;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by mleib on 24/09/2015.
 */
public class History implements IMappable {

    private static final int NUMBER_OF_LINES_IN_HISTORY = 100;
    private String m_userId;
    private static final String USER_ID_FIELD = ATSOSyncDbObject.USER_ID_FIELD;
    private String m_id;
    private static final String ID_FIELD = ATSOBaseDBObject.OBJECT_ID_FIELD;
    private CircularFifoBuffer history;
    private static final String HISTORY_FIELD = "history";

    /*package*/ History() {
        history = new CircularFifoBuffer(NUMBER_OF_LINES_IN_HISTORY);
    }

    public ArrayList<StateChangeData> getHistory() {
        return new ArrayList<StateChangeData>(history);
    }

    /*package*/ void add(StateChangeData stateChangeData) {
        history.add(stateChangeData);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        History history1 = (History) o;

        if (m_userId != null ? !m_userId.equals(history1.m_userId) : history1.m_userId != null)
            return false;
        if (m_id != null ? !m_id.equals(history1.m_id) : history1.m_id != null) return false;
        return !(history != null ? !history.equals(history1.history) : history1.history != null);

    }

    @Override
    public int hashCode() {
        int result = m_userId != null ? m_userId.hashCode() : 0;
        result = 31 * result + (m_id != null ? m_id.hashCode() : 0);
        result = 31 * result + (history != null ? history.hashCode() : 0);
        return result;
    }
    
    @Override
    public void initObjectFromMap(Map<String, Object> stringObjectMap) {
        if(stringObjectMap != null) {
            if(stringObjectMap.containsKey(USER_ID_FIELD)) {
                this.m_userId = (String)stringObjectMap.get(USER_ID_FIELD);
            }

            if(stringObjectMap.containsKey(ID_FIELD)) {
                this.m_id = (String)stringObjectMap.get(ID_FIELD);
            }

            if (stringObjectMap.containsKey(HISTORY_FIELD)) {
                history = new CircularFifoBuffer(NUMBER_OF_LINES_IN_HISTORY);
                final Object historyField = stringObjectMap.get(HISTORY_FIELD);
                if (historyField instanceof ArrayList) {
                    ArrayList StateChangeArrayList = (ArrayList) stringObjectMap.get(HISTORY_FIELD);
                    if (StateChangeArrayList != null) {
                        StateChangeData StateChangeData;
                        for (int i = 0; i < StateChangeArrayList.size(); ++i) {
                            StateChangeData = new StateChangeData();
                            StateChangeData.initObjectFromMap((Map) StateChangeArrayList.get(i));
                            history.add(StateChangeData);
                        }
                    }
                }
            }
        }
    }

    @Override
    public Map<String, Object> objectToMap() {
        Map<String, Object> objectMap = new HashMap<>();

        if(m_userId != null) { objectMap.put(USER_ID_FIELD, m_userId); }
        if(m_id != null) { objectMap.put(ID_FIELD, m_id); }
        if(history != null && !history.isEmpty()) {
            StateChangeData StateChangeData;
            final ArrayList<StateChangeData> history = getHistory();
            final ArrayList<Map<String, Object>> dataToStore = new ArrayList<Map<String, Object>>(history.size());
            for(int i=0 ; i< history.size() ; ++i) {
                StateChangeData = history.get(i);
                if (StateChangeData != null) {
                    dataToStore.add(StateChangeData.objectToMap());
                }
            }

            objectMap.put(HISTORY_FIELD, dataToStore);
        }

        return objectMap;
    }

}
