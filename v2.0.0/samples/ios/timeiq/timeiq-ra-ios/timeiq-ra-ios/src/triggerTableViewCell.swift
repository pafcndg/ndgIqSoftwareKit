//
//  triggerTableViewCell.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 03/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

class triggerTableViewCell : UITableViewCell {
    
    @IBOutlet weak var selectedImageView: UIImageView!
    @IBOutlet weak var titleLabel: UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib();
    }
    
    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated);
        
        selectedImageView.hidden = !selected;
    }
}