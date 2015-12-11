package com.intel.wearable.platform.timeiq.refapp.reminders.add_edit;

/**
 * Created by adura on 26/10/2015.
 */

/**
 * Interface for all reminder fragment
 */
/*package*/ interface IReminderFragment {

    /**
     * Validate if all data to create reminder are in place
     * @return Error string if failed, else an empty string
     */
    String isOkToCreateReminder();


    /**
     * In order to update the "Create reminder icon" at the main UI each fragment must
     *  register a listener for its data change
     * @param dataChangeListener
     */
    void setDataChangeListener(IFragmentFormDataChangedListener dataChangeListener);

}
