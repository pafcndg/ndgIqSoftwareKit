//
//  JBEnumSet.h
//  TSO
//
//  Created by Nir Bitton on 7/13/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>

#define EnumSet NSMutableArray

@interface  NSArray (EnumSet_JavaAPI)

-(bool)containsWithId:(id)object;

+(EnumSet*) ofWithEnum:(id)object;

@end
