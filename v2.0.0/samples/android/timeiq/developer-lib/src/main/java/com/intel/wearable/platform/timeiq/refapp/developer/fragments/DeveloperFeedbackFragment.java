package com.intel.wearable.platform.timeiq.refapp.developer.fragments;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.Fragment;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.intel.wearable.platform.timeiq.common.storage.db.ADbManager;
import com.intel.wearable.platform.timeiq.common.system.SystemUtilsHelper;
import com.intel.wearable.platform.timeiq.platform.android.sensors.utils.MailManager;
import com.intel.wearable.platform.timeiq.refapp.developer.BuildConfig;
import com.intel.wearable.platform.timeiq.refapp.developer.R;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.channels.FileChannel;

public class DeveloperFeedbackFragment extends Fragment implements IDeveloperFragment {

    private static final String TAG = DeveloperFeedbackFragment.class.getSimpleName();
    private static final String[] EMAILS = { "timeiq.ref.app.feedback@intel.com" };
    private EditText mFeedbackText;
    private InputMethodManager mInputMethodManager;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_developer_feedback, container, false);
        mFeedbackText = (EditText) rootView.findViewById(R.id.developer_feedback_text);
        final Button sendFeedbackButton = (Button) rootView.findViewById(R.id.developer_feedback_button_send);
        sendFeedbackButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                sendFeedback();
            }
        });

        mInputMethodManager =  (InputMethodManager) getActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
        refreshAndSetFloatingButton();

        return rootView;
    }

    private void sendFeedback() {
        Context context = getActivity();
        Toast.makeText(context, "Preparing feedback", Toast.LENGTH_SHORT).show();
        exportDbFileToSD(context);
        MailManager mailManager = new MailManager(context);
        String bodyText = mFeedbackText.getText().toString();
        if (TextUtils.isEmpty(bodyText)) {
            bodyText = "";
        }

        String versionName = "";
        try {
            versionName = getActivity().getPackageManager().getPackageInfo(getActivity().getPackageName(), 0).versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        mailManager.sendAndroidFeedbackMail(getString(R.string.developer_feedback_title_prefix) + " (" + versionName + " " + BuildConfig.BUILD_DATE_SHORT + ")", bodyText, EMAILS);
    }

    private void exportDbFileToSD(Context context) {
        String databaseName = ADbManager.MAIN_DB_NAME;
        try {
            File sd = SystemUtilsHelper.getSystemRepositoryFolder();
            File data = Environment.getDataDirectory(); //Should be supported also by SystemUtilsHelper

            if (sd.canWrite()) {
                String currentDBPath = "//data//" + context.getPackageName() + "//databases//" + databaseName + "";
                File currentDB = new File(data, currentDBPath);
                File backupDB = new File(sd, databaseName + ".db");

                if (currentDB.exists()) {
                    FileChannel src = new FileInputStream(currentDB).getChannel();
                    FileChannel dst = new FileOutputStream(backupDB).getChannel();
                    dst.transferFrom(src, 0, src.size());
                    src.close();
                    dst.close();

                    Toast.makeText(context, "Export succeeded", Toast.LENGTH_SHORT).show();
                }
            }
        } catch (Exception e) {
            Toast.makeText(context, e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    public void onFragmentHidden() {
        Log.d(TAG, "onFragmentHidden");
        mInputMethodManager.hideSoftInputFromWindow(mFeedbackText.getWindowToken(), 0);
    }

    @Override
    public void onFragmentShown() {
        Log.d(TAG, "onFragmentShown");
        refreshAndSetFloatingButton();
    }

    private void refreshAndSetFloatingButton() {
        mFeedbackText.requestFocus();
        mInputMethodManager.showSoftInput(mFeedbackText, InputMethodManager.SHOW_IMPLICIT);
        Activity activity = getActivity();
        if(activity != null) {
            FloatingActionButton floatingActionButton = (FloatingActionButton) activity.findViewById(R.id.fab);
            if (floatingActionButton != null) {
                floatingActionButton.setVisibility(View.GONE);
                floatingActionButton.setOnClickListener(null);
            }
        }
    }

}
