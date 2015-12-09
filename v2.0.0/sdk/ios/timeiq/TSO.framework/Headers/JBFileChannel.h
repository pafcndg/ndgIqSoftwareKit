//
//  JBFileChannel.h
//  TSO
//
//  Created by AviadX Ganon on 20/10/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBDefines.h"

@interface FileChannel : NSObject
{
    NSString *path;
}

-(instancetype)initWithPath:(NSString *)inPath;

-(long64)size;

@end
