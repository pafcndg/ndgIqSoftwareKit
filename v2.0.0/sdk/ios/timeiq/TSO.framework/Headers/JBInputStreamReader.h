//
//  JBInputStreamReader.h
//  places-api
//
//  Created by Bari Levi on 3/1/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"
#include "JBReader.h"
#include "JBInputStream.h"


@interface InputStreamReader : Reader
{
    InputStream* inputStream;
}

-(instancetype) initWithInputStream:(InputStream*)p;

@end
