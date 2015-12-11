//
//  Constants.swift
//  timeiq-ra-ios
//
//  Created by AviadX Ganon on 03/11/2015.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

let REMINDER_TYPE_DO:Int = 0
let REMINDER_TYPE_CALL:Int = 1
let REMINDER_TYPE_NOTIFY:Int = 2

let TRIGGER_TYPE_ARRIVE:Int = 0
let TRIGGER_TYPE_LEAVE:Int = 1
let TRIGGER_TYPE_NEXT_DRIVE:Int = 2
let TRIGGER_TYPE_BATTERY:Int = 3
let TRIGGER_TYPE_TIME:Int = 4

let IDENT_FIELD = "IDENTIFIER_FIELD";
let USERNAME_FIELD = "USERNAME_FIELD";
let PASSWORD_FIELD = "PASSWORD_FIELD";

enum PlaceType {
    case PlaceType_HOME
    case PlaceType_WORK
    case PlaceType_OTHER
}

enum LateType {
    case NOT_LATE
    case LATE
    case OVERDUE
}

enum RouteDataType {
    case TOO_FAR_FOR_DRIVING                // its a CAR route.
    case TOO_FAR_FOR_WALKING                // its a WAlK route.
    case AT_DESTINATION_WHILE_DRIVING       // did not check route. aerial distance is very near
    case AT_DESTINATION                     // did not check route. aerial distance is very near
    case DESTINATION_CLOSE_BY_WHILE_DRIVING // it's a CAR route. user is driving , and the destination is close by
    case DESTINATION_CLOSE_BY               // it's a CAR route. the destination is close by
    case NEAR_DESTINATION                   // its a WAlK route.
    case WALK                               // its a WAlK route.
    case DRIVE                              // its a CAR route.
    case DRIVE_WHILE_DRIVING                 // its a CAR route.
}