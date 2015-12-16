//
//  ThunderDropdownMenuConfiguration.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/2/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

// Defines the looks and feels of the dropdown
public class ThunderDropdownMenuConfiguration {
    
    private var context : ThunderDropdownMenu?
    
    // Attributes for the header of the dropdown
    private var _headerBackgroundColor: UIColor!
    private var _headerViewHeight: CGFloat?
    
    // Attributes for the title (selected item)
    private var _titleColor : UIColor!
    private var _titleFont: UIFont!
    
    // Attributes for the dropdown list items
    private var _cellHeight : CGFloat!
    private var _dropdownListBackgroundColor: UIColor!
    private var _cellFont: UIFont!
    private var _cellTextColor : UIColor!
    private var _dropdownListBorderColor: UIColor!
    private var _dropdownListBorderWidth: CGFloat!
    private var _dropdownListMaxHeight: CGFloat!
    private var _dropdownListSelectedItemIndex: Int!
    private var _dropdownListSelectedItemBackgroundColor: UIColor!
    private var _dropdownListSelectedItemAccessoryType : UITableViewCellAccessoryType!
    
    // Animations
    private var _animationDuration : NSTimeInterval!
    
    // Dropdown icon
    private var _dropdownIconAssetName: String?
    private var _dropdownIconWillRotate: Bool!
    
    // Returns a prepopulated configuration set
    static public func getDefaultConfiguration(context: ThunderDropdownMenu) -> ThunderDropdownMenuConfiguration{
        
        let configuration = ThunderDropdownMenuConfiguration(context: context)
        
        configuration.headerBackgroundColor = UIColor.clearColor()
        
        configuration.titleColor = UIColor.blackColor()
        configuration.titleFont = UIFont(name: "HelveticaNeue", size: 17)!
        
        configuration.cellHeight = 40
        configuration.animationDuration = 0.3
        
        configuration.cellFont = UIFont(name: "HelveticaNeue", size: 17)!
        configuration.cellTextColor = UIColor.blackColor()
        
        configuration.dropdownListBackgroundColor = UIColor.whiteColor()
        configuration.dropdownListBorderColor = UIColor.lightGrayColor()
        configuration.dropdownListBorderWidth = 0.5
        
        configuration.dropdownListMaxHeight = 200
        configuration.dropdownListSelectedItemIndex = 0
        
        configuration.dropdownListSelectedItemBackgroundColor = UIColor(red: 211/255, green: 234/255, blue: 242/255, alpha: 1.0)
        configuration.dropdownListSelectedItemAccessoryType = UITableViewCellAccessoryType.Checkmark
        
        configuration.dropdownIconAssetName = "arrow"
        configuration.dropdownIconWillRotate = true
        
        
        return configuration
        
    }
    
    init(context: ThunderDropdownMenu) {
        self.context = context
    }
    
}


// MARK: Access methods

extension ThunderDropdownMenuConfiguration {
    
    // Attributes for the header of the dropdown
    public var headerBackgroundColor: UIColor {
        get {
            return self._headerBackgroundColor
        } set(value){
            self._headerBackgroundColor = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var headerViewHeight: CGFloat? {
        get {
            return self._headerViewHeight
        } set(value){
            self._headerViewHeight = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    
    // Attributes for the title (selected item)
    public var titleColor : UIColor {
        get {
            return self._titleColor
        } set(value) {
            self._titleColor = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var titleFont: UIFont {
        get {
            return self._titleFont
        } set(value){
            self._titleFont = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    
    // Attributes for the dropdown list items
    public var cellHeight : CGFloat {
        get {
            return self._cellHeight
        } set(value){
            self._cellHeight = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var animationDuration: NSTimeInterval {
        get {
            return self._animationDuration
        } set(value){
            self._animationDuration = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var dropdownListBackgroundColor: UIColor {
        get {
            return self._dropdownListBackgroundColor
        } set(value){
            self._dropdownListBackgroundColor = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var cellFont: UIFont {
        get {
            return self._cellFont
        } set(value){
            self._cellFont = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var cellTextColor: UIColor {
        get {
            return self._cellTextColor
        } set(value){
            self._cellTextColor = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var dropdownListBorderColor: UIColor {
        get {
            return self._dropdownListBorderColor
        } set(value){
            self._dropdownListBorderColor = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var dropdownListBorderWidth: CGFloat {
        get {
            return self._dropdownListBorderWidth
        } set(value){
            self._dropdownListBorderWidth = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var dropdownListMaxHeight: CGFloat {
        get {
            return self._dropdownListMaxHeight
        } set(value){
            self._dropdownListMaxHeight = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var dropdownListSelectedItemIndex: Int {
        get {
            return self._dropdownListSelectedItemIndex
        } set(value){
            self._dropdownListSelectedItemIndex = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var dropdownListSelectedItemBackgroundColor: UIColor {
        get {
            return self._dropdownListSelectedItemBackgroundColor
        } set(value){
            self._dropdownListSelectedItemBackgroundColor = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    public var dropdownListSelectedItemAccessoryType: UITableViewCellAccessoryType {
        get {
            return self._dropdownListSelectedItemAccessoryType
        } set(value){
            self._dropdownListSelectedItemAccessoryType = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    
    // Dropdown icon
    public var dropdownIconAssetName: String? {
        get {
            return self._dropdownIconAssetName
        } set(value){
            self._dropdownIconAssetName = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    
    public var dropdownIconWillRotate: Bool {
        get {
            return self._dropdownIconWillRotate
        } set(value){
            self._dropdownIconWillRotate = value
            if let context = self.context, _ = context.configuration {
                context.renderDropdownView()
            }
        }
    }
    
}