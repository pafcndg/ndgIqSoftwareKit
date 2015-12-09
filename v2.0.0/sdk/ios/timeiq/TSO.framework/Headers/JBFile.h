//
//  JBFile.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/15/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef __JavaBridge__File_h__
#define __JavaBridge__File_h__

#import <Foundation/Foundation.h>
#import "JBFilenameFilter.h"
#import "NSArray+JavaAPI.h"

char File_get_separatorChar_();

@interface File : NSObject
{
@private
    NSString *path;
}

- (instancetype)initWithFile:(File *)dir withNSString:(NSString *)name;

- (instancetype)initWithDirName:(NSString *)dirPath withFileName:(NSString *)fileName;

- (instancetype)initWithNSString:(NSString *)dirPath withNSString:(NSString *)fileName;

- (instancetype)initWithNSString:(NSString *)fileName;

-(Array*)listFilesWithFilenameFilter:(id<FilenameFilter>)filter;

-(long64) lastModified;

-(boolean) exists;

-(boolean) canRead;

-(NSString*) getPath;

-(NSString*) getAbsolutePath;

-(boolean) mkdirs;

-(boolean) mkdir;

-(boolean) createNewFile;

-(boolean) isDirectory;

-(boolean) delete__;

//-(boolean) writeWithNSData:(NSData*)content;

+(File*) createTempFileWithNSString:(NSString*)prefix withNSString:(NSString*)suffix withFile:(File*)directory;

-(Array*) list;

-(NSString*) getName;

-(long64)length;

-(NSString*) rootPath;

+(NSString*)getExternalStoragePath;

@end

#endif