//
//  LocationApi.h
//  TSO
//
//  Created by AviadX Ganon on 06/12/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface LocationApi : NSObject

+(CLLocation*)currentLocation;
+(void)setCurrentLocation:(CLLocation*)loc;

@end
