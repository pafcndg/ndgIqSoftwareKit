//
//  Log.h
//  Java-Bridge
//
//  Created by Bari.Levi on 11/20/14.
//  Copyright (c) 2014 Intel. All rights reserved.
//

#ifndef _Log_h
#define _Log_h

#include "JB.h"

@interface Logger: NSObject

+(Logger*) getLoggerWithNSString:(NSString*)logName;

+(void) d:(NSString*)tag withMessage:(NSString*)message;

+(void) e: (NSString*)tag withMessage:(NSString*)message;

-(void) infoWithNSString:(NSString*)str;

-(void) fineWithNSString:(NSString*)str;

@end

#endif
