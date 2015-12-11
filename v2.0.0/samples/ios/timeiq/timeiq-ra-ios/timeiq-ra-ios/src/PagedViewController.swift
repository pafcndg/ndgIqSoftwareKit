//
//  PagedViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 11/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

import UIKit

protocol PagedViewControllerDelegate {
    func didAddOrUpdateReminder();
}

class PagedViewController: UIViewController, UIPageViewControllerDataSource, UIPageViewControllerDelegate, BaseAddReminderViewControllerDelegate
{
    var pageViewController : UIPageViewController?
    var pageTitles : Array<String> = ["DO", "Call", "Notify", "Be"]
    var currentIndex : Int = 0
    var prevIndex : Int = 0
    var pendingViewController: BaseAddReminderViewController?
    var delegate:PagedViewControllerDelegate! = nil;
    @IBOutlet var contentView: UIView?
    @IBOutlet var reminderSegmentControl: UISegmentedControl?
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        
        self.navigationController?.navigationBar.setBackgroundImage(UIImage(), forBarMetrics: UIBarMetrics.Default)
        self.navigationController?.navigationBar.shadowImage = UIImage()
        
        pageViewController = UIPageViewController(transitionStyle: .Scroll, navigationOrientation: .Horizontal, options: nil)
        pageViewController!.dataSource = self
        pageViewController?.delegate = self;
        
        let startingViewController: BaseAddReminderViewController = viewControllerAtIndex(0)!
        let viewControllers = [startingViewController]
        pageViewController!.setViewControllers(viewControllers, direction: .Forward, animated: false, completion: nil)
        pageViewController!.view.frame = CGRectMake(0, 0, contentView!.frame.size.width, contentView!.frame.size.height);
        
        addChildViewController(pageViewController!)
        contentView!.addSubview(pageViewController!.view)
        pageViewController!.didMoveToParentViewController(self)
    }
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
    
    func moveToIndex(index: Int) {
        let nextVC = self.viewControllerAtIndex(index)
        
        if (index > prevIndex)
        {
            self.pageViewController!.setViewControllers([nextVC!], direction: .Forward, animated: true, completion: nil)
        }
        else
        {
            self.pageViewController!.setViewControllers([nextVC!], direction: .Reverse, animated: true, completion: nil)
        }
    }
    
    func pageViewController(pageViewController: UIPageViewController, viewControllerBeforeViewController viewController: UIViewController) -> UIViewController?
    {
        var index = (viewController as! BaseAddReminderViewController).pageIndex
        
        if (index == 0) || (index == NSNotFound) {
            return nil
        }
        
        index--
        
        return viewControllerAtIndex(index)
    }
    
    func pageViewController(pageViewController: UIPageViewController, viewControllerAfterViewController viewController: UIViewController) -> UIViewController?
    {
        var index = (viewController as! BaseAddReminderViewController).pageIndex
        
        if index == NSNotFound {
            return nil
        }
        
        index++
        
        if (index == self.pageTitles.count) {
            return nil
        }
        
        return viewControllerAtIndex(index)
    }
    
    func viewControllerAtIndex(index: Int) -> BaseAddReminderViewController?
    {
        if self.pageTitles.count == 0 || index >= self.pageTitles.count
        {
            return nil
        }
        
        // Create a new view controller and pass suitable data.
        let pageContentViewController: BaseAddReminderViewController
        if (index == 3)
        {
            pageContentViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewControllerWithIdentifier("AddBeReminderViewController") as! BaseAddReminderViewController;
        }
        else
        {
            pageContentViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewControllerWithIdentifier("AddReminderViewController") as! BaseAddReminderViewController;
        }
    
        pageContentViewController.delegate = self;
        pageContentViewController.reminderType = index
        pageContentViewController.pageIndex = index
        currentIndex = index
        
        return pageContentViewController
    }
    
//    func presentationCountForPageViewController(pageViewController: UIPageViewController) -> Int
//    {
//        return self.pageTitles.count
//    }
//    
//    func presentationIndexForPageViewController(pageViewController: UIPageViewController) -> Int
//    {
//        return 0
//    }
    
    @IBAction func valueChanged(sender: UISegmentedControl) {
        self.prevIndex = self.currentIndex;
        self.moveToIndex(sender.selectedSegmentIndex)
    }
    
    @IBAction func didClickDone(sender: AnyObject) {
        
        let currentViewController = self.pageViewController?.viewControllers?.first as! BaseAddReminderViewController;
        currentViewController.didClickDone()
        
    }
    
    @IBAction func didClickCancel(sender: AnyObject) {
        
        self.dismissViewControllerAnimated(true, completion: nil);
        
        
    }
    
    func pageViewController(pageViewController: UIPageViewController, willTransitionToViewControllers pendingViewControllers: [UIViewController]) {
        
        self.pendingViewController = (pendingViewControllers.first as! BaseAddReminderViewController);
    }
    
    func pageViewController(pageViewController: UIPageViewController, didFinishAnimating finished: Bool, previousViewControllers: [UIViewController], transitionCompleted completed: Bool) {
        
        if (completed && pendingViewController != nil)
        {
            self.reminderSegmentControl?.selectedSegmentIndex = self.pendingViewController!.pageIndex;
        }
    }
    
    func didAddReminder() {
        self.delegate.didAddOrUpdateReminder()
    }
    
    func didUpdateReminder() {
        self.delegate.didAddOrUpdateReminder()
    }
}

