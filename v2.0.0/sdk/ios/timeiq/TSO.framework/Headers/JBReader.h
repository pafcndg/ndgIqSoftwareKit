//
//  JBReader.h
//  places-api
//
//  Created by Bari Levi on 3/2/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NSArray+JavaAPI.h"

@interface Reader : NSObject
{
    
}

-(int) read;

-(int) readWithData:(NSMutableData*) buf withOffset:(int) off withLen:(int)len;

-(boolean) ready;

-(void) reset;

-(void) close;

-(void) skipBytes:(int)n;

@end
