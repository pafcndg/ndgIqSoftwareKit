//
//  ActivityViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/6/15.
//  Copyright © 2015 Intel. All rights reserved.
//


import UIKit

class LeftMenuCell:UITableViewCell {
    
    @IBOutlet weak var radioBtn:UIButton!
    @IBOutlet weak var deviceLb:UILabel!
    @IBOutlet weak var connectedState:UIImageView!
    @IBOutlet weak var batteryState:UIImageView!
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
    
    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)
    }
}

class LeftMenuViewController : UITableViewController {
    
    
    // MARK: Members
    
    let menuOptions = ["Thunder 7"]
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        DLog("\(NSStringFromClass(self.classForCoder)) loaded")        
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)        
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
    }
    
    // MARK: Setup
    
    private func setupNavBar() {
        
        self.navigationController?.setNavigationBarHidden(false, animated: true)
    }
    
    
    // MARK: UITableViewDelegate methods
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        tableView.deselectRowAtIndexPath(indexPath, animated: true)
        
        switch indexPath.row {
            case 1:
                // ContainerVC.swift listens for this
                NSNotificationCenter.defaultCenter().postNotificationName(Constants.OpenModalWindow, object: nil)
            case 0:
                // Both SessionsViewController and ActivityViewController listen for this
                NSNotificationCenter.defaultCenter().postNotificationName(Constants.OpenPushWindow, object: nil)
            default:
                print("indexPath.row:: \(indexPath.row)")
        }
        
        // also close the menu
        NSNotificationCenter.defaultCenter().postNotificationName(Constants.CloseMenu, object: nil)
    }
    
    override func tableView(tableView: UITableView, estimatedHeightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        
        return 44.0
    }
    
    
    // MARK: UITableViewDataSource methods
    
    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        
        tableView.separatorColor = UIColor .clearColor()
        return menuOptions.count
    }
    
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        
        let cell                = tableView.dequeueReusableCellWithIdentifier(Constants.MenuCellId) as! LeftMenuCell
        
        // Disable the cell's selection property
        cell.selectionStyle     = UITableViewCellSelectionStyle.None
        
        // Fill cell
        cell.deviceLb.text      = menuOptions[indexPath.row]
        
        return cell;
    }
    
    override func tableView(tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        
        return "my_devices".localized;
    }
    
    override func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 70
    }
    
    override func tableView(tableView: UITableView, willDisplayHeaderView view: UIView, forSection section: Int) {
        
        let header: UITableViewHeaderFooterView = view as! UITableViewHeaderFooterView
        
        // Background
        header.contentView.backgroundColor      = UIColor.whiteColor()
        
        // Text
        header.textLabel!.textColor             = UIColor.init(hex: 0x747474)
        header.textLabel!.font                  = UIFont(name: "HelveticaNeueLight ", size: 17)
    }
}