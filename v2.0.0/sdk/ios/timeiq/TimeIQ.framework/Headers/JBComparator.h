//
//  JBComparator.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/15/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"

@protocol IComparator <NSObject>

@required - (int)compareWithId:(id)left withId:(id)right;

@required - (BOOL)isEqual:(id)object;

@end
