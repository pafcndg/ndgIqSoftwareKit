//
//  PlacesViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 08/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TimeIQ
import MapKit
import GoogleMaps

protocol PlacesViewControllerDelegate {
    func didSelectPlace(place: TSOPlace)
}

class PlacesViewController: UIViewController, UITableViewDelegate, UITableViewDataSource, UISearchBarDelegate, UISearchResultsUpdating, SearchPlaceViewControllerDelegate {
    
    var places: Array<TSOPlace>?
    var filteredPlaces = Array<TSOPlace>();
    var searchController: UISearchController?
    var delegate: PlacesViewControllerDelegate! = nil
    var modal = false;
    
    @IBOutlet var placesTableView: UITableView?
    @IBOutlet var activityIndicator: UIActivityIndicatorView?
    
    override func viewDidLoad() {
        self.reloadData()
        
        if (modal)
        {
            self.navigationItem.leftBarButtonItem = UIBarButtonItem.init(barButtonSystemItem: .Cancel, target: self, action: "didClickCancel");
        }
        
        let searchResultsController = UIStoryboard.init(name: "Main", bundle: nil).instantiateViewControllerWithIdentifier("SearchPlaceViewController");
        self.searchController = UISearchController.init(searchResultsController: searchResultsController);
        self.searchController!.searchResultsUpdater = self;
        self.searchController!.searchBar.frame = CGRect(x: self.searchController!.searchBar.frame.origin.x, y: self.searchController!.searchBar.frame.origin.y, width: self.searchController!.searchBar.frame.size.width, height: 44.0);
        self.searchController?.searchBar.placeholder = NSLocalizedString("search_placeholder", comment: "")
        self.searchController?.searchBar.setImage(UIImage(named: "blank"), forSearchBarIcon: .Search, state: .Normal)
        self.searchController?.searchBar.searchTextPositionAdjustment = UIOffsetMake(-19, 0);
        self.placesTableView!.tableHeaderView = self.searchController!.searchBar;
    }
    
    func reloadData() {
        self.places = TimeIQPlacesUtil.getAllPlaces();
        self.placesTableView?.reloadData()
    }
    
    func didClickCancel() {
        self.dismissViewControllerAnimated(true, completion: nil);
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return self.places!.count;
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        if (modal)
        {
            let selectedPlace = self.places![indexPath.row];
            self.delegate.didSelectPlace(selectedPlace)
            self.dismissViewControllerAnimated(true, completion: nil)
        }
    }
    
    
    func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        return true
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = self.placesTableView!.dequeueReusableCellWithIdentifier("placeTableViewCell", forIndexPath: indexPath) as! placeTableViewCell
        
        let place: TSOPlace;
        
        place = self.places![indexPath.row];
        
        cell.place = place;
        return cell;
    }
    
    func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
        if (editingStyle == UITableViewCellEditingStyle.Delete)
        {
            let place = self.places![indexPath.row];
            
            if (TimeIQPlacesUtil.deletePlace(place.getPlaceId()))
            {
                self.places?.removeAtIndex(indexPath.row);
                tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: UITableViewRowAnimation.Automatic);
            }
        }
    }
    
    func filterContetnForSearchText(searchText: String, scope: String = "Name")
    {
        self.filteredPlaces = self.places!.filter({( place : TSOPlace) -> Bool in
            let categoryMatch = (scope == "Name")
            let stringMatch = place.getName().rangeOfString(searchText)
            return categoryMatch && (stringMatch != nil)
        })
    }
    
    func updateSearchResultsForSearchController(searchController: UISearchController) {
        
        if (self.searchController!.searchBar.text!.isEmpty)
        {
            return;
        }
        
        let filter = GMSAutocompleteFilter()
        filter.type = GMSPlacesAutocompleteTypeFilter.Geocode
        GMSPlacesClient.sharedClient().autocompleteQuery(self.searchController!.searchBar.text!, bounds: nil, filter: filter, callback: { (results, error: NSError?) -> Void in
            if let error = error {
                print("Autocomplete error \(error)")
            }
            
            if ((self.searchController!.searchResultsController) != nil) {
                let vc = self.searchController!.searchResultsController as! SearchPlaceViewController
                vc.delegate = self
                vc.places = (results as? Array<GMSAutocompletePrediction>)!;
                vc.searchResultsTableView?.reloadData();
            }
        })
    }
    
    func didSelectAutocompletePlace(place: GMSAutocompletePrediction, placeName: String) {
        self.activityIndicator?.startAnimating()

        GMSPlacesClient.sharedClient().lookUpPlaceID(place.placeID, callback: { (gmsPlace, error) -> Void in
            
            let coordinate = TSOCoordinate(double: (gmsPlace?.coordinate.latitude)!, withDouble: (gmsPlace?.coordinate.longitude)!)
            
            let result = TimeIQPlacesUtil.savePlace(nil, placeType: PlaceType.PlaceType_OTHER, placeAddress: place.attributedFullText.string, placeName: placeName, placeCoordinate: coordinate)
            
            if result.success {
                self.places = TimeIQPlacesUtil.getAllPlaces()
                self.searchController?.searchBar.text = ""
                self.activityIndicator?.stopAnimating()
                self.reloadData()
            }
            else
            {
                self.showAlertView("", message: result.message)
            }
        })
//        let priority = DISPATCH_QUEUE_PRIORITY_DEFAULT
//        dispatch_async(dispatch_get_global_queue(priority, 0)) {
//
//            GMSPlacesClient.sharedClient().lookUpPlaceID(place.placeID, callback: { (gmsPlace, error) -> Void in
//                
//                let coordinate = TSOCoordinate(double: (gmsPlace?.coordinate.latitude)!, withDouble: (gmsPlace?.coordinate.longitude)!)
//                
//                let result = TimeIQPlacesUtil.savePlace(nil, placeType: PlaceType.PlaceType_OTHER, placeAddress: place.attributedFullText.string, placeName: placeName, placeCoordinate: coordinate)
//                
//                if result.success {
//                    self.places = TimeIQPlacesUtil.getAllPlaces()
//                }
//                else
//                {
//                    self.showAlertView("", message: result.message)
//                }
//            })
//            
//            dispatch_async(dispatch_get_main_queue()) {
//                self.searchController?.searchBar.text = ""
//                self.activityIndicator?.stopAnimating()
//                self.reloadData()
//            }
//        }
    }
    
    func showAlertView(title:String, message:String)
    {
        let alert = UIAlertController(title: title, message: message, preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: NSLocalizedString("Dismiss", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
        self.presentViewController(alert, animated: true, completion: nil);
    }
}
