//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/common/storage/db/TSOSqlDbException.java
//

#ifndef _TSOSqlDbException_H_
#define _TSOSqlDbException_H_

#import "NSException+JavaAPI.h"

#import "JB.h"
#import "NSException+JavaAPI.h"

/**
 @brief Created by yaronabr on 19-Jul-15.
 */
@interface TSOSqlDbException : Exception {
}

- (instancetype)initWithThrowable:(Throwable *)ex;

- (instancetype)initWithNSString:(NSString *)message;

@end

__attribute__((always_inline)) inline void TSOSqlDbException_init() {}

typedef TSOSqlDbException ComIntelWearablePlatformTimeiqCommonStorageDbTSOSqlDbException;

#endif // _TSOSqlDbException_H_