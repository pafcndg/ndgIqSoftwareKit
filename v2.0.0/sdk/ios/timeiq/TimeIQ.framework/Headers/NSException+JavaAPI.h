//
//  NSException+JavaAPI.h
//  Java-Bridge
//
//  Created by Bari Levi on 2/15/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef _JAVA_API_NSException_H_
#define _JAVA_API_NSException_H_

#import <Foundation/Foundation.h>
#import "JBWriter.h"

//typedef NSException ClassCastException;
//typedef NSException CloneNotSupportedException;
//typedef NSException Throwable;
//typedef NSException Exception;
//typedef NSException ParseException;
//typedef NSException IOException;
//typedef NSException IllegalArgumentException;
//typedef NSException NotSerializableException;
//typedef NSException InterruptedException;
//typedef NSException NullPointerException;
//typedef NSException IndexOutOfBoundsException;
//typedef NSException ClassNotFoundException;
//typedef NSException RuntimeException;
//typedef NSException NumberFormatException;
//typedef NSException AbstractMethodError;
//typedef NSException IllegalArgumentException;
//typedef NSException EndOfStreamException;
//typedef NSException UnsupportedOperationException;
//typedef NSException FileNotFoundException;
//typedef NSException UnsupportedEncodingException;

#define ClassCastException NSException
#define CloneNotSupportedException NSException
#define Throwable NSException
#define Exception NSException
#define ParseException NSException
#define IOException NSException
#define IllegalArgumentException NSException
#define NotSerializableException NSException
#define InterruptedException NSException
#define NullPointerException NSException
#define IndexOutOfBoundsException NSException
#define ClassNotFoundException NSException
#define RuntimeException NSException
#define NumberFormatException NSException
#define AbstractMethodError NSException
#define IllegalArgumentException NSException
#define EndOfStreamException NSException
#define UnsupportedOperationException NSException
#define FileNotFoundException NSException
#define UnsupportedEncodingException NSException
#define IllegalAccessException NSException
#define InstantiationException NSException
#define ExecutionException NSException
#define TimeoutException NSException
#define ExceptionInInitializerError NSException
#define InputMismatchException NSException
#define IllegalStateException NSException
#define AssertionError NSException
#define NoSuchElementException NSException

@interface NSException (ExceptionAPI)

-(instancetype) init;

-(instancetype) initWithId:(NSString*)reasonStr;

-(instancetype) initWithNSString:(NSString*)reasonStr;

-(instancetype) initWithNSString:(NSString*)r withThrowable:(Throwable*)ex;

-(instancetype) initWithThrowable:(Throwable*)ex;

-(NSString*) getMessage;

-(void) printStackTrace;

-(void) printStackTraceWithPrintWriter:(Writer*)writer;

@end

#endif