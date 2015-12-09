//
//  ReentrantLock.h
//  TSO
//
//  Created by Bari Levi on 6/14/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"

@interface ReentrantLock : NSObject

-(instancetype) initWithBoolean:(boolean)b;

-(void)lock;

-(void)unlock;
@end
