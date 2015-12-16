//
//  ThunderPicker.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/3/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

typealias PickerDataArray = [String]

class ThunderPicker: UIPickerView, UIPickerViewDataSource, UIPickerViewDelegate {

    
    // MARK: Members
    
    private var pickerData:PickerDataArray!
    private(set) var pickerDataValue:String?
    
    
    // MARK: Init
    
    init(frame: CGRect, pickerData: PickerDataArray) {
        
        super.init(frame: frame)
        
        self.pickerData = pickerData
    }

    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    
    // MARK: Data Sources
    
    func numberOfComponentsInPickerView(pickerView: UIPickerView) -> Int {
        return 1
    }
    func pickerView(pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        return pickerData.count
    }
    
    
    // MARK: Delegates
    
    func pickerView(pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String? {
        return pickerData[row]
    }
    
    func pickerView(pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int) {
        
        DLog("Selected \(pickerData[row])")
        
        pickerDataValue = pickerData[row]
    }
    
    func pickerView(pickerView: UIPickerView, attributedTitleForRow row: Int, forComponent component: Int) -> NSAttributedString? {
        
        let titleData   = pickerData[row]
        let myTitle     = NSAttributedString(string: titleData,
            attributes: [NSFontAttributeName:UIFont(name: "HelveticaNeue",
                size: 17.0)!,
                NSForegroundColorAttributeName:UIColor.blueColor()])
        return myTitle
    }
    
    func pickerView(pickerView: UIPickerView, viewForRow row: Int, forComponent component: Int, reusingView view: UIView?) -> UIView {
        
        var pickerLabel = view as! UILabel!
        
        if view == nil {
            
            pickerLabel                 = UILabel()
            pickerLabel.backgroundColor = UIColor(hex: Constants.ButtonsActionBarLinkColor)
        }
        
        let titleData   = pickerData[row]
        let myTitle     = NSAttributedString(string: titleData,
            attributes: [NSFontAttributeName:UIFont(name: "HelveticaNeue",
                size: 17.0)!,
                NSForegroundColorAttributeName:UIColor.whiteColor()])
        pickerLabel!.attributedText = myTitle
        
        pickerLabel!.textAlignment  = .Center
        
        return pickerLabel
    }
}
