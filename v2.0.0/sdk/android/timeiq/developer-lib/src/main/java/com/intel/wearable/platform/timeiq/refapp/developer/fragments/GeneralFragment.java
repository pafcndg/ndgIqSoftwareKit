package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.util.ArrayList;
import java.util.Collection;

/**
 * Created by mleib on 14/05/2015.
 */
public abstract class GeneralFragment extends Fragment {

    protected View m_rootView;
    private static Collection<GeneralFragment> m_fragmentsForResult = null;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // The last two arguments ensure LayoutParams are inflated properly.
        if (m_rootView != null) {
            ViewGroup parent = (ViewGroup)m_rootView.getParent();
            if (parent != null) {
                parent.removeView(m_rootView);
            }
        }

        onCreateViewHelper(inflater, container, savedInstanceState);

        return m_rootView;
    }

    protected abstract void onCreateViewHelper(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState);

    public void onRemoved() { }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent intent) {
        super.onActivityResult(requestCode, resultCode, intent);

        if (m_fragmentsForResult != null) {
            for (GeneralFragment generalFragment : m_fragmentsForResult) {
                generalFragment.onActivityResult(requestCode, resultCode, intent);
            }
        }
    }

    @Override
    public void startActivityForResult(Intent intent, int requestCode) {
        super.startActivityForResult(intent, requestCode);

        if (m_fragmentsForResult == null) {
            m_fragmentsForResult = new ArrayList<GeneralFragment>();
        }

        if (!m_fragmentsForResult.contains(this)) {
            m_fragmentsForResult.add(this);
        }
    }
}
