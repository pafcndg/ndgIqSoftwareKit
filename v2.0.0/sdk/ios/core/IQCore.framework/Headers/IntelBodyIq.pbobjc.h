// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: IntelBodyIQ.proto

#import "GPBProtocolBuffers.h"

#if GOOGLE_PROTOBUF_OBJC_GEN_VERSION != 30000
#error This file was generated by a different version of protoc-gen-objc which is incompatible with your Protocol Buffer sources.
#endif

// @@protoc_insertion_point(imports)

CF_EXTERN_C_BEGIN

@class Activity_bikingActivity;
@class Activity_runningActivity;
@class Activity_walkingActivity;
@class DateTime;

NS_ASSUME_NONNULL_BEGIN

#pragma mark - Enum Activity_activityType

typedef GPB_ENUM(Activity_activityType) {
  Activity_activityType_Walking = 0,
  Activity_activityType_Running = 1,
  Activity_activityType_Biking = 2,
};

GPBEnumDescriptor *Activity_activityType_EnumDescriptor(void);

BOOL Activity_activityType_IsValidValue(int32_t value);

#pragma mark - Enum Activity_activityStatus

typedef GPB_ENUM(Activity_activityStatus) {
  // activity is started 
  Activity_activityStatus_Started = 0,

  // activity is on-going 
  Activity_activityStatus_Ongoing = 1,

  // activity is finished 
  Activity_activityStatus_Finished = 2,
};

GPBEnumDescriptor *Activity_activityStatus_EnumDescriptor(void);

BOOL Activity_activityStatus_IsValidValue(int32_t value);

#pragma mark - IntelBodyIqRoot

@interface IntelBodyIqRoot : GPBRootObject

// The base class provides:
//   + (GPBExtensionRegistry *)extensionRegistry;
// which is an GPBExtensionRegistry that includes all the extensions defined by
// this file and all files that it depends on.

@end

#pragma mark - Activity

typedef GPB_ENUM(Activity_FieldNumber) {
  Activity_FieldNumber_Type = 1,
  Activity_FieldNumber_TimestampBegin = 2,
  Activity_FieldNumber_TimestampEnd = 3,
  Activity_FieldNumber_ActivityStatus = 4,
  Activity_FieldNumber_WalkingActivity = 5,
  Activity_FieldNumber_RunningActivity = 6,
  Activity_FieldNumber_BikingActivity = 7,
};

@interface Activity : GPBMessage

@property(nonatomic, readwrite) BOOL hasType;
@property(nonatomic, readwrite) Activity_activityType type;

@property(nonatomic, readwrite) BOOL hasTimestampBegin;
@property(nonatomic, readwrite, strong, null_resettable) DateTime *timestampBegin;

@property(nonatomic, readwrite) BOOL hasTimestampEnd;
@property(nonatomic, readwrite, strong, null_resettable) DateTime *timestampEnd;

@property(nonatomic, readwrite) BOOL hasActivityStatus;
@property(nonatomic, readwrite) Activity_activityStatus activityStatus;

@property(nonatomic, readwrite) BOOL hasWalkingActivity;
@property(nonatomic, readwrite, strong, null_resettable) Activity_walkingActivity *walkingActivity;

@property(nonatomic, readwrite) BOOL hasRunningActivity;
@property(nonatomic, readwrite, strong, null_resettable) Activity_runningActivity *runningActivity;

@property(nonatomic, readwrite) BOOL hasBikingActivity;
@property(nonatomic, readwrite, strong, null_resettable) Activity_bikingActivity *bikingActivity;

@end

#pragma mark - Activity_walkingActivity

typedef GPB_ENUM(Activity_walkingActivity_FieldNumber) {
  Activity_walkingActivity_FieldNumber_StepCount = 1,
};

@interface Activity_walkingActivity : GPBMessage

@property(nonatomic, readwrite) BOOL hasStepCount;
@property(nonatomic, readwrite) uint32_t stepCount;

@end

#pragma mark - Activity_runningActivity

typedef GPB_ENUM(Activity_runningActivity_FieldNumber) {
  Activity_runningActivity_FieldNumber_StepCount = 1,
};

@interface Activity_runningActivity : GPBMessage

@property(nonatomic, readwrite) BOOL hasStepCount;
@property(nonatomic, readwrite) uint32_t stepCount;

@end

#pragma mark - Activity_bikingActivity

// nothing for now until external sensors support 
@interface Activity_bikingActivity : GPBMessage

@end

#pragma mark - ActivitySettings

typedef GPB_ENUM(ActivitySettings_FieldNumber) {
  ActivitySettings_FieldNumber_ActivityClassificationEnableDisable = 1,
  ActivitySettings_FieldNumber_ActivityReportGranularity = 2,
  ActivitySettings_FieldNumber_ActivityDiscardStoredData = 3,
};

@interface ActivitySettings : GPBMessage

@property(nonatomic, readwrite) BOOL hasActivityClassificationEnableDisable;
@property(nonatomic, readwrite) BOOL activityClassificationEnableDisable;

@property(nonatomic, readwrite) BOOL hasActivityReportGranularity;
@property(nonatomic, readwrite) uint32_t activityReportGranularity;

// in minutes - this is the maximum (end-begin) time window for each activity message 
@property(nonatomic, readwrite) BOOL hasActivityDiscardStoredData;
@property(nonatomic, readwrite) BOOL activityDiscardStoredData;

@end

NS_ASSUME_NONNULL_END

CF_EXTERN_C_END

// @@protoc_insertion_point(global_scope)
