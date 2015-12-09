//
//  NSMutableArray+JavaAPI.h
//  TSO
//
//  Created by Bari Levi on 4/13/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"
#import "JBHashtable.h"


@interface MapValues : NSMutableArray

- (instancetype)initWithKeys:(NSArray*)arrKeys withOriginMap:(Map*)map;
- (instancetype)initWithValues:(NSArray*)arrValues withOriginMap:(Map*)map;

- (NSUInteger) count;
- (id)objectAtIndex:(NSUInteger)index;
- (BOOL)containsObject:(id)anObject;
- (NSString *)description;
- (id)firstObjectCommonWithArray:(NSArray *)otherArray;

- (NSUInteger)indexOfObject:(id)anObject;
- (BOOL)isEqualToArray:(NSArray *)otherArray;
- (id) firstObject;
- (id) lastObject;
- (NSEnumerator *)objectEnumerator;
- (NSEnumerator *)reverseObjectEnumerator;

- (NSArray *)sortedArrayUsingFunction:(NSInteger (*)(id, id, void *))comparator context:(void *)context;
- (NSArray *)sortedArrayUsingFunction:(NSInteger (*)(id, id, void *))comparator context:(void *)context hint:(NSData *)hint;
- (NSArray *)sortedArrayUsingSelector:(SEL)comparator;
- (NSArray *)subarrayWithRange:(NSRange)range;
- (BOOL)writeToFile:(NSString *)path atomically:(BOOL)useAuxiliaryFile;
- (BOOL)writeToURL:(NSURL *)url atomically:(BOOL)atomically;

//- (void)makeObjectsPerformSelector:(SEL)aSelector;
//- (void)makeObjectsPerformSelector:(SEL)aSelector withObject:(id)argument;

- (NSArray *)objectsAtIndexes:(NSIndexSet *)indexes;

- (id)objectAtIndexedSubscript:(NSUInteger)idx NS_AVAILABLE(10_8, 6_0);

- (NSArray *)sortedArrayUsingComparator:(NSComparator)cmptr NS_AVAILABLE(10_6, 4_0);
- (NSArray *)sortedArrayWithOptions:(NSSortOptions)opts usingComparator:(NSComparator)cmptr NS_AVAILABLE(10_6, 4_0);
- (NSUInteger)indexOfObject:(id)obj inSortedRange:(NSRange)r options:(NSBinarySearchingOptions)opts usingComparator:(NSComparator)cmp NS_AVAILABLE(10_6, 4_0); // binary search

//- (instancetype)initWithArray:(NSArray *)array;
//- (instancetype)initWithArray:(NSArray *)array copyItems:(BOOL)flag;

- (void)addObject:(id)anObject;
- (void)insertObject:(id)anObject atIndex:(NSUInteger)index;
- (void)removeLastObject;//impl
- (void)removeObjectAtIndex:(NSUInteger)index;//impl
- (void)replaceObjectAtIndex:(NSUInteger)index withObject:(id)anObject;
//- (instancetype)init NS_DESIGNATED_INITIALIZER;
//- (instancetype)initWithCapacity:(NSUInteger)numItems NS_DESIGNATED_INITIALIZER;
//- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_DESIGNATED_INITIALIZER;

- (void)addObjectsFromArray:(NSArray *)otherArray;
- (void)exchangeObjectAtIndex:(NSUInteger)idx1 withObjectAtIndex:(NSUInteger)idx2;//impl
- (void)removeAllObjects;//impl
- (void)removeObject:(id)anObject inRange:(NSRange)range;//impl
- (void)removeObject:(id)anObject;//impl
- (void)removeObjectIdenticalTo:(id)anObject inRange:(NSRange)range;//impl
- (void)removeObjectIdenticalTo:(id)anObject;//impl
- (void)removeObjectsFromIndices:(NSUInteger *)indices numIndices:(NSUInteger)cnt NS_DEPRECATED(10_0, 10_6, 2_0, 4_0);//impl
- (void)removeObjectsInArray:(NSArray *)otherArray;//impl
- (void)removeObjectsInRange:(NSRange)range;//impl
- (void)replaceObjectsInRange:(NSRange)range withObjectsFromArray:(NSArray *)otherArray range:(NSRange)otherRange;//impl
- (void)replaceObjectsInRange:(NSRange)range withObjectsFromArray:(NSArray *)otherArray;//impl
- (void)setArray:(NSArray *)otherArray;//impl
- (void)sortUsingFunction:(NSInteger (*)(id, id, void *))compare context:(void *)context;
- (void)sortUsingSelector:(SEL)comparator;

- (void)insertObjects:(NSArray *)objects atIndexes:(NSIndexSet *)indexes;
- (void)removeObjectsAtIndexes:(NSIndexSet *)indexes;//impl
- (void)replaceObjectsAtIndexes:(NSIndexSet *)indexes withObjects:(NSArray *)objects;//impl

- (void)setObject:(id)obj atIndexedSubscript:(NSUInteger)idx NS_AVAILABLE(10_8, 6_0);

- (void)sortUsingComparator:(NSComparator)cmptr NS_AVAILABLE(10_6, 4_0);
- (void)sortWithOptions:(NSSortOptions)opts usingComparator:(NSComparator)cmptr NS_AVAILABLE(10_6, 4_0);

@end
