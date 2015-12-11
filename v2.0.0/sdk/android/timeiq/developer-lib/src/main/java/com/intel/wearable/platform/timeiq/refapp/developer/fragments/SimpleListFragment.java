package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.intel.wearable.platform.timeiq.refapp.developer.R;

import java.util.ArrayList;
import java.util.Collection;

/**
 * Created by mleib on 03/03/2015.
 */
public abstract class SimpleListFragment<ListType> extends GeneralFragment {

    protected final ArrayList<ListType> m_entryList = new ArrayList<ListType>();
    private ListView m_listView;
    private ArrayAdapter m_listAdapter;
    private TextView m_titleView;
    private String m_title;

    @Override
    protected void onCreateViewHelper(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        m_rootView = inflater.inflate(R.layout.fragment_list, container, false);
        m_listView = (ListView)m_rootView.findViewById(R.id.fragment_list_list);

        m_listAdapter = new ArrayAdapter(getActivity().getApplicationContext(), R.layout.list_view, m_entryList);
        m_listView.setAdapter(m_listAdapter);

        m_listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                onItemClicked(parent, view, position, id);
            }
        });

        m_titleView = (TextView) m_rootView.findViewById(R.id.fragment_list_title);
        setTitle(m_title);
    }

    protected void setAdapter(ArrayAdapter arrayAdapter) {
        m_listAdapter = arrayAdapter;
        m_listView.setAdapter(m_listAdapter);
    }

    public void onItemClicked(AdapterView<?> parent, View view, int position, long id) { }

    public void addToList(final ListType data) {
        if (data != null) {
            try {
                if (getActivity() != null) {
                    getActivity().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            synchronized(m_entryList) {
                                m_entryList.add(data);
                                notifyDataHasChanged();
                            }
                        }
                    });
                } else {
                    m_entryList.add(data);
                }
            } catch(NullPointerException ex) {
                ex.printStackTrace();
            }
        }
    }

    public void addAllToList(final Collection<ListType> data) {
        if (data != null && !data.isEmpty()) {
            try {
                if (getActivity() != null) {
                    getActivity().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            synchronized(m_entryList) {
                                m_entryList.addAll(data);
                                notifyDataHasChanged();
                            }
                        }
                    });
                } else {
                    m_entryList.addAll(data);
                }
            } catch(NullPointerException ex) {
                ex.printStackTrace();
            }
        }
    }

    public void clearList() {
        try {
            if (getActivity() != null) {
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        synchronized(m_entryList) {
                            m_entryList.clear();
                            notifyDataHasChanged();
                        }
                    }
                });
            } else {
                m_entryList.clear();
            }
        } catch(NullPointerException ex) {
            ex.printStackTrace();
        }
    }

    public void setTitle(String title) {
        m_title = title;
        if (m_titleView != null && m_title != null) {
            m_titleView.setText(m_title);
        }
    }

    private void notifyDataHasChanged() {
        if (m_listAdapter != null) {
            m_listAdapter.notifyDataSetChanged();
        }
    }
}
