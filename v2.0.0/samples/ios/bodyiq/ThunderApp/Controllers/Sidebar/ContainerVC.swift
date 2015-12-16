//
//  ActivityViewController.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 12/6/15.
//  Copyright © 2015 Intel. All rights reserved.
//


import UIKit

class ContainerVC : UIViewController, UIScrollViewDelegate {
    
    
    // MARK: Members
    
    /// This value matches the left menu's width in the Storyboard
    let leftMenuWidth:CGFloat = 260
    
    let backgroundView = UIView(frame: CGRect(x: 0.0, y: 0.0, width: UIScreen.mainScreen().bounds.size.width, height: 20.0))
    
    
    // MARK: UI
    
    @IBOutlet weak var scrollView: UIScrollView!
    
    
    // MARK: View Life Cycle
    
    override func viewDidLoad() {
        
        // Initially close menu programmatically.
        dispatch_async(dispatch_get_main_queue()) {
            self.closeMenu(false)
        }
                
        // Tab bar controller's child pages have a top-left button toggles the menu
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "toggleMenu", name: Constants.ToggleMenu, object: nil)
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "closeMenuViaNotification", name: Constants.CloseMenu, object: nil)
        
        // Close the menu when the device rotates
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "rotated", name: UIDeviceOrientationDidChangeNotification, object: nil)
        
        // LeftMenu sends openModalWindow
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "openModalWindow", name: Constants.OpenModalWindow, object: nil)

        // Disable vertical scroll 
        self.automaticallyAdjustsScrollViewInsets = false
    }
    
    
    // MARK: deinit
    
    /**
    Cleanup notifications added in viewDidLoad
    */
    deinit {
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }
    
    
    // MARK: Actions
    
    func openModalWindow() {
        performSegueWithIdentifier(Constants.ModalWindowFromSideBar, sender: nil)
    }
    
    func toggleMenu() {
        scrollView.contentOffset.x == 0  ? closeMenu() : openMenu()
    }
    
    func closeMenuViaNotification() {
        closeMenu()
    }
    
    func closeMenu(animated:Bool = true) {
        
        scrollView.setContentOffset(CGPoint(x: leftMenuWidth, y: 0), animated: animated)
        removeBackground()
    }
    
    func openMenu() {
        
        addBackground()
        scrollView.setContentOffset(CGPoint(x: 0, y: 0), animated: true)
    }

    func rotated() {
        if UIDeviceOrientationIsLandscape(UIDevice.currentDevice().orientation) {
            dispatch_async(dispatch_get_main_queue()) {
                self.closeMenu()
            }
        }
    }
    
    
    // MARK: Status Bar Background
    
    private func addBackground() {
        
        backgroundView.backgroundColor = UIColor.init(hex: Constants.StatusBarColor)
        
        self.view.addSubview(backgroundView)
        
    }
    
    private func removeBackground() {
        
        backgroundView.removeFromSuperview()
    }
    
    
    // MARK: UIScrollViewDelegate
    
    func scrollViewDidScroll(scrollView: UIScrollView) {
        //DLog("scrollView.contentOffset.x:: \(scrollView.contentOffset.x)")
    }
    
    func scrollViewWillBeginDragging(scrollView: UIScrollView) {
        scrollView.pagingEnabled = true
    }
    
    func scrollViewDidEndDecelerating(scrollView: UIScrollView) {
        scrollView.pagingEnabled = false
    }
    
}