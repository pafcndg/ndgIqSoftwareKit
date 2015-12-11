//
//  placeTableViewCell.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 09/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import TSO
import MapKit
import GoogleMaps

class placeTableViewCell: UITableViewCell {
    
    @IBOutlet var placeImageView: UIImageView?
    @IBOutlet var placeNameLabel: UILabel?
    @IBOutlet var placeInfoLabel: UILabel?
    
    var place: TSOPlace? {
        willSet(newPlace) {
            
            self.placeNameLabel?.text = newPlace?.getName();
            self.placeInfoLabel?.text = newPlace?.getAddress();
            self.placeImageView?.image = UIImage(named: self.imageForPlace(newPlace!));
        }
    }
    
    var item: GMSAutocompletePrediction? {
        willSet(newItem) {
            self.placeNameLabel?.text = newItem!.placeID;
            self.placeInfoLabel?.text = newItem!.attributedFullText.string
            self.placeImageView?.image = UIImage(named: "list_onmap");
        }
    }
    
    override func awakeFromNib() {
        super.awakeFromNib();
    }
    
    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated);
    }
    
    func imageForPlace(place: TSOPlace) -> String
    {
        var placeImageName: String?

        switch (place.getSemanticKey()){

//        case SemanticKey_get_HOME_():
//            placeImageName = "list_home";
//            break;
//        case SemanticKey_get_WORK_():
//            placeImageName = "list_word"
//            break;
        default:
            placeImageName = "list_onmap";
        }
        return placeImageName!;
        
    }
    
}
