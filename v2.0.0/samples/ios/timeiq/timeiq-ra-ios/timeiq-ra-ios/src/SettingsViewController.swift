//
//  SettingsViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 15/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TSO

class SettingsViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {

    var dataSource = ["Calendar", "Pairing"]
    var settingsTableView: UITableView?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        dataSource.append("Version " + SystemConfig_VERSION_) //+ " (" + BuildConfig_BUILD_DATE_SHORT_ + ")")
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("SettingsTableViewCell", forIndexPath: indexPath)
        
        cell.textLabel?.text = NSLocalizedString(dataSource[indexPath.row], comment: "");
        
        return cell;
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return dataSource.count;
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        switch (indexPath.row)
        {
        case 0:
            let testappVC = UIStoryboard.init(name: "calendar", bundle: nil).instantiateViewControllerWithIdentifier("CalendarSelectVC");
            self.navigationController?.pushViewController(testappVC, animated: true)
            break;
        case 1:
            let deviceVC = UIStoryboard.init(name: "device", bundle: nil).instantiateViewControllerWithIdentifier("ConnectToDeviceVC");
            self.navigationController?.pushViewController(deviceVC, animated: true)
            break;
        default:
            break;
        }
    }
}
