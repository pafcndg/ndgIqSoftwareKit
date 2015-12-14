//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/places/locationprovider/TSOLocationProviderImplBase.java
//

#ifndef _TSOLocationProviderImplBase_H_
#define _TSOLocationProviderImplBase_H_

@class BaseSensorData;
@class ReceiverSensorsRequest;
@class ResultData;
@class SensorType;
@class TSOCoordinate;

#import "JB.h"
#import "ILocationProvider.h"
#import "ISensorDataReceiver.h"

/**
 @brief Created by orencoh2 on 23/11/14.
 */
@interface TSOLocationProviderImplBase : NSObject < ILocationProvider, ISensorDataReceiver > {
 @public
  float GET_LOCATION_MIN_ACCURACY_;
  long64 GET_LOCATION_MIN_AGE_;
  long64 m_latestSampleTimeStamp_;
  float m_accuracy_;
  TSOCoordinate *m_tsoCoordinate_;
  boolean m_bInvokedFuseSingleSample_;
  boolean m_bInvokedGPSSingleSample_;
}

- (instancetype)init;

- (ResultData *)getLastKnownLocation;

- (ResultData *)getPlatformLastKnownLocation;

- (ResultData *)getUserLocationWithLong:(long64)maximumAgeInMillis;

- (ResultData *)getUserLocationWithDouble:(double)minimumAccuracy
                                 withLong:(long64)maximumAgeInMillis;

- (boolean)invokeLocationSamplingWithSensorType:(SensorType *)sensorType;

- (boolean)sampleLocationWithSensorType:(SensorType *)sensorType;

- (boolean)isMatchingAccuracyWithDouble:(double)minimumAccuracy;

- (boolean)isMatchingAgeWithDouble:(double)maximumAgeInMillis;

- (void)onDataReceivedWithBaseSensorData:(BaseSensorData *)data;

- (void)onResetReceiver;

- (void)onSampleIntervalChangedWithSensorType:(SensorType *)sensorType
                                         withLong:(long64)sampleInterval;

- (ReceiverSensorsRequest *)generateReceiverSensorsRequest;

- (void)copyAllFieldsTo:(TSOLocationProviderImplBase *)other;

@end

FOUNDATION_EXPORT BOOL TSOLocationProviderImplBase_initialized;
J2OBJC_STATIC_INIT(TSOLocationProviderImplBase)

//J2OBJC_FIELD_SETTER(TSOLocationProviderImplBase, m_tsoCoordinate_, TSOCoordinate *)

FOUNDATION_EXPORT NSString *TSOLocationProviderImplBase_TAG_;
J2OBJC_STATIC_FIELD_GETTER(TSOLocationProviderImplBase, TAG_, NSString *)

typedef TSOLocationProviderImplBase ComIntelWearablePlatformTimeiqPlacesLocationproviderTSOLocationProviderImplBase;

#endif // _TSOLocationProviderImplBase_H_