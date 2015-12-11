//
//  SearchPlaceViewController.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 10/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TSO
import MapKit
import GoogleMaps

protocol SearchPlaceViewControllerDelegate {
    func didSelectAutocompletePlace(place: GMSAutocompletePrediction, placeName: String)
}

class SearchPlaceViewController: UIViewController {
    
    var places = Array<GMSAutocompletePrediction>()
    var delegate: SearchPlaceViewControllerDelegate! = nil
    @IBOutlet var searchResultsTableView: UITableView?
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if (self.places.count > 0)
        {
            self.searchResultsTableView!.backgroundView = nil
            return self.places.count
        }
        else
        {
            // Display a message when the table is empty
            self.searchResultsTableView!.backgroundView = common.labelMessageForString(NSLocalizedString("nothing_to_display_autocomplete_places", comment: ""));
        }
        
        return 0;
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        
        let place = self.places[indexPath.row]
        
        let alertController = UIAlertController(title: "Enter place name", message: "", preferredStyle: UIAlertControllerStyle.Alert)
        
        alertController.addTextFieldWithConfigurationHandler {
            (textField) -> Void in
            textField.placeholder = "Enter place name"
            textField.text = place.attributedFullText.string
        }
        
        alertController.addAction(UIAlertAction(
            title: "Create", style: UIAlertActionStyle.Default) {
                (action) -> Void in
                
                let placeName = ((alertController.textFields?.first)! as UITextField).text
                
                if (placeName!.isEmpty)
                {
                    self.showAlertView(NSLocalizedString("", comment: ""), message: NSLocalizedString("toast_no_valid_name", comment: ""));
                        return;
                }
                
                self.delegate.didSelectAutocompletePlace(place, placeName: placeName!)
                self.dismissViewControllerAnimated(true, completion: nil)
            })
        
        alertController.addAction(UIAlertAction(title: "Cancel", style: UIAlertActionStyle.Default, handler:{ (UIAlertAction)in
            //User click Cancel button
        }))
        
        self.presentViewController(alertController, animated: true, completion: {
            //completion block
        })
    }
    
    func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        return true
    }
        
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = self.searchResultsTableView!.dequeueReusableCellWithIdentifier("searchResultsTableViewCell", forIndexPath: indexPath)
        
        let place = self.places[indexPath.row]
        
        cell.textLabel?.text = place.attributedFullText.string
        
        return cell;
    }
    
    func showAlertView(title:String, message:String)
    {
        let alert = UIAlertController(title: title, message: message, preferredStyle: UIAlertControllerStyle.Alert)
        alert.addAction(UIAlertAction(title: NSLocalizedString("Close", comment: ""), style: UIAlertActionStyle.Default, handler: nil));
        self.presentViewController(alert, animated: true, completion: nil);
    }
}