//
//  Intent.h
//  Java-Bridge
//
//  Created by Bari Levi on 1/5/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__Intent_h__
#define __JavaBridge__Intent_h__

#import <Foundation/Foundation.h>
#import "JBDefines.h"
#import "JBBundle.h"

@interface Intent: NSObject <NSCopying>
{
    NSString* m_actionId;
    
    Bundle* m_boundle;
}

//-(id) init;

-(id) initWithAction:(NSString*)actionId;

-(void) putExtra:(NSString*)key value:(id<NSCopying>)value;

-(void) putExtra:(NSString*)key ncValue:(id)value;

-(Bundle*) getExtras;

- (id)copyWithZone:(NSZone *)zone;

@end

#endif