//
//  JBRandom.h
//  TSO
//
//  Created by AviadX Ganon on 19/10/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NSException+JavaAPI.h"

@interface Random : NSObject
{
    NSNumber *m_seed;
}

- (instancetype)initWithLong:(long)seed;
- (double)nextDouble;
- (bool)nextBoolean;
- (double)nextIntWithInt:(int)bound;

@end
