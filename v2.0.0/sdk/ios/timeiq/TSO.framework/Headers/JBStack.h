//
//  JBStack.h
//  TSO
//
//  Created by Bari Levi on 6/14/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Stack : NSObject{
    NSMutableArray *contents;
}

- (void)push:(id)object;
- (id)pop;


@end
