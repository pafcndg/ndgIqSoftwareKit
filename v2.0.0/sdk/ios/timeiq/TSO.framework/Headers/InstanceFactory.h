//
//  InstanceFactory.h
//  TSO
//
//  Created by Vladimir Kofman on 09/12/2015.
//  Copyright © 2015 intel. All rights reserved.
//

#ifndef _InstanceFactory_H_
#define _InstanceFactory_H_

#import "JBClass.h"

#import "JB.h"
#include "AObjectFactory.h"

@interface InstanceFactory : AObjectFactory

-(instancetype)initWithInstance:(id)instance;

@end

#endif // _InstanceFactory_H_