//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/api/userstate/VisitedPlaces.java
//

#ifndef _VisitedPlaces_H_
#define _VisitedPlaces_H_

@class PlaceID;
#import "JBHashSet.h"
#import "JBHashtable.h"

#import "JB.h"
#import "IMappable.h"
#import "JBArrayList.h"

/**
 @brief Created by gilsharo on 5/19/2015.
 */
@interface VisitedPlaces : ArrayList < IMappable > {
}

/**
 @brief Default empty constructor for IMappable
 */
- (instancetype)init;

/**
 @brief Copy constructor
 @param other
 */
- (instancetype)initWithVisitedPlaces:(VisitedPlaces *)other;

- (boolean)semanticContainsWithPlaceID:(PlaceID *)id_;

- (int)semanticIndexOfWithPlaceID:(PlaceID *)id_;

- (boolean)semanticRemoveWithPlaceID:(PlaceID *)id_;

- (boolean)semanticRemoveAllWithJavaUtilCollection:(Set*)ids;

- (void)getMutualDiffWithVisitedPlaces:(VisitedPlaces *)other
                     withVisitedPlaces:(VisitedPlaces *)inThisNotInOther
                     withVisitedPlaces:(VisitedPlaces *)inOtherNotInThis;

- (void)getMutualSemanticDiffWithVisitedPlaces:(VisitedPlaces *)other
                             withVisitedPlaces:(VisitedPlaces *)inThisNotInOther
                             withVisitedPlaces:(VisitedPlaces *)inOtherNotInThis;

- (void)initObjectFromMapWithJavaUtilMap:(HashMap*)map OBJC_METHOD_FAMILY_NONE;

- (HashMap*)objectToMap;


@end

__attribute__((always_inline)) inline void VisitedPlaces_init() {}

FOUNDATION_EXPORT NSString *VisitedPlaces_IDS_;
J2OBJC_STATIC_FIELD_GETTER(VisitedPlaces, IDS_, NSString *)

typedef VisitedPlaces ComIntelWearablePlatformTimeiqApiUserstateVisitedPlaces;

#endif // _VisitedPlaces_H_