//
//  JBObjectOutputStream.h
//  places-api
//
//  Created by Bari Levi on 2/24/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDataOutputStream.h"

@interface ObjectOutputStream : OutputStream
{
    DataOutputStream*   m_stream;
    NSMutableData*  data;
}

-(instancetype) initWithOutputStream:(OutputStream*)os;

-(void) writeObjectWithId:(NSObject*)obj;

-(void) flush;

-(void) close;

@end
