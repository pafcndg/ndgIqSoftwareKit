//
//  Errors.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/10/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

/**
 Loading error conditions
 
 - LoadingInstanceAlreadyAdded:   The hud is already in place and you are trying to add it again.
 - LoadingInstanceAlreadyRemoved: The hud is already been removed and you are trying to remove it again.
 */
enum LoadingError: ErrorType {
    case LoadingInstanceAlreadyAdded
    case LoadingInstanceAlreadyRemoved
}

/**
 Communication protocol error conditions
 
 - CommProtocolNotImplemented:   Protocol pending to be implemented
 - LoadingInstanceAlreadyRemoved: Protocol not recognized
 */
enum CommunicationProtocolError: ErrorType {
    case CommProtocolNotImplemented
    case CommProtocolNotRecognized
}

enum WearableNotificationError: ErrorType {
    case WearableNotificationInvalid
}

enum NotificationDurationPatternError: ErrorType {
    case NotificationDurationPatternInvalid
}

enum NotificationLedPatternError: ErrorType {
    case NotificationLedPatternInvalid
}

enum NotificationVibrationPatternError: ErrorType {
    case NotificationVibrationPatternInvalid
}

enum NotificationError: ErrorType {
    case NotificationInvalid
}

enum ColorComponentError: ErrorType {
    case InvalidRedComponent
    case InvalidGreenComponent
    case InvalidBlueComponent
}

enum LocalDataStoreError: ErrorType {
    case UnreachableObject
}