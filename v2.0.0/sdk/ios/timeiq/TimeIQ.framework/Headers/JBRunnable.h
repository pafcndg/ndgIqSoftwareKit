//
//  JBRunnable.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/21/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Runnable_h__
#define __JavaBridge__Runnable_h__

#import <Foundation/Foundation.h>

@protocol Runnable <NSObject>

@required

- (void)run;

@optional

-(id) call;

@end
#endif
