//
//  Iterator.h
//  Java-Bridge
//
//  Created by Bari.Levi on 11/19/14.
//  Copyright (c) 2014 Intel. All rights reserved.
//

#ifndef JavaBridge_Iterator_h
#define JavaBridge_Iterator_h

#import <Foundation/Foundation.h>

typedef NSEnumerator Iterable;

@interface NSEnumerator (Enumerator)

- (id)nextElement;

@end

#endif

