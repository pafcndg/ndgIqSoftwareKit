//
//  JBZipOutputStream.h
//  TSO
//
//  Created by AviadX Ganon on 20/10/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBOutputStream.h"
#import "JBZipEntry.h"

@interface ZipOutputStream : OutputStream

-(instancetype)initWithOutputStream:(OutputStream*)os;
-(void)putNextEntryWithZipEntry:(ZipEntry*)ze;
-(void)writeWithByteArray:(NSMutableData*)data withInt:(int)i withInt:(int)count;

@end
