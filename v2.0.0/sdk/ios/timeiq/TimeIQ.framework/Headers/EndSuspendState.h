//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/places/modules/visitinplacemodule/state/EndSuspendState.java
//

#ifndef _EndSuspendState_H_
#define _EndSuspendState_H_

@class IPC;
@class PlaceID;
@class Visit;
@protocol IState;

#import "JB.h"
#import "AbstractState.h"

@interface EndSuspendState : AbstractState {
 @public
  int suspendTimeMins_;
}

- (instancetype)initWithPlaceID:(PlaceID *)id_
                    withIPC:(IPC *)ipc
                       withLong:(long64)timestamp;

- (instancetype)initWithPlaceID:(PlaceID *)id_
                       withLong:(long64)timestamp
                    withBoolean:(boolean)hasVisit;

- (void)updateSuspendTimeWithIPC:(IPC *)ipc
                         withBoolean:(boolean)hasVisit;

- (id<IState>)reportStateWhenExpiredWithLong:(long64)timestamp;

- (id<IState>)updateWifiIpcWithIPC:(IPC *)ipc
                              withLong:(long64)timestamp;

- (id<IState>)updateIpcWithIPC:(IPC *)ipc
                          withLong:(long64)timestamp
                       withIPC:(IPC *)lastWifIpc
                          withLong:(long64)wifiTimestamp;

- (id<IState>)updateVisitStartWithVisit:(Visit *)visit
                               withLong:(long64)timestamp;

- (id<IState>)updateVisitEndWithVisit:(Visit *)visit
                             withLong:(long64)timestamp;

- (id<IState>)updateTimerWithLong:(long64)timestamp;

- (void)copyAllFieldsTo:(EndSuspendState *)other;

@end

__attribute__((always_inline)) inline void EndSuspendState_init() {}

typedef EndSuspendState ComIntelWearablePlatformTimeiqPlacesModulesVisitinplacemoduleStateEndSuspendState;

#endif // _EndSuspendState_H_