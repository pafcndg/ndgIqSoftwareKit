//
//  JBCopyOnWriteArrayList.h
//  TSO
//
//  Created by AviadX Ganon on 20/10/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "ITSOLoggerPlaces.h"
#import "JBArrayList.h"


@interface CopyOnWriteArrayList : ArrayList
{
//    ArrayList *innerList;
//    NSLock* lock;
}

//-(instancetype)init;

//-(boolean)addWithId:(id<ITSOLoggerPlaces>)logger;

- (NSEnumerator *)objectEnumerator;

- (NSEnumerator *)reverseObjectEnumerator;

-(id<Iterator>)listIterator;

-(id<Iterator>)descendingIterator;


@end
