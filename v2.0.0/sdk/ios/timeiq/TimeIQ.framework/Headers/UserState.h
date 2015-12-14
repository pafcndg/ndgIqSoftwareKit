//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/api/userstate/UserState.java
//

#ifndef _UserState_H_
#define _UserState_H_

@class UserStateChanges;
@class UserStateData;
@class UserStateType;

#import "JB.h"

/**
 @brief Created by gilsharo on 5/11/2015.
 */
@interface UserState : NSObject {
 @public
  long64 m_timeStamp_;
  UserStateData *m_visits_;
  UserStateData *m_mot_;
  UserStateData *m_carLocation_;
}

/**
 @brief Constructor
 @param visit
 @param mot
 @param carLocation
 */
- (instancetype)initWithUserStateData:(UserStateData *)visit
                    withUserStateData:(UserStateData *)mot
                             withLong:(long64)timeStamp
                    withUserStateData:(UserStateData *)carLocation;

- (long64)getTimeStamp;

- (UserStateData *)getDataWithUserStateType:(UserStateType *)type;

- (UserStateData *)getVisits;

- (UserStateData *)getMot;

- (UserStateData *)getCarLocation;

+ (boolean)equalsWithId:(id)obj1
                  withId:(id)obj2;

+ (UserStateChanges *)compareWithUserState:(UserState *)state1
                             withUserState:(UserState *)state2;

- (NSString *)description;

- (void)copyAllFieldsTo:(UserState *)other;

@end

__attribute__((always_inline)) inline void UserState_init() {}

//J2OBJC_FIELD_SETTER(UserState, m_visits_, UserStateData *)
//J2OBJC_FIELD_SETTER(UserState, m_mot_, UserStateData *)
//J2OBJC_FIELD_SETTER(UserState, m_carLocation_, UserStateData *)

typedef UserState ComIntelWearablePlatformTimeiqApiUserstateUserState;

#endif // _UserState_H_