package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.ListFragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.common.db.IDaoCommonMethods;
import com.intel.wearable.platform.timeiq.common.ioc.ClassFactory;
import com.intel.wearable.platform.timeiq.common.utils.time.TimeFormatUtil;
import com.intel.wearable.platform.timeiq.dblayer.DaoFactory;
import com.intel.wearable.platform.timeiq.dblayer.interfaces.IGenericDaoImpl;
import com.intel.wearable.platform.timeiq.dblayer.sync.SyncObject;
import com.intel.wearable.platform.timeiq.dblayer.sync.decorators.GenericSyncDaoDecorator;
import com.intel.wearable.platform.timeiq.dbobjects.interfaces.ITSOBaseDBObject;
import com.intel.wearable.platform.timeiq.dbobjects.interfaces.ITSOSyncDbObject;
import com.intel.wearable.platform.timeiq.exception.TSODBException;
import com.intel.wearable.platform.timeiq.places.modules.persistence.syncdb.daoimpls.modules.ISyncDao;
import com.intel.wearable.platform.timeiq.protocol.response.RetCode;
import com.intel.wearable.platform.timeiq.refapp.developer.R;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public abstract class DeveloperDaoListFragment<T extends ITSOBaseDBObject> extends ListFragment implements IDeveloperFragment {

    private static final String TAG = DeveloperDaoListFragment.class.getSimpleName();
    protected View m_rootView;
    private TextView listCountTextView;
    private TextView listSyncStateTextView;
    private TextView listSyncStateTimeView;
    protected final List<T> m_data = new ArrayList<T>();
    protected final Class<T> m_typeParameterClass;
    protected boolean m_isSyncable;
    private Context m_context;

    protected DeveloperDaoListFragment(Class<T> typeParameterClass) {
        m_typeParameterClass = typeParameterClass;
        m_isSyncable = ITSOSyncDbObject.class.isAssignableFrom(m_typeParameterClass);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        if (m_rootView != null) {
            ViewGroup parent = (ViewGroup) m_rootView.getParent();
            if (parent != null)
                parent.removeView(m_rootView);
        }
        m_rootView = inflater.inflate(R.layout.fragment_developer_list, container, false);
        listCountTextView = (TextView) m_rootView.findViewById(R.id.list_count);
        listSyncStateTextView = (TextView) m_rootView.findViewById(R.id.list_sync_state);
        listSyncStateTimeView = (TextView) m_rootView.findViewById(R.id.list_sync_time);
        //hide sync views for non syncable types
        if (!m_isSyncable) {
            listSyncStateTextView.setVisibility(View.GONE);
            listSyncStateTimeView.setVisibility(View.GONE);
        }

        m_context = getActivity().getApplicationContext();

        return m_rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
        if (isAdded()) {
            refreshView();
        }
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        ListAdapter adapter = new ArrayAdapter<T>(m_context, R.layout.list_view, m_data);
        setListAdapter(adapter);
    }

    @Override
    public void onListItemClick(ListView l, View v, int position, long id) {
        super.onListItemClick(l, v, position, id);
        ITSOBaseDBObject dbObj = m_data.get(position);
        if (dbObj != null) {
            Log.d(TAG, "onListItemClick() item clicked:" + dbObj.toString());
        }
    }

    public void refreshView() {
        try {
            if (getActivity() != null) {
                final Collection<T> newData = getUpdatedDataSortedByCreateTime();
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        refreshTextViews(newData);
                        refreshListView(newData);
                    }
                });
            }
        } catch (Exception e) {
            Log.e(TAG, "refreshView() threw exception", e);
        }
    }

    protected Collection<T> getUpdatedDataSortedByCreateTime() {
        List<T> data;
        try {
            IGenericDaoImpl<T> dao = DaoFactory.getDaoBySourceType(m_typeParameterClass);
            data = dao.getAllObjectsByUserId(null);
            Collections.sort(data, new Comparator<T>() {
                @Override
                public int compare(T a, T b) {
                    long aVal = a.getCreationTime();
                    long bVal = b.getCreationTime();

                    if (aVal == bVal)
                        return 0;

                    if (aVal < bVal)
                        return 1;

                    return -1;
                }
            });
        } catch (Exception e) {
            data = new ArrayList<>(0);
            Log.e(TAG, "getUpdatedData() threw exception", e);
        }
        return data;
    }

    private void refreshTextViews(Collection<T> data) {
        try {
            IDaoCommonMethods idaoCommonMethods = ClassFactory.getInstance().resolve(IDaoCommonMethods.class);
            listCountTextView.setText(m_context.getString(R.string.developer_fragment_list_object_amount, data.size()));
            listCountTextView.invalidate();

            if (m_isSyncable) {
                Class<ITSOSyncDbObject> syncClass = (Class<ITSOSyncDbObject>) m_typeParameterClass;
                IGenericDaoImpl<ITSOSyncDbObject> dao = DaoFactory.getDaoBySourceType(syncClass);
                GenericSyncDaoDecorator<?> syncDao = idaoCommonMethods.extractGenericSyncDao(dao);
                if (syncDao != null) {
                    long lastSyncTimeStamp = syncDao.getLastSyncTimeStamp();
                    if (lastSyncTimeStamp == -1) {
                        listSyncStateTextView.setText(m_context.getString(R.string.developer_fragment_list_sync_state_not_synced));
                        listSyncStateTimeView.setText(m_context.getString(R.string.developer_fragment_list_sync_time_not_synced));
                    } else {
                        setSyncStateViewTextAndColor(syncDao.getLastSyncStatus());
                        listSyncStateTimeView.setText(m_context.getString(R.string.developer_fragment_list_sync_time, TimeFormatUtil.timeToStr(lastSyncTimeStamp)));
                    }

                    listSyncStateTextView.invalidate();
                    listSyncStateTimeView.invalidate();
                }
            }
        } catch (Exception ex) {
            Log.e(TAG, "refreshTextViews() throw exception", ex);
        }
    }

    private void setSyncStateViewTextAndColor(RetCode state) {
        ISyncDao syncDao = (ISyncDao) DaoFactory.getDaoBySourceType(SyncObject.class);

        if (state == RetCode.RETCODE_NA && syncDao != null) {
            try {
                Collection<SyncObject> allObjectsBySourceType = syncDao.getAllObjectsBySourceType(null, m_typeParameterClass);
                if (allObjectsBySourceType.isEmpty()) {
                    state = RetCode.FULLY_SYNCED;
                } else {
                    state = RetCode.PENDING_SYNC;
                }
            } catch (TSODBException e) {
                Log.e(TAG, "setSyncStateViewTextAndColor() throw exception", e);
            }
        }

        if (state == RetCode.FULLY_SYNCED) {
            listSyncStateTextView.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
        } else if (state == RetCode.PENDING_SYNC) {
            listSyncStateTextView.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
        }

        listSyncStateTextView.setText(m_context.getString(R.string.developer_fragment_list_sync_state, state.name()));
    }

    private void refreshListView(Collection<T> newData) {
        m_data.clear();
        m_data.addAll(newData);
        ListAdapter adapter = getListAdapter();
        if (adapter != null) {
            ((ArrayAdapter) adapter).notifyDataSetChanged();
        }
    }

    @Override
    public void onFragmentHidden() {
        Log.d(TAG, "onFragmentHidden");
    }

    @Override
    public void onFragmentShown() {
        Log.d(TAG, "onFragmentShown");
        refreshAndSetFloatingButton();
    }

    private void refreshAndSetFloatingButton() {
        refreshView();
        Activity activity = getActivity();
        if (activity != null) {
            FloatingActionButton floatingActionButton = (FloatingActionButton) activity.findViewById(R.id.fab);
            if (floatingActionButton != null) {
                floatingActionButton.setVisibility(View.VISIBLE);
                floatingActionButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        refreshView();
                    }
                });
            }
        }
    }

}