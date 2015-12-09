//
//  Iterator.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/15/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __ITERATOR__
#define __ITERATOR__

#import <Foundation/Foundation.h>
#import "JBDefines.h"

@protocol Iterator < NSObject >

@required

- (boolean)hasNext;

- (id)next;

- (void)remove;


@end

#endif