//
//  ThunderDropdownListTableView.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/2/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class ThunderDropdownListTableView : UITableView, UITableViewDelegate, UITableViewDataSource {
    
    var context: ThunderDropdownMenu?
    var items: [String]?
    
    var isAnimating = false
    
    var itemSelectHandler: ((index: Int, item: AnyObject?) -> ())?
    
    convenience init(var frame: CGRect, style: UITableViewStyle, items: [String], context: ThunderDropdownMenu) {
        
        var frameHeight = frame.size.height
        var rowHeight : CGFloat = 0
        
        if let configuration = context.configuration {
            
            let cellHeight = configuration.cellHeight
            
            rowHeight = cellHeight
            
            for _ in items {
                frameHeight += cellHeight
            }
            
        }
        
        frame = CGRectMake(frame.origin.x, frame.origin.y, frame.size.width, frameHeight)
        
        self.init(frame: frame, style: style)
        
        self.items = items
        self.context = context
        self.rowHeight = rowHeight
        
        self.delegate = self
        self.dataSource = self
        
        self.scrollEnabled = true
        
    }
    
    override init(frame: CGRect, style: UITableViewStyle) {
        super.init(frame: frame, style: style)
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    func tableView(tableView: UITableView, heightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        
        if let context = self.context {
            if let configuration = context.configuration {
                
                return configuration.cellHeight
                
            }
        }
        
        return UITableViewAutomaticDimension
        
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        let selectedItem = self.items![indexPath.row]
        self.context?.title = selectedItem
        self.context?.hideList()
        
        // Upon selection add the according selection style
        tableView.cellForRowAtIndexPath(indexPath)?.backgroundColor = self.context?.configuration.dropdownListSelectedItemBackgroundColor
        tableView.cellForRowAtIndexPath(indexPath)?.accessoryType = self.context!.configuration.dropdownListSelectedItemAccessoryType
        
        if let handler = self.itemSelectHandler {
            
            handler(index: indexPath.row, item: selectedItem)
            
        }
        
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return (self.items?.count)!
    }
    
    
    // If the next row to be selected is not the same as the present one, remove any selection indicators
    func tableView(tableView: UITableView, willSelectRowAtIndexPath indexPath: NSIndexPath) -> NSIndexPath? {
        
        let currentIndexPath = tableView.indexPathForSelectedRow
        
        if currentIndexPath != nil {
            
            tableView.cellForRowAtIndexPath(currentIndexPath!)?.backgroundColor = self.context?.configuration.dropdownListBackgroundColor
            tableView.cellForRowAtIndexPath(currentIndexPath!)?.accessoryType = UITableViewCellAccessoryType.None
            
        }
        
        return indexPath
        
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = ThunderDropdownListTableViewCell(style: UITableViewCellStyle.Default, reuseIdentifier: "Cell")
        
        cell.textLabel?.text = self.items![indexPath.row]
        
        if let context = self.context {
            if let configuration = context.configuration {
                
                cell.textLabel?.textColor = configuration.cellTextColor
                cell.textLabel?.font = configuration.cellFont
                
            }
        }
        
        return cell
    }
    
    // On first cell rendering, if the cell is marked as selected proceed with adding the according style
    func tableView(tableView: UITableView, willDisplayCell cell: UITableViewCell, forRowAtIndexPath indexPath: NSIndexPath) {
        
        cell.selectionStyle = UITableViewCellSelectionStyle.None
        cell.layoutMargins = UIEdgeInsetsZero
        
        if cell.selected {
            
            cell.backgroundColor = self.context?.configuration.dropdownListSelectedItemBackgroundColor
            cell.accessoryType = self.context!.configuration.dropdownListSelectedItemAccessoryType
            
        }
        
    }
    
}
