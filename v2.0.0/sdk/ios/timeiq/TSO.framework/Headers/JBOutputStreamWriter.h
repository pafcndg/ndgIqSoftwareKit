//
//  JBOutputStreamWriter.h
//  places-api
//
//  Created by Nir Bitton on 3/10/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"
#include "JBWriter.h"
#include "JBOutputStream.h"

@interface OutputStreamWriter: Writer
{
    OutputStream* m_outStream;
}

-(instancetype) initWithOutputStream:(OutputStream*)outputStream;

-(void) writeWithNSString:(NSString*)buf;

-(void) flush;

-(void) close;

@end
