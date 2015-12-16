//
//  ThunderDropdownMenu.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/2/15.
//  Copyright © 2015 Intel. All rights reserved.
//


import UIKit

public class ThunderDropdownMenu : UIView {
    
    
    // MARK: Members
    
    private var _configuration  : ThunderDropdownMenuConfiguration?
    private var _title : UILabel?
    private var _items: [String]?
    private var _titleText: String!
    private var _menuButton: UIButton!
    private var wrapper : UIView? //view in which you wish to present the dropdown
    private var _dropdownList : ThunderDropdownListTableView?
    private var _dropdownListWrapper: ThunderDropdownListWrapperView?
    private var _topBorder : UIView?
    private var _selectedItem: String?
    private var _dropdownIcon: UIImageView?
    
    public var onItemSelect: ((index: Int, item: AnyObject?) -> ())?
    public var onMenuButtonTapped: ((willOpen: Bool) -> ())?
    
    public var items : [String]? {
        get {
            return self._items
        } set (value) {
            self._items = value
            self.renderDropdownView()
        }
    }
    
    public var configuration : ThunderDropdownMenuConfiguration! {
        get {
            return self._configuration
        } set(value){
            self._configuration = value
            self.renderDropdownView()
        }
    }
    
    public var title: String? {
        get {
            return self._titleText
        } set(value) {
            self._titleText = value
            self.renderDropdownView()
        }
    }
    
    
    // MARK: Available initializers
    
    convenience public init(title: String, items: [String]) {
        self.init(title: title, items: items, frame: nil, configuration: nil)
    }
    
    convenience public init(title: String, items: [String], wrapper: UIView?) {
        self.init(title: title, items: items, frame: nil, configuration: nil)
        self.wrapper = wrapper
    }
    
    convenience public init(title: String, items: [String], frame: CGRect?) {
        self.init(title: title, items: items, frame: frame, configuration: nil)
    }
    
    convenience public init(title: String, items: [String], frame: CGRect?, wrapper: UIView?) {
        self.init(title: title, items: items, frame: frame, configuration: nil)
        self.wrapper = wrapper
    }
    
    convenience public init(title: String, items: [String], configuration: ThunderDropdownMenuConfiguration?) {
        self.init(title: title, items: items, frame: nil, configuration: configuration)
    }
    
    convenience public init(title: String, items: [String], configuration: ThunderDropdownMenuConfiguration?, wrapper: UIView?) {
        self.init(title: title, items: items, frame: nil, configuration: configuration)
        self.wrapper = wrapper
    }
    
    override public init(frame: CGRect){
        super.init(frame: frame)
    }
    
    init(title: String, items: [String], frame: CGRect?, configuration: ThunderDropdownMenuConfiguration?) {
        
        self._titleText = title
        self._items = items
        
        var _frame = CGRectMake(0, 0, 300, 100)
        
        if let frame = frame {
            _frame = frame
        }
        
        super.init(frame: _frame)
        
        if let configuration = configuration {
            self._configuration = configuration
        } else {
            self._configuration = ThunderDropdownMenuConfiguration.getDefaultConfiguration(self)
        }
        
        self.renderDropdownView()
        self.registerDropdown()
    }
    
    required public init?(coder aDecoder: NSCoder) {
        
        self._titleText = ""
        super.init(coder: aDecoder)
        self._configuration = ThunderDropdownMenuConfiguration.getDefaultConfiguration(self)
        self.renderDropdownView()
        self.registerDropdown()
        
    }
    
    private func registerDropdown(){
        
        if !ThunderDropdownMenuObserverList.instances.contains(self){
            ThunderDropdownMenuObserverList.instances.append(self)
        }
        
    }
    
    override public func setNeedsLayout() {
        
        super.setNeedsLayout()
        
        if let wrapper = self.wrapper {
            
            let positionInWindow = self.convertRect(self.bounds, toView: nil)
            var verticalOffset = positionInWindow.origin.y + self.frame.size.height
            
            if !UIApplication.sharedApplication().statusBarHidden {
                // Button - table separation
                verticalOffset += 0
            }
            
            self._dropdownListWrapper!.frame.origin.x = wrapper.frame.origin.x
            self._dropdownListWrapper!.frame.origin.y = verticalOffset
            self._dropdownListWrapper!.frame.size.width = wrapper.frame.size.width
            
            self._dropdownList!.frame.size.width = self._dropdownListWrapper!.frame.width
            self._topBorder?.frame.size.width = self._dropdownList!.frame.size.width
        }
        
    }

    
    // MARK: Renders the dropdown view
    
    public func renderDropdownView() {
        
        if let _title = self._title {
            if _title.isDescendantOfView(self){
                _title.removeFromSuperview()
            }
        }
        
        let buttonFrame = CGRectMake(0, 0, self.frame.width, self.frame.height)
        
        self._title = UILabel(frame: buttonFrame)
        self._title!.text = self._titleText
        self._title!.textAlignment = NSTextAlignment.Left 
        self._title!.font = self.configuration.titleFont
        self._title!.textColor = self.configuration.titleColor
        self._title?.backgroundColor = self.configuration.headerBackgroundColor
        
        if let headerViewHeight = self.configuration.headerViewHeight {
            self._title?.frame = CGRectMake(self._title!.frame.origin.x, self._title!.frame.origin.y, self._title!.frame.size.width, headerViewHeight)
        }
        
        self._title?.frame.size.width = (self._title!.text! as NSString).sizeWithAttributes([NSFontAttributeName:self._title!.font]).width
        
        self._menuButton = UIButton(frame: buttonFrame)
        
        self.addSubview(self._menuButton)
        self._menuButton.addSubview(self._title!)
        
        if let dropdownIconAsset = self.configuration.dropdownIconAssetName {
            let dropdownIcon = UIImage(named: dropdownIconAsset)
            
            if self._dropdownIcon == nil {
                
                self._dropdownIcon = UIImageView(frame: CGRectMake(self._title!.frame.origin.x + self._title!.frame.size.width, 0, 30, 30))
                self._dropdownIcon?.image = dropdownIcon
                
                self._menuButton.addSubview(self._dropdownIcon!)
                
            } else {
                
                self._dropdownIcon?.frame.origin.x = self._title!.frame.origin.x + self._title!.frame.size.width
                
            }
            
        }
        
        if self.items != nil {
            self.renderList()
        }
        
        self.setNeedsLayout()
        self._menuButton.addTarget(self, action: "menuButtonTapped:", forControlEvents: UIControlEvents.TouchUpInside)
        
    }
    
    func menuButtonTapped(sender: UIButton){
        
        for menu in ThunderDropdownMenuObserverList.instances {
            
            if menu != self {
                
                menu.hideList()
                
            }
            
        }
        
        if let handler = self.onMenuButtonTapped {
            
            if let dropdownList = self._dropdownList {
                
                handler(willOpen: dropdownList.hidden)
                
            }
            
        }
        
        self.toggleList()
        
    }
    
    private func renderList(){
        if self._dropdownList == nil && self.items != nil {
            if self.wrapper == nil {
                var wrapper = self.superview
                
                while wrapper != nil {
                    
                    self.wrapper = wrapper
                    wrapper = wrapper?.superview
                    
                }
            }
            
            if let wrapper = self.wrapper {
                
                let positionInWindow = self.convertRect(self.bounds, toView: nil)
                var verticalOffset = positionInWindow.origin.y + self.frame.size.height
                
                if !UIApplication.sharedApplication().statusBarHidden {
                    
                    verticalOffset += 20
                }
                
                let wrapperFrame = CGRectMake(wrapper.frame.origin.x, verticalOffset, wrapper.frame.size.width, wrapper.frame.size.height)
                self._dropdownListWrapper = ThunderDropdownListWrapperView(frame: wrapperFrame)
                self._dropdownListWrapper?.clipsToBounds = true
                self._dropdownListWrapper?.context = self
                
                wrapper.addSubview(self._dropdownListWrapper!)
                
                let tableFrame = CGRectMake(wrapper.frame.origin.x, verticalOffset, wrapper.frame.size.width, 0)
                
                self._dropdownList = ThunderDropdownListTableView(frame: tableFrame, style: UITableViewStyle.Plain, items: self.items!, context: self)
                
                self._dropdownList?.itemSelectHandler = { (index: Int, item: AnyObject?) -> () in
                    
                    if let handler = self.onItemSelect {
                        
                        handler(index: index, item: item)
                        
                    }
                    
                }
                
                if self.items != nil && self.items!.count > 0 && self.items!.count > self.configuration.dropdownListSelectedItemIndex && self.configuration.dropdownListSelectedItemIndex >= 0 {
                    
                    if let title = self.title {
                        
                        if self.items!.contains(title) {
                            
                            if let selectedIndex = self.items!.indexOf(title) {
                                self.configuration.dropdownListSelectedItemIndex = selectedIndex
                            }
                            
                        }
                        
                    }
                    
                    self._dropdownList?.selectRowAtIndexPath(NSIndexPath(forRow: self.configuration.dropdownListSelectedItemIndex, inSection: 0), animated: false, scrollPosition: UITableViewScrollPosition.Top)
                    
                    self.title = self.items![self.configuration.dropdownListSelectedItemIndex]
                    
                }
                
                self._dropdownList!.hidden = true
                
                self._dropdownList?.separatorInset = UIEdgeInsetsZero
                self._dropdownList?.layoutMargins = UIEdgeInsetsZero
                self._dropdownList?.separatorStyle = UITableViewCellSeparatorStyle.None
                
                self._dropdownListWrapper?.addSubview(self._dropdownList!)
                self._topBorder = UIView(frame: CGRectMake(0, 0, self._dropdownListWrapper!.frame.width, 0.5))
                self._topBorder?.hidden = true
                self._dropdownListWrapper?.addSubview(self._topBorder!)
                
            }
        }
        
        if let configuration = self.configuration, dropdownList = self._dropdownList {
            dropdownList.backgroundColor = configuration.dropdownListBackgroundColor
            dropdownList.layer.borderWidth = configuration.dropdownListBorderWidth
            dropdownList.layer.borderColor = configuration.dropdownListBorderColor.CGColor
            self._topBorder?.backgroundColor = configuration.dropdownListBorderColor
        }
        
    }
    
    func showList(){
        
        if !self._dropdownList!.isAnimating {
            
            self._dropdownList!.frame = CGRectMake(0, -self._dropdownList!.frame.height, self._dropdownList!.frame.width, self._dropdownList!.frame.height)
            self._dropdownList?.hidden = false
            
            if self._dropdownList!.frame.height > self.configuration.dropdownListMaxHeight {
                
                self._dropdownList!.frame.size.height = self.configuration.dropdownListMaxHeight
                
            }
            
            if let configuration = self.configuration {
                
                self._dropdownList?.isAnimating = true
                
                self._topBorder?.hidden = false
                
                UIView.animateWithDuration(configuration.animationDuration, delay: 0, options: UIViewAnimationOptions.CurveEaseOut, animations: { () -> Void in
                    
                    if let dropdownIcon = self._dropdownIcon {
                        if configuration.dropdownIconWillRotate {
                            
                            dropdownIcon.transform = CGAffineTransformMakeRotation(CGFloat(M_PI))
                            
                        }
                    }
                    
                    self._dropdownList?.frame = CGRectMake(0, 30, self._dropdownList!.frame.width, self._dropdownList!.frame.height)
                    }, completion: { (completed) -> Void in
                        
                        UIView.animateWithDuration(configuration.animationDuration, delay: 0, options: UIViewAnimationOptions.CurveEaseOut, animations: { () -> Void in
                            self._dropdownList?.frame = CGRectMake(0, 0, self._dropdownList!.frame.width, self._dropdownList!.frame.height)
                            }, completion: { (completed) -> Void in
                                self._dropdownList?.isAnimating = false
                        })
                })
                
            }
            
        }
        
    }
    
    func hideList(){
        
        if !self._dropdownList!.isAnimating {
            
            if let configuration = self.configuration {
                
                self._dropdownList?.isAnimating = true
                
                UIView.animateWithDuration(configuration.animationDuration, delay: 0, options: UIViewAnimationOptions.CurveEaseOut, animations: { () -> Void in
                    
                    if let dropdownIcon = self._dropdownIcon {
                        if configuration.dropdownIconWillRotate {
                            
                            dropdownIcon.transform = CGAffineTransformMakeRotation(CGFloat(0))
                            
                        }
                    }
                    
                    self._dropdownList?.frame = CGRectMake(0, -self._dropdownList!.frame.height, self._dropdownList!.frame.width, self._dropdownList!.frame.height)
                    }, completion: { (completed) -> Void in
                        if completed {
                            
                            self._dropdownList?.isAnimating = false
                            self._dropdownList?.hidden = true
                            self._topBorder?.hidden = true
                            
                        }
                })
                
            }
            
        }
        
    }
    
    private func toggleList(){
        
        if let dropdownList = self._dropdownList {
            
            if dropdownList.hidden {
                self.showList()
            } else {
                self.hideList()
            }
            
        } else {
            
            self.renderList()
            self.showList()
        }
    }
    
    public func closeList() {
        
        if let dropdownList = self._dropdownList {
            
            guard dropdownList.hidden else {
                self.hideList()
                return
            }
        }
    }
}

// MARK: Dropdown menu list items

class ThunderDropdownMenuObserverList {
    
    static var instances = [ThunderDropdownMenu]()
    
}