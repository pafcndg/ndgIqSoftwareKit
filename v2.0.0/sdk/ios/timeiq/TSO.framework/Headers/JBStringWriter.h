//
//  JBStringWriter.h
//  places-api
//
//  Created by Bari Levi on 3/1/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"
#import "JBWriter.h"

@interface StringWriter : Writer
{
    StringBuffer* buf;
}

-(void) writeWithChar:(char) c;

-(void) writeWithNSString:(NSString*) str;

-(void) flush;

-(void) close;
@end
