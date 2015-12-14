//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/common/preferences/BuildPrefs.java
//

#ifndef _BuildPrefs_H_
#define _BuildPrefs_H_

@class ClassFactory;
@class PrefsStorage;
@class Result;
@protocol IJSONUtils;
@protocol IPrefsChangedListener;
@protocol IPrefsLoader;

#import "JB.h"
#import "APrefs.h"
#import "IBuildPrefs.h"

/**
 @brief Created by mleib on 02/08/2015.
 */
@interface BuildPrefs : APrefs < IBuildPrefs > {
}

- (instancetype)init;

- (instancetype)initWithClassFactory:(ClassFactory *)classFactory;

- (instancetype)initWithIPrefsLoader:(id<IPrefsLoader>)prefsLoader
                      withIJSONUtils:(id<IJSONUtils>)jsonUtils;

- (void)init__ OBJC_METHOD_FAMILY_NONE;

/**
 @brief should create and return the storage (inherited from preferencesStorage)
 */
- (PrefsStorage *)createAndReturnStorage;

- (Result *)setWithNSString:(NSString *)key
                     withId:(id)value;

- (void)registerForChangeWithIPrefsChangedListener:(id<IPrefsChangedListener>)prefsChangedListener;

- (void)unregisterForChangeWithIPrefsChangedListener:(id<IPrefsChangedListener>)prefsChangedListener;

- (NSString *)getObjectId;

@end

__attribute__((always_inline)) inline void BuildPrefs_init() {}

FOUNDATION_EXPORT NSString *BuildPrefs_INT_CONNECTION_TIMEOUT_;
J2OBJC_STATIC_FIELD_GETTER(BuildPrefs, INT_CONNECTION_TIMEOUT_, NSString *)

FOUNDATION_EXPORT NSString *BuildPrefs_INT_DATA_RETRIEVAL_TIMEOUT_;
J2OBJC_STATIC_FIELD_GETTER(BuildPrefs, INT_DATA_RETRIEVAL_TIMEOUT_, NSString *)

FOUNDATION_EXPORT NSString *BuildPrefs_DOUBLE_LOW_LOCATION_ACCURACY_;
J2OBJC_STATIC_FIELD_GETTER(BuildPrefs, DOUBLE_LOW_LOCATION_ACCURACY_, NSString *)

FOUNDATION_EXPORT NSString *BuildPrefs_LONG_HIGH_LOCATION_MAX_AGE_;
J2OBJC_STATIC_FIELD_GETTER(BuildPrefs, LONG_HIGH_LOCATION_MAX_AGE_, NSString *)

typedef BuildPrefs ComIntelWearablePlatformTimeiqCommonPreferencesBuildPrefs;

#endif // _BuildPrefs_H_