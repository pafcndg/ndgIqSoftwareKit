//
//  NSUUID+JavaAPI.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/18/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__NSUUID_h__
#define __JavaBridge__NSUUID_h__

#import <Foundation/Foundation.h>



@interface UUID : NSObject
{
    NSUUID* ID;
}
+(UUID*) randomUUID;

@end

#endif