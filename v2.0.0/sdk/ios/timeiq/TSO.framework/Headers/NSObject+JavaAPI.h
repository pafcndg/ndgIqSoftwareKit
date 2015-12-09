//
//  NSObject+Object.h
//  Java-Bridge
//
//  Created by Bari.Levi on 12/22/14.
//  Copyright (c) 2014 Intel. All rights reserved.
//

#ifndef __JavaBridge__Object_h__
#define __JavaBridge__Object_h__

#include "JBDefines.h"
#import "Protocol+JavaAPI.h"
#import "JBClass.h"
#import "JBDefines.h"

//@class Hashtable;

@interface NSObject (Object)

-(bool) instanceof:(Class)c;

-(void)copyAllFieldsTo:(NSObject *)other;

-(Class)getClass;

-(NSString*) getSimpleName;

-(NSString*) getName1;

-(void) waitWithLong:(long long)ms withInt:(int)ns;

+(NSMutableDictionary *)objectClassInArray;

-(id)copyWithZone:(NSZone *)zone;

-(void) wait;

-(void)notify;

@end

#endif
