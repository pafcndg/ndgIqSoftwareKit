//
//  JBPriorityQueue.h
//  TSO
//
//  Created by Bari Levi on 7/1/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "JBQueue.h"
#import "JBComparator.h"

@interface PriorityQueue : NSMutableArray
{
    id<IComparator> comparator;
    NSMutableArray *internalData;
}
-(instancetype)initWithInt:(int)size withIComparator:(id<IComparator>)comp;

-(void)removeWithId:(id)o;

-(instancetype)initWithInt:(int)size;

-(instancetype)initWithCollection:(Array*)arr;

-(NSUInteger) count;

-(boolean) addWithId:(id)object;

-(boolean)addAllWithJavaUtilCollection:(NSArray*)list;

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

- (NSArray *)sortedArrayUsingFunction:(NSInteger (*)(id, id, void *))comp context:(void *)context;
- (NSArray *)sortedArrayUsingFunction:(NSInteger (*)(id, id, void *))comp context:(void *)context hint:(NSData *)hint;
- (NSArray *)sortedArrayUsingSelector:(SEL)comp;
- (NSArray *)subarrayWithRange:(NSRange)range;
- (BOOL)writeToFile:(NSString *)path atomically:(BOOL)useAuxiliaryFile;
- (BOOL)writeToURL:(NSURL *)url atomically:(BOOL)atomically;

- (NSArray *)objectsAtIndexes:(NSIndexSet *)indexes;

- (id)objectAtIndexedSubscript:(NSUInteger)idx NS_AVAILABLE(10_8, 6_0);

- (NSArray *)sortedArrayUsingComparator:(NSComparator)cmptr NS_AVAILABLE(10_6, 4_0);
- (NSArray *)sortedArrayWithOptions:(NSSortOptions)opts usingComparator:(NSComparator)cmptr NS_AVAILABLE(10_6, 4_0);
- (NSUInteger)indexOfObject:(id)obj inSortedRange:(NSRange)r options:(NSBinarySearchingOptions)opts usingComparator:(NSComparator)cmp NS_AVAILABLE(10_6, 4_0); // binary search

- (void)addObject:(id)anObject;
- (void)insertObject:(id)anObject atIndex:(NSUInteger)index;
- (void)removeLastObject;//impl
- (void)removeObjectAtIndex:(NSUInteger)index;//impl
- (void)replaceObjectAtIndex:(NSUInteger)index withObject:(id)anObject;

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
- (void)sortUsingSelector:(SEL)comp;

- (void)insertObjects:(NSArray *)objects atIndexes:(NSIndexSet *)indexes;
- (void)removeObjectsAtIndexes:(NSIndexSet *)indexes;//impl
- (void)replaceObjectsAtIndexes:(NSIndexSet *)indexes withObjects:(NSArray *)objects;//impl

- (void)setObject:(id)obj atIndexedSubscript:(NSUInteger)idx NS_AVAILABLE(10_8, 6_0);

- (void)sortUsingComparator:(NSComparator)cmptr NS_AVAILABLE(10_6, 4_0);
- (void)sortWithOptions:(NSSortOptions)opts usingComparator:(NSComparator)cmptr NS_AVAILABLE(10_6, 4_0);
- (boolean) removeAllWithJavaUtilCollection:(NSArray*)collection;

@end
