//
//  JBLocale.h
//  TSO
//
//  Created by Bari Levi on 3/25/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Locale : NSObject
{
@public
    NSLocale* orig;
}

+(Locale*) US;

@end
