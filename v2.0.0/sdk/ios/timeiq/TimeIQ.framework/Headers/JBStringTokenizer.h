//
//  JBStringTokenizer.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/22/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__StringTokenizer_h__
#define __JavaBridge__StringTokenizer_h__

#import <Foundation/Foundation.h>

@interface StringTokenizer : NSObject
{
    NSString* orignStr;
    
    NSArray* separatedItems;
    
    int     currentIndex;
}
-(instancetype) initWithNSString:(NSString*) str;

-(NSString*) nextToken;

@end

#endif
