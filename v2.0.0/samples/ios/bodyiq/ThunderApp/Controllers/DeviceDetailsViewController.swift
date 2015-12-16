//
//  DeviceDetailsViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 10/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class DeviceDetailsCell: UITableViewCell {
    @IBOutlet var titleLabel: UILabel!
    @IBOutlet var detailLabel: UILabel!
}

class DeviceDetailsViewController: UIViewController, UITableViewDataSource, UITableViewDelegate {

    // MARK: Members
    
    var data                = [String]()
    
    
    // MARK: UI
    @IBOutlet weak var table: UITableView!
    
    
    // MARK: Buttons
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        DLog("\(NSStringFromClass(self.classForCoder)) loaded")
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
        
        setupData()
        
        table.reloadData()
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
        
        setupNavBar()
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
    }
    
    
    // MARK: Back
    
    @objc private func back(sender: AnyObject) {
        self.navigationController?.popViewControllerAnimated(true)
    }
    
    
    // MARK: Setup
    
    private func setupNavBar() {
        
        self.navigationController?.setNavigationBarHidden(false, animated: true)
        self.navigationItem.customize("device_details".localized, target: self, action: Selector("back:"))
    }
    
    private func setupData() {
        
        data.append(GlobalStorage.deviceDescription?.displayName ?? "")
        data.append(GlobalStorage.deviceDescription?.uuid ?? "")
        data.append(GlobalStorage.deviceDescription?.manufacturerName ?? "")
        data.append(GlobalStorage.deviceDescription?.modelNumber ?? "")
        data.append(GlobalStorage.deviceDescription?.serialNumber ?? "")
        data.append(GlobalStorage.deviceDescription?.firmwareRevision ?? "")
        data.append(GlobalStorage.deviceDescription?.softwareRevision ?? "")
        data.append(GlobalStorage.deviceDescription?.hardwareRevision ?? "")
    }
    
    
    // MARK: UITableViewDataSource
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        
        return data.count
        
    }
    
    func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        
        return 1
        
    }
    
    func tableView(tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        
        return "device_info".localized;
        
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        
        let cell                = tableView.dequeueReusableCellWithIdentifier(Constants.DeviceDetailsCellId, forIndexPath: indexPath) as! DeviceDetailsCell
        
        // Disable the cell's selection property
        cell.selectionStyle     = UITableViewCellSelectionStyle.None
        
        configureDataCell(cell, forRow: indexPath.row)
        
        return cell
    }
    
    
    // MARK: UITableViewDelegate
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
    }
    
    func tableView(tableView: UITableView, estimatedHeightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        
        return 80.0
    }
    
    func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 70
    }
    
    func tableView(tableView: UITableView, willDisplayHeaderView view: UIView, forSection section: Int) {
        
        let header: UITableViewHeaderFooterView = view as! UITableViewHeaderFooterView
        
        // Background
        header.contentView.backgroundColor      = UIColor.whiteColor()
        
        // Text
        header.textLabel!.textColor             = UIColor.init(hex: Constants.HighlightActiveTabs)
        header.textLabel!.font                  = UIFont(name: "HelveticaNeue", size: 18)
    }

    
    // MARK: Data Cell
    
    private func configureDataCell(cell: DeviceDetailsCell, forRow row: Int) {
        switch row {
            case 0:
                cell.titleLabel.text = "Device Name"
                cell.detailLabel.text = data[0]
            case 1:
                cell.titleLabel.text = "Device Address"
                cell.detailLabel.text = data[1]
            case 2:
                cell.titleLabel.text = "Manufacturing Name"
                cell.detailLabel.text = data[2]
            case 3:
                cell.titleLabel.text = "Model Name"
                cell.detailLabel.text = data[3]
            case 4:
                cell.titleLabel.text = "Serial Number"
                cell.detailLabel.text = data[4]
            case 5:
                cell.titleLabel.text = "Firmware Revision"
                cell.detailLabel.text = data[5]
            case 6:
                cell.titleLabel.text = "Software Revision"
                cell.detailLabel.text = data[6]
            case 7:
                cell.titleLabel.text = "Hardware Revision"
                cell.detailLabel.text = data[7]
            default: break
        }
    }
}
