//
//  ConnectToDeviceVC.swift
//  SensorRefApp2
//
//  Created by Gal Shirin on 16/11/2015.
//  Copyright © 2015 fiat.bender. All rights reserved.
//

import UIKit
import IQCore

class ConnectToDeviceVC: UITableViewController {
    
    var foundTokens = [WearableToken]()
    var spinner:UIActivityIndicatorView!
    
    private var deviceConnectionManager = DeviceConnectionManager.sharedInstance
    private static var updateTable = false
    private static var runOnce = true
    
    @IBOutlet weak var scanBtn: UIBarButtonItem!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        spinner = UIActivityIndicatorView(activityIndicatorStyle: .Gray)
        spinner.center = self.view.center
        spinner.transform = CGAffineTransformMakeScale(2.25, 2.25);
        self.view.addSubview(spinner)
        
        addConnected()
        
        if ConnectToDeviceVC.runOnce {
            deviceConnectionManager.registerForConnectionUpdates(ConnectToDeviceVC.connectionUpdate)
            ConnectToDeviceVC.runOnce = false
        }
        
        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem()
    }
    
    private static func connectionUpdate(connected:Bool) {
        ConnectToDeviceVC.updateTable = true
    }
    
    override func viewDidAppear(animated: Bool) {
        if ConnectToDeviceVC.updateTable {
            self.tableView.reloadData()
            ConnectToDeviceVC.updateTable = false
        }
    }
    
    func addConnected() {
        if deviceConnectionManager.isConnected() {
            self.foundTokens.append(self.deviceConnectionManager.getToken())
        }
    }
    
    func startScan() {
        self.foundTokens.removeAll()
        addConnected()
        self.tableView.reloadData()
        spinner.startAnimating()
        scanBtn.title = "Stop"
        deviceConnectionManager.scan({ (newToken) -> () in
            self.foundTokens.append(newToken)
            self.tableView.reloadData()
            }, err: { (e) -> ()? in
                self.stopAnimating()
            }) { () -> ()? in
                self.stopAnimating()
        }
    }
    
    func stopAnimating() {
        self.spinner.stopAnimating()
        self.scanBtn.title = "Scan"
    }
    
    func stopScan() {
        deviceConnectionManager.stopScan()
        stopAnimating()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // MARK: - Table view data source

    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return self.foundTokens.count
    }

    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("connectCell", forIndexPath: indexPath) as! deviceCell

        cell.name.text = self.foundTokens[indexPath.row].displayName
        cell.address.text = self.foundTokens[indexPath.row].id
        cell.name.textColor = UIColor.blackColor()
        cell.address.textColor = UIColor.lightGrayColor()
        
        if (deviceConnectionManager.isConnected() && deviceConnectionManager.getToken() == self.foundTokens[indexPath.row]) {
            cell.name.textColor = UIColor.blueColor()
            cell.address.textColor = UIColor(red: 0, green: 0, blue: 1, alpha: 0.3)
        }

        return cell
    }
    @IBAction func scanBtn(sender: AnyObject) {
        if deviceConnectionManager.isScanning() {
            stopScan()
        } else {
            startScan()
        }
    }
    func showAlertView(strTitle:String, strBody:String)
    {
        let alert = UIAlertView()
        alert.title = strTitle
        alert.message = strBody
        alert.addButtonWithTitle("Dismiss")
        alert.show()
    }
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if let identifier = segue.identifier where identifier == "TokenSelected",
            let controller = segue.destinationViewController as? PairVC {
            let indexSelected = self.tableView.indexPathForSelectedRow
            let token = self.foundTokens[indexSelected!.row]
                if !deviceConnectionManager.isConnected() {
                    deviceConnectionManager.pair(token)
                }
            stopScan()
            self.tableView.deselectRowAtIndexPath(indexSelected!, animated: true)
            controller.setSelectedToken(token)
        }
    }
    /*
    // Override to support conditional editing of the table view.
    override func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return false if you do not want the specified item to be editable.
        return true
    }
    */

    /*
    // Override to support editing the table view.
    override func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
        if editingStyle == .Delete {
            // Delete the row from the data source
            tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: .Fade)
        } else if editingStyle == .Insert {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }    
    }
    */

    /*
    // Override to support rearranging the table view.
    override func tableView(tableView: UITableView, moveRowAtIndexPath fromIndexPath: NSIndexPath, toIndexPath: NSIndexPath) {

    }
    */

    /*
    // Override to support conditional rearranging of the table view.
    override func tableView(tableView: UITableView, canMoveRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return false if you do not want the item to be re-orderable.
        return true
    }
    */

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
class deviceCell: UITableViewCell {
    @IBOutlet weak var name: UILabel!
    @IBOutlet weak var address: UILabel!
}
