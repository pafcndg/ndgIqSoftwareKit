//
//  ObjectInputStream.h
//  places-api
//
//  Created by Bari Levi on 2/24/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBInputStream.h"

#import "JBDataInputStream.h"

@interface ObjectInputStream : InputStream
{
    DataInputStream* inputStream;
}
-(instancetype) initWithInputStream:(InputStream*)is;

-(NSObject*) readObject;

-(void) close;

-(NSInputStream*) getNSInputStream;

@end
