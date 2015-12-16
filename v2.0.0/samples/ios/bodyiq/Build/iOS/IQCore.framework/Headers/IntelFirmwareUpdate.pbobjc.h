// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: IntelFirmwareUpdate.proto

#import "GPBProtocolBuffers.h"

#if GOOGLE_PROTOBUF_OBJC_GEN_VERSION != 30000
#error This file was generated by a different version of protoc-gen-objc which is incompatible with your Protocol Buffer sources.
#endif

// @@protoc_insertion_point(imports)

CF_EXTERN_C_BEGIN

NS_ASSUME_NONNULL_BEGIN

#pragma mark - Enum FirmwareUpdate_firmwareOperation

typedef GPB_ENUM(FirmwareUpdate_firmwareOperation) {
  // Start the firmware update. The protobuf shall contain  at least the firmware_total_length 
  FirmwareUpdate_firmwareOperation_Start = 0,

  // Continue the firmware update and provide next chunk of firmware data with the size 
  FirmwareUpdate_firmwareOperation_Continue = 1,

  // Cancel the current firmware update and discard all previously transfered data 
  FirmwareUpdate_firmwareOperation_Cancel = 2,
};

GPBEnumDescriptor *FirmwareUpdate_firmwareOperation_EnumDescriptor(void);

BOOL FirmwareUpdate_firmwareOperation_IsValidValue(int32_t value);

#pragma mark - Enum FirmwareUpdateState_respType

typedef GPB_ENUM(FirmwareUpdateState_respType) {
  FirmwareUpdateState_respType_Started = 0,
  FirmwareUpdateState_respType_InProgress = 1,
  FirmwareUpdateState_respType_None = 2,
};

GPBEnumDescriptor *FirmwareUpdateState_respType_EnumDescriptor(void);

BOOL FirmwareUpdateState_respType_IsValidValue(int32_t value);

#pragma mark - IntelFirmwareUpdateRoot

@interface IntelFirmwareUpdateRoot : GPBRootObject

// The base class provides:
//   + (GPBExtensionRegistry *)extensionRegistry;
// which is an GPBExtensionRegistry that includes all the extensions defined by
// this file and all files that it depends on.

@end

#pragma mark - FirmwareUpdate

typedef GPB_ENUM(FirmwareUpdate_FieldNumber) {
  FirmwareUpdate_FieldNumber_FirmwareUpdateCmd = 1,
  FirmwareUpdate_FieldNumber_FirmwareTotalLength = 2,
  FirmwareUpdate_FieldNumber_Data_p = 3,
  FirmwareUpdate_FieldNumber_Offset = 4,
};

// topic intel/core/firmwareupdate 
@interface FirmwareUpdate : GPBMessage

@property(nonatomic, readwrite) BOOL hasFirmwareUpdateCmd;
@property(nonatomic, readwrite) FirmwareUpdate_firmwareOperation firmwareUpdateCmd;

// Firmware total length in bytes 
@property(nonatomic, readwrite) BOOL hasFirmwareTotalLength;
@property(nonatomic, readwrite) uint32_t firmwareTotalLength;

// Firmware data chunk, max size = 130 
@property(nonatomic, readwrite) BOOL hasData_p;
@property(nonatomic, readwrite, copy, null_resettable) NSData *data_p;

// Offset in byte of the firmware within the firmware total length 
@property(nonatomic, readwrite) BOOL hasOffset;
@property(nonatomic, readwrite) uint32_t offset;

@end

#pragma mark - FirmwareUpdateState

typedef GPB_ENUM(FirmwareUpdateState_FieldNumber) {
  FirmwareUpdateState_FieldNumber_FirmwareUpdateState = 1,
};

// topic intel/core/firmwareupdate/state 
@interface FirmwareUpdateState : GPBMessage

@property(nonatomic, readwrite) BOOL hasFirmwareUpdateState;
@property(nonatomic, readwrite) FirmwareUpdateState_respType firmwareUpdateState;

@end

NS_ASSUME_NONNULL_END

CF_EXTERN_C_END

// @@protoc_insertion_point(global_scope)
