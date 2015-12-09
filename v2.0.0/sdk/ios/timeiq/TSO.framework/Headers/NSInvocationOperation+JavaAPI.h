//
//  NSInvocationOperation+JavaAPI.h
//  TSO
//
//  Created by Gal Shirin on 7/19/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBTimeUnit.h"

@interface NSInvocationOperation (NSInvocationOperation_JavaAPI)

- (id)get;

-(id)getWithLong:(long64)l withTimeUnit:(TimeUnit*)tu;

@end
