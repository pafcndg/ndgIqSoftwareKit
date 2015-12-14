//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/places/modules/motmodule/Time.java
//

#ifndef _Time_H_
#define _Time_H_

@class SimulatedTimeMode;

#import "JB.h"

@interface Time : NSObject {
 @public
  SimulatedTimeMode *m_simulatedTimeMode_;
  long64 m_displacedTime_;
  long64 m_timeOfDisplacement_;
  long64 m_timeReferenceForFaster_;
  float m_fastFactor_;
}

/**
 @brief Get the instance of this Singleton class
 */
+ (Time *)getInstance;

- (long64)getCurrent;

- (void)setDisplacedWithLong:(long64)displacedTime;

- (void)setFasterWithFloat:(float)fastFactor
                  withLong:(long64)timeReferenceForFaster;

- (void)setFasterWithFloat:(float)fastFactor;

- (instancetype)init;

- (void)copyAllFieldsTo:(Time *)other;

@end

FOUNDATION_EXPORT BOOL Time_initialized;
J2OBJC_STATIC_INIT(Time)

//J2OBJC_FIELD_SETTER(Time, m_simulatedTimeMode_, SimulatedTimeMode *)

FOUNDATION_EXPORT Time *Time_inst_;
J2OBJC_STATIC_FIELD_GETTER(Time, inst_, Time *)
//J2OBJC_STATIC_FIELD_SETTER(Time, inst_, Time *)

FOUNDATION_EXPORT NSString *Time_TAG_;
J2OBJC_STATIC_FIELD_GETTER(Time, TAG_, NSString *)

typedef Time ComIntelWearablePlatformTimeiqPlacesModulesMotmoduleTime;

#endif // _Time_H_