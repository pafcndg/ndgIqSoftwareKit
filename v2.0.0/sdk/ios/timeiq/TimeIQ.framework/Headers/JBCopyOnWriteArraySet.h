//
//  JBCopyOnWriteArraySet.h
//  TSO
//
//  Created by Bari Levi on 5/18/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBHashSet.h"
#import "JBArrayList.h"

@interface CopyOnWriteArraySet : Set
//{
//    ArrayList *innerSet;
//}

- (NSEnumerator *)objectEnumerator;

- (NSEnumerator *)reverseObjectEnumerator;

-(id<Iterator>)listIterator;

-(id<Iterator>)descendingIterator;

//-(instancetype)init;
//-(void)addWithId:(id)i;
//-(void)addAllWithJavaUtilCollection:(HashSet*)col;
//-(void)removeWithId:(id)i;
//-(void)clear;
//-(int)size;
//-(id)objectAtIndex:(NSUInteger)index;

@end
