//
//  Generated by the J2ObjC translator.  DO NOT EDIT!
//  source: //com/intel/wearable/platform/timeiq/places/modules/placesmodule/geometry/quadtree/QuadTree.java
//

#ifndef _QuadTree_H_
#define _QuadTree_H_

@class Coord;
@class Quad;
#import "JBArrayList.h"

#import "JB.h"

#define QuadTree_MinTileSize 1.0E-5

/**
 @brief Quad tree for geo-hashing Assumption - the qtree is constructed on predefined points
 @author ylempert
 */
@interface QuadTree : NSObject {
 @public
  double additionalDistance_;
  /**
   @brief Points in this unless there are tiles in which case, quads is not null
   */
  Quad *root_;
}

- (instancetype)init;

- (instancetype)initWithCoord:(Coord *)c1
                    withCoord:(Coord *)c2;

/**
 @brief The tree in general should be constructed once.
 In case when building the tree is called again, on of the new points are contained in the present tree, the tree grows. Otherwise, the tree is reconstructed which is not very efficient.
 @param points
 */
- (void)buildTreeWithJavaUtilList:(ArrayList*)points;

/**
 @brief Filling up the array of points recursively
 @param points
 */
- (void)getAllPointsWithJavaUtilList:(ArrayList*)points;

- (void)addPointWithCoord:(Coord *)p;

- (ArrayList*)findNearestPointsWithCoord:(Coord *)p
                                    withDouble:(double)distance;

+ (ArrayList*)boundingBoxWithJavaUtilList:(ArrayList*)points;

- (boolean)isEmpty;

- (void)resetTreeWithJavaUtilList:(ArrayList*)bbox;

- (ArrayList*)getBoundingBox;

- (void)print;

- (void)updatePointWithCoord:(Coord *)point;

- (void)copyAllFieldsTo:(QuadTree *)other;

@end

__attribute__((always_inline)) inline void QuadTree_init() {}

//J2OBJC_FIELD_SETTER(QuadTree, root_, Quad *)

FOUNDATION_EXPORT int QuadTree_MaxPointsInTile_;
J2OBJC_STATIC_FIELD_GETTER(QuadTree, MaxPointsInTile_, int)
//J2OBJC_STATIC_FIELD_REF_GETTER(QuadTree, MaxPointsInTile_, int)

J2OBJC_STATIC_FIELD_GETTER(QuadTree, MinTileSize, double)

typedef QuadTree ComIntelWearablePlatformTimeiqPlacesModulesPlacesmoduleGeometryQuadtreeQuadTree;

#endif // _QuadTree_H_