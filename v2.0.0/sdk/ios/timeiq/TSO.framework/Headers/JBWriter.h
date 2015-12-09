//
//  Writer.h
//  places-api
//
//  Created by Bari Levi on 3/1/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "NSMutableString+JavaApi.h"

@interface Writer : NSObject
{
    
}
-(instancetype)init;

-(void) writeWithNSString:(NSString*) buf;

-(void) flush;
    
-(void) close;

@end
