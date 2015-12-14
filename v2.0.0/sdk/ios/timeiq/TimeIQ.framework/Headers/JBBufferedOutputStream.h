//
//  JBBufferedOutputStream.h
//  TSO
//
//  Created by AviadX Ganon on 20/10/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBOutputStream.h"

@interface BufferedOutputStream : OutputStream

-(instancetype)initWithOutputStream:(OutputStream*)os;

- (void) writeWithByteArray:(NSData*)bytes;

-(void) flush;

-(void) close;


@end
