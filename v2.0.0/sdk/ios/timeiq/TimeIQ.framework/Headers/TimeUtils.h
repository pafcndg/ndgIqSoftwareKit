//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/utils/TimeUtils.java
//

#ifndef _TimeUtils_H_
#define _TimeUtils_H_

#import "JBDateFormatter.h"

#import "JB.h"

/**
 @brief Created by aeirew on 13/10/2015.
 */
@interface TimeUtils : NSObject {
}

+ (NSString *)convertLongToISOTimeWithLong:(long64)time;

- (instancetype)init;

@end

FOUNDATION_EXPORT BOOL TimeUtils_initialized;
J2OBJC_STATIC_INIT(TimeUtils)

FOUNDATION_EXPORT NSString *TimeUtils_DATA_FORMAT_;
J2OBJC_STATIC_FIELD_GETTER(TimeUtils, DATA_FORMAT_, NSString *)

FOUNDATION_EXPORT SimpleDateFormat *TimeUtils_sdf_;
J2OBJC_STATIC_FIELD_GETTER(TimeUtils, sdf_, SimpleDateFormat *)

typedef TimeUtils ComIntelWearablePlatformTimeiqUtilsTimeUtils;

#endif // _TimeUtils_H_