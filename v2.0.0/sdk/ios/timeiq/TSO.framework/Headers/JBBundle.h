//
//  Boundle.h
//  Java-Bridge
//
//  Created by Bari Levi on 1/5/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Bundle_h__
#define __JavaBridge__Bundle_h__

#import <Foundation/Foundation.h>

@interface Bundle: NSObject
{
    NSMutableDictionary* m_data;
}

-(id) init;

-(void) putWithId:(id) id_ withData:(id<NSCopying>)data;

- (id)getWithId:(id)aKey;

-(id) getString:(NSString*)key;

@end

#endif