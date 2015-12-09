//
//  JBEnumerator.h
//  TSO
//
//  Created by Gal Shirin on 9/9/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Enumeration : NSEnumerator

-(instancetype) initWithNSDictionary:(NSDictionary*)dict;

-(instancetype)initWithNSArray:(NSArray*)array;

-(BOOL) hasMoreElements;

-(id) nextElement;

- (id) nextObject;

@end
