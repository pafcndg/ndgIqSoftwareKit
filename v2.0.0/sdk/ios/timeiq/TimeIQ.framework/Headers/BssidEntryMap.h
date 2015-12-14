//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/places/modules/visitsmodule/wifisim/BssidEntryMap.java
//

#ifndef _BssidEntryMap_H_
#define _BssidEntryMap_H_

#import "JBArrayList.h"
#import "JBHashtable.h"

#import "JB.h"

@interface BssidEntryMap : NSObject {
 @public
  HashMap* bssidEntries_;
}

- (int)bssidToEntryWithNSString:(NSString *)bssid;

- (NSString *)entryToBssidWithInt:(int)entry_;

- (void)removeSourceWithNSString:(NSString *)bssid;

- (void)addSourceWithNSString:(NSString *)bssid
                      withInt:(int)entry_;

- (void)reset;

- (int)numberOfSources;

- (ArrayList*)getSources;

- (instancetype)init;

- (void)copyAllFieldsTo:(BssidEntryMap *)other;

@end

__attribute__((always_inline)) inline void BssidEntryMap_init() {}

//J2OBJC_FIELD_SETTER(BssidEntryMap, bssidEntries_, HashMap*)

typedef BssidEntryMap ComIntelWearablePlatformTimeiqPlacesModulesVisitsmoduleWifisimBssidEntryMap;

#endif // _BssidEntryMap_H_