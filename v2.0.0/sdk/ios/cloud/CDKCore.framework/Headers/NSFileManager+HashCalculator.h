//
//  NSFileManager+HashCalculator.h
//  CloudSDK
//
//  Created by RaghavendraX Gutta on 12/10/15.
//  Copyright © 2015 Intel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSFileManager (HashCalculator)

-(NSString*)computeMessageDigestOn:(NSURL*)filePathUrl;

@end
