//
//  JBBufferedWriter.h
//  TSO
//
//  Created by Bari Levi on 11/10/15.
//  Copyright © 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JBWriter.h"

@interface BufferedWriter : Writer
{
    Writer* out;
    
    NSMutableString* cb;
}
-(instancetype)initWithWriter:(Writer*)outWriter;

-(void) writeWithNSString:(NSString*) buf;

-(void) flush;

-(void) close;

@end
