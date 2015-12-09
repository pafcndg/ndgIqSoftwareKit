//
//  JBFileWriter.h
//  TSO
//
//  Created by Bari Levi on 11/10/15.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBWriter.h"
#import "JBFileOutputStream.h"

@interface FileWriter : Writer
{
    FileOutputStream *fos;
}

- (instancetype)initWithNSString:(NSString*)fileName;

-(void) flush;

-(void) close;

@end
