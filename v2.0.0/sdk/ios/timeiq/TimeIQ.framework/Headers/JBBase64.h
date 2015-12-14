//
//  Base64.h
//  TSO
//
//  Created by Bari Levi on 4/1/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NSArray+JavaAPI.h"

@interface Base64 : NSObject

+(NSString*) DEFAULT;

+(NSString*) encodeToStringWithByteArray:(ByteArray*)byteArray withNSString:(NSString*) default2;

+(ByteArray*) decodeWithNSString:(NSString*)serializedObject withNSString:(NSString*) default2;

@end
