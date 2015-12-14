//
//  JBMapEntry.h
//  TSO
//
//  Created by Nir Bitton on 8/17/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBArrayList.h"

@interface HashtableEntry : NSObject <JavaUtilMap_Entry>
{
    id key;
    id value;
}

-(instancetype) init;

-(instancetype) initWithValue:(id)val withKey:(id)k;

-(id) getValue;

-(id) getKey;

-(id) setValueWithId:(id)val;

@end
