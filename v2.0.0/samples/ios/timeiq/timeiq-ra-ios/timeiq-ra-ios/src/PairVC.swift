//
//  PairVC.swift
//  SensorRefApp2
//
//  Created by Gal Shirin on 22/11/2015.
//  Copyright © 2015 fiat.bender. All rights reserved.
//

import UIKit
import IQCore

class PairVC: UIViewController, UITableViewDelegate, UITableViewDataSource {
    
    var token: WearableToken!
    private var deviceConnectionManager = DeviceConnectionManager.sharedInstance
    var spinner:UIActivityIndicatorView!

    @IBOutlet weak var connectBtn: UIButton!
    @IBOutlet weak var disconnectBtn: UIButton!
    @IBOutlet weak var tableView: UITableView!
    override func viewDidLoad() {
        super.viewDidLoad()
        
        spinner = UIActivityIndicatorView(activityIndicatorStyle: .Gray)
        spinner.center = self.view.center
        spinner.transform = CGAffineTransformMakeScale(2.25, 2.25);
        self.view.addSubview(spinner)
        
        if (token != nil) {
            let title = "Pairing to \(token!.displayName!)"
            self.navigationItem.title = title
        }
        tableView.delegate = self
        tableView.dataSource = self
        tableView.userInteractionEnabled = false

        // Do any additional setup after loading the view.
    }
    
    override func viewDidAppear(animated: Bool) {
        if deviceConnectionManager.isConnected() {
            connectBtn.enabled = false
            disconnectBtn.enabled = false
            if token == deviceConnectionManager.getToken() {
                disconnectBtn.enabled = true
            }
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func setSelectedToken(selectedToken: WearableToken) {
        token = selectedToken
    }
    
    func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if (token != nil) {
            return 1
        }
        return 0
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("devCell", forIndexPath: indexPath) as! deviceCell
        
        cell.name.text = token.displayName
        cell.address.text = token.id
        
        return cell
    }
    @IBAction func unpair(sender: AnyObject) {
        deviceConnectionManager.disconnect()
        self.navigationController?.popViewControllerAnimated(true)
    }
    @IBAction func connectBtnPress(sender: AnyObject) {
        spinner.startAnimating()
        deviceConnectionManager.connectToController({ (e) -> ()? in
            dispatch_async(dispatch_get_main_queue(), { () -> Void in
                self.spinner.stopAnimating()
                self.showAlertView("Error connecting to \(self.token!.displayName!)", strBody: e.localizedDescription)
            })
            }) { () -> ()? in
                dispatch_async(dispatch_get_main_queue(), { () -> Void in
                    self.spinner.stopAnimating()
                    self.navigationController?.popViewControllerAnimated(true)
                })
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

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
