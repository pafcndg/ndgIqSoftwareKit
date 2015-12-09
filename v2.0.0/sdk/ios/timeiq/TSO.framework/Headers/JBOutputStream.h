//
//  OutputStream.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/22/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__OutputStream_h__
#define __JavaBridge__OutputStream_h__

#import <Foundation/Foundation.h>

@interface OutputStream: NSObject
{

}
-(instancetype) init;

- (void) writeWithByteArray:(NSData*)bytes;

-(void) flush;

-(void) close;

@end

#endif
