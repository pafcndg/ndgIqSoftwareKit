//
//  ActivityViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/6/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import BodyIQ

class ActivityCell:UITableViewCell {
    
    @IBOutlet weak var activityIcon:UIImageView!
    @IBOutlet weak var activityValue:UILabel!
    @IBOutlet weak var activityProperty:UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
    
    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        self.backgroundColor = UIColor.clearColor()
    }
}

class ActivityViewController: TabVCTemplate, UITableViewDelegate, UITableViewDataSource {

    
    // MARK: Members
    let menuOptions = ["Biking", "Biking", "Biking", "Biking"]
    var ts:TimeSeriesManager?
    var activity:Activity?
    
    
    // MARK: UI
    @IBOutlet weak var table:UITableView!
    @IBOutlet weak var leftGraph:ThunderCircularProgress!
    @IBOutlet weak var rightGraph:ThunderCircularProgress!
    @IBOutlet weak var leftLb:UILabel!
    @IBOutlet weak var rightLb:UILabel!
    @IBOutlet weak var date:UILabel!
    
    
    // MARK: ViewController lifecycle
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        
        table.tableFooterView = nil
        table.tableHeaderView = nil
        
        selectedTab = 0
    }
    
    override func viewWillAppear(animated: Bool) {
        
        super.viewWillAppear(animated)
    }
    
    override func viewDidAppear(animated: Bool) {
        
        super.viewDidAppear(animated)
                
        setupNavBar()
        
        date.text = NSDate.currentDate("EEEE, MMMM dd")
        
        // Start observing
        NSNotificationCenter.defaultCenter().addObserver(self, selector: Selector("setConnectionState:"), name: Constants.ConnectionStateNotification, object: nil)
        
        setConnectionStatusIndicator()
        
        queryTimeSeries()
    }
    
    override func viewWillDisappear(animated: Bool) {
        
        super.viewWillDisappear(animated)
    }
    
    override func viewDidDisappear(animated: Bool) {
        
        super.viewDidDisappear(animated)
        
        // Stop observing
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }
    
    
    // MARK: Buttons
    
    @IBAction func toggleMenu(sender: AnyObject) {
        NSNotificationCenter.defaultCenter().postNotificationName(Constants.ToggleMenu, object: nil)
    }
    
    
    // MARK: UITableViewDelegate methods
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        tableView.deselectRowAtIndexPath(indexPath, animated: true)
        
    }
    
    func tableView(tableView: UITableView, estimatedHeightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        
        return 44.0
    }
    
    
    // MARK: Time Series
    
    func queryTimeSeries() {
        
        print("Query TS")
        
        guard let bodyStatistics = BodyIQ.bodyStatisticsForUserIdentifier("user-id") else {
            fatalError("bodyStatistics not stored")
        }
        
        if let error = BodyIQ.configureWithUserIdentifier("user-id", bodyStats: bodyStatistics) {
            fatalError("Error generating user: \(error)")
        }
        
        let now         = NSDate.nowUTC()
        
        let midNight    = NSDate.dateSinceMidnight()
        
        ts = TimeSeriesManager(startDate: midNight!, endDate: now!)
        
        ts?.timeSeriesQuery({ data in
        
            self.activity = self.ts?.extractTimeSeriesData(data)
            
            // TODO: review this
            self.setupGraphs((self.activity?.steps)!, calories: Int((self.activity?.calories)!))
            
            self.table.reloadData()
        })
    }
    
    
    // MARK: Setup
    
    func setupNavBar() {
        
        let image       = UIImage(named: "green_indicator")
        let frame       = CGRectMake(0, 0, 25, 25)
        let button      = UIButton()
        button.frame    = frame
        button.setImage(image, forState: .Normal)
        let rightMenuButton = UIBarButtonItem(customView: button)
        self.navigationItem.rightBarButtonItem = rightMenuButton
    }
    
    func setupActivityLabels(steps: Int, calories: Int) {
        
        // Center
        let paragraphStyle                  = NSMutableParagraphStyle()
        paragraphStyle.alignment            = .Center
        paragraphStyle.firstLineHeadIndent  = 0.001
        
        
        let attStringLeft = "<b><font face=\"HelveticaNeue\" size=\"6\" color=\"white\">\(steps)</font></b></br><font face=\"HelveticaNeue\" size=\"2\" color=\"white\">STEPS</font>".html2AttStr
        
        let mutableAttrStrLeft = NSMutableAttributedString(attributedString: attStringLeft)
        mutableAttrStrLeft.addAttribute(NSParagraphStyleAttributeName, value: paragraphStyle, range: NSMakeRange(0, mutableAttrStrLeft.length))
        
        leftLb.attributedText = mutableAttrStrLeft
        
        
        let attStringRight = "<b><font face=\"HelveticaNeue\" size=\"6\" color=\"white\">\(calories)</font></b></br><font face=\"HelveticaNeue\" size=\"2\" color=\"white\">CALORIES</font>".html2AttStr
        
        let mutableAttrStrRight = NSMutableAttributedString(attributedString: attStringRight)
        mutableAttrStrRight.addAttribute(NSParagraphStyleAttributeName, value: paragraphStyle, range: NSMakeRange(0, mutableAttrStrRight.length))
        
        rightLb.attributedText = mutableAttrStrRight
    }
    
    func setupGraphs(steps: Int, calories: Int) {
        
        leftGraph.animateFromAngle(0, toAngle: 360*steps/4000, duration: 1, completion: { b in })
        rightGraph.animateFromAngle(0, toAngle: 360*calories/1600, duration: 1, completion: { b in })
        
        setupActivityLabels(steps, calories: calories)
    }
    
    
    // MARK: UITableViewDataSource methods
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        
        tableView.separatorColor = UIColor .clearColor()
        return menuOptions.count
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        
        let cell                = tableView.dequeueReusableCellWithIdentifier(Constants.ActivityCellId) as! ActivityCell
        
        // Add bottom border to cells
        cell.addBorder(edges: [.Bottom], colour: UIColor.lightGrayColor(), thickness: 0.5)
        
        // Disable the cell's selection property
        cell.selectionStyle     = UITableViewCellSelectionStyle.None
        
        cell.backgroundColor    = UIColor.clearColor()
        
        // Fill cell
        if activity != nil {
            configureDataCell(cell, data: activity!, forRow: indexPath.row)
        }
        
        return cell;
    }
    
    func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 0
    }
    
    
    // MARK: Data Cell
    
    private func configureDataCell(cell: ActivityCell, data: Activity, forRow row: Int) {
        switch row {
            case 0:
                cell.activityProperty.text      = "Distance"
                cell.activityValue.text         = data.distance
                cell.activityIcon.image         = UIImage(named: "activity_icon_distance")
            case 1:
                cell.activityProperty.text      = "Duration"
                cell.activityValue.text         = data.duration
                cell.activityIcon.image         = UIImage(named: "activity_icon_time")
            case 2:
                cell.activityProperty.text      = "Average Speed"
                cell.activityValue.text         = data.avgSpeed
                cell.activityIcon.image         = UIImage(named: "average_speed")
            case 3:
                cell.activityProperty.text      = "Longest Inactivity"
                cell.activityValue.text         = data.longestInactivity
                cell.activityIcon.image         = UIImage(named: "activity_icon_max")
            default: break
        }
    }
}

