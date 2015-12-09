//
//  JBClass.h
//  TSO
//
//  Created by Bari Levi on 3/18/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "NSObject+JavaAPI.h"
#include "NSException+JavaAPI.h"

#define newInstance new

@interface IOSClass : NSObject

+(Protocol*) classWithProtocol:(Protocol*)protocolType;

+(Class) forName:(NSString*)className;

+(id)newInstance:(Class)clazz;

@end
