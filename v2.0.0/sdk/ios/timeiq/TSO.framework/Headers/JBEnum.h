//
//  Enum.h
//  Java-Bridge
//
//  Created by Bari Levi on 1/19/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Enum_h__
#define __JavaBridge__Enum_h__

#import "JBDefines.h"

@interface Enum : NSObject < NSCopying , NSCoding>
{
@public
    NSString* name;
    UInt8 value;
}

- (instancetype)initWithNSString:(NSString*)_name
                        withInt:(UInt8)_ordinal;

- (void)encodeWithCoder:(NSCoder *)aCoder;

- (id)initWithCoder:(NSCoder *)aDecoder;

- (id)copyWithZone:(NSZone *)zone;

- (NSString*) description;

+(Enum*) valueOfWithClass:clazz withNSString:(NSString *) name;

+ (Enum*) valueOfString:(NSString*)name withTypes:(NSArray*)types;

+ (Enum*) valueOfInt:(int)val withTypes:(NSArray*)types;

-(boolean) isEqual:(id)object;

-(int) ordinal;

-(int) getVal;

-(int) getId;//for customize using

-(NSString*) name;

-(Class)getPlaceTypeClass;

-(Class) getDeclaringClass;

@end

#endif