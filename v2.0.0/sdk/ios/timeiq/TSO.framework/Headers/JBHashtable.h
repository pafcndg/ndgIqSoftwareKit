//
//  HashMap.h
//  Java-Bridge
//
//  Created by Bari.Levi on 11/27/14.
//  Copyright (c) 2014 Intel. All rights reserved.
//

#ifndef __JavaBridge__HashMap_h__
#define __JavaBridge__HashMap_h__

#import <Foundation/Foundation.h>
#include "JB.h"
#include "JBHashSet.h"
#include "JBEnumeration.h"

//NSMapTable
//typedef NSMutableDictionary Hashtable;

@interface NSMutableDictionary (JavaAPI)

-(void) putWithId:key withId:val;

@end
@interface Hashtable : NSMutableDictionary
{
@private
    NSMutableDictionary* internalDictionary;
    NSMutableArray* internalValus;
    NSMutableArray* internalKeys;
}

- (instancetype)initWithDictionary:(NSDictionary *)otherDictionary;

- (instancetype)initWithJavaUtilMap:(Hashtable *)otherMap;

- (instancetype)initWithInt:(int)size;

- (id)getWithId:(id)aKey;

- (id)removeWithId:(id)aKey;

- (Set*) values;

-(void) clear;

-(int) size;

-(boolean) containsKeyWithId:(id)key;

-(id) putWithId:key withId:val;

-(Set*) keySet;

-(Set*) entrySet;

-(boolean)isEmpty;

-(void)removeObjectForKey:(id)aKey;

- (void)setObject:(id)anObject forKey:(id <NSCopying>)aKey;

-(id) keyForValue:(id)value;

-(int)count;

- (NSEnumerator *)keyEnumerator;

-(id) objectForKey:(id)aKey;

-(id) objectForKeyedSubscript:(id)key;

-(BOOL)isEqualToDictionary:(NSDictionary*)other;

-(void)putAllWithJavaUtilMap:(Hashtable*)inMap;

-(Enumeration*)elements;

@end

//typedef Hashtable Map;
//typedef Hashtable HashMap;

#define Map Hashtable
#define HashMap Hashtable 

#endif