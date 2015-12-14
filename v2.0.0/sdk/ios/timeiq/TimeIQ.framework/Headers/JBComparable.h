//
//  JBCompareable.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/15/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__IComparable_h__
#define __JavaBridge__IComparable_h__

#import <Foundation/Foundation.h>

@protocol IComparable <NSObject>

@required - (int)compareToWithId:(id)another;

@end

#endif