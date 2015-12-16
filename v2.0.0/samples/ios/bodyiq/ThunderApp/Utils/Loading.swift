//
//  Loading.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/9/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

public typealias LoadingCompletionBlock = () -> Void

/// Loading Util class
public class Loading
{
    static let sharedInstance = Loading()
    
    
    // MARK: Members
    
    private var hud:MBProgressHUD?
        
    
    /**
     Loading Modes
     
     - Indeterminate: Progress showing using an UIActivityIndicatorView
     - PieChart:      Progress shown using a pie chart
     - HorizotalBar:  Progress shown using a horizontal progress bar
     - Ring:          Progress shown using a ring progress bar
     */
    public enum LoadingMode: Int {
        case Indeterminate
        case PieChart
        case HorizotalBar
        case Ring
    }
    
    
    // MARK: Init
    
    // Prevent from using the default class initializer
    private init() {}

    
    // MARK: Add / Remove Loading
    
    /**
     Add loading to your view
     
     - throws: Loading already added to view
    
     - parameter view:  The view that the loading will be added to
     - parameter mode:  LoadingMode
     - parameter label: Message to be displayed below the activity indicator
     */
    public func addLoading(view: UIView, mode: LoadingMode, label: String) throws{
        
        guard hud == nil else { throw LoadingError.LoadingInstanceAlreadyAdded }
        
        dispatch_async(dispatch_get_main_queue()) { [weak self] in
        //NSThread.performBlockOnMainThread({ () -> Void in
            
            var hudMode:MBProgressHUDMode
            
            switch mode {
            case LoadingMode.Indeterminate:
                hudMode = MBProgressHUDMode.Indeterminate
                break
            case LoadingMode.PieChart:
                hudMode = MBProgressHUDMode.Determinate
                break
            case LoadingMode.HorizotalBar:
                hudMode = MBProgressHUDMode.DeterminateHorizontalBar
                break
            case LoadingMode.Ring:
                hudMode = MBProgressHUDMode.AnnularDeterminate
                break
            }
            
            // Config hud
            self?.hud              = MBProgressHUD()
            self?.hud              = MBProgressHUD.showHUDAddedTo(view, animated: true)
            self?.hud!.mode        = hudMode
            self?.hud!.labelText   = label
            
        }//)
        
    }
   
    /**
     Remove loading from your view
     
     - throws: Loading already removed to view
     
     - parameter view:       The view that is going to be searched for a loading subview.
     - parameter completion: Optional closure that gets called after the HUD is completely hidden
     */
    public func removeLoading(view: UIView, completion: LoadingCompletionBlock?) throws{
        
        guard hud != nil else { throw LoadingError.LoadingInstanceAlreadyAdded }
        
        dispatch_async(dispatch_get_main_queue()) { [weak self] in
        //NSThread.performBlockOnMainThread({ () -> Void in
            
            if let cmp = completion {
                self?.hud!.completionBlock = cmp
            }
            
            // Hide and remove from super view
            MBProgressHUD.hideHUDForView(view, animated: true)
            
            self?.hud = nil
            
        }//)
    }
}
