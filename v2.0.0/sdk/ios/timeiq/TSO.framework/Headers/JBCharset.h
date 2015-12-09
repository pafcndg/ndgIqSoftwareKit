//
//  JBCharset.h
//  TSO
//
//  Created by Nir Bitton on 5/11/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NSArray+JavaAPI.h"

@interface Charset : NSObject
{
    NSString* canonicalName;
}

-(instancetype)initWithNSString:(NSString*)str withArray:(Array*)arr;

+(Charset*)forNameWithNSString:(NSString*)str;

-(NSString*)displayName;

@end
