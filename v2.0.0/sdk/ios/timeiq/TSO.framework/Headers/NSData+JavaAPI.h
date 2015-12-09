//
//  NSData+JavaAPI.h
//  TSO
//
//  Created by Tomer Lekach on 4/29/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"
#import "JBNativeArray.h"

@interface NSMutableData (JavaAPI)

+(ByteArray*) arrayWithLength:(int)size;

+(ByteArray*) arrayWithBytes:(Byte[])arr count:(int)count;

-(int)size;

@end
