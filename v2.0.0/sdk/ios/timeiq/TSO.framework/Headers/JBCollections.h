//
//  JBCollections.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/17/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Collections_h__
#define __JavaBridge__Collections_h__

#import <Foundation/Foundation.h>
#import "NSArray+JavaAPI.h"
#import "JBComparator.h"
#import "JBHashtable.h"

@interface Collections : NSObject

+(void)sortWithJavaUtilList:(Array*)arr;

+(void)sortWithJavaUtilList:(Array*)arr withIComparator:(id<IComparator>)comparator;

+(int) binarySearchWithJavaUtilList:(Array*)arr withId:item withIComparator:(id<IComparator>)comparator;

+(HashMap*)unmodifiableMapWithJavaUtilMap:(HashMap*)map;

+(Array*) unmodifiableCollectionWithJavaUtilCollection:(Array*)collection;

+(void)fillWithJavaUtilList:(Array*)arr withId:(id)obj;

@end

#endif