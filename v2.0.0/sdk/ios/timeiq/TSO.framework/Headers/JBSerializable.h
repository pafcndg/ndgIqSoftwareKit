//
//  JBSerializable.h
//  TSO
//
//  Created by Tomer Lekach on 4/30/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBHashtable.h"

#import "JB.h"

@protocol Serializable <NSObject>

- (void)initObjectFromMapWithJavaUtilMap:(HashMap*)map;

- (HashMap*)objectToMap;


@end
