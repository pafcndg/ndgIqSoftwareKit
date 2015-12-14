//
//  JavaIoFilenameFilter.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/7/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__FilenameFilter_h__
#define __JavaBridge__FilenameFilter_h__

#import <Foundation/Foundation.h>
#import "JBDefines.h"

@class File;

@protocol FilenameFilter <NSObject>

/**
 * Tests if a specified file should be included in a file list.
 *
 * @param   dir    the directory in which the file was found.
 * @param   name   the name of the file.
 * @return  <code>true</code> if and only if the name should be
 * included in the file list; <code>false</code> otherwise.
 */
@required
- (boolean)acceptWithFile:(File *)dir withNSString:(NSString *)filename;


@end

#endif
