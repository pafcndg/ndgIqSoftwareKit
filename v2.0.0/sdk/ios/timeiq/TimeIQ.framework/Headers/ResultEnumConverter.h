//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/ttlAlerts/ResultEnumConverter.java
//

#ifndef _ResultEnumConverter_H_
#define _ResultEnumConverter_H_

@class ResultCode;
@class UpdateAlertResultType;

#import "JB.h"

/**
 @brief Created by mleib on 13/04/2015.
 */
@interface ResultEnumConverter : NSObject {
}

+ (UpdateAlertResultType *)convertWithResultCode:(ResultCode *)resultCode;

- (instancetype)init;

@end

__attribute__((always_inline)) inline void ResultEnumConverter_init() {}

typedef ResultEnumConverter ComIntelWearablePlatformTimeiqTtlAlertsResultEnumConverter;

#endif // _ResultEnumConverter_H_