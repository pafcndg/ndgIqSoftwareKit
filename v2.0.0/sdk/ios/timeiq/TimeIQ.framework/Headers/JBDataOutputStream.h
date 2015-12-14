//
//  JBDataOutputStream.h
//  places-api
//
//  Created by Bari Levi on 3/2/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//


#import <Foundation/Foundation.h>
#include "JBOutputStream.h"

@interface DataOutputStream : OutputStream
{
    OutputStream*   m_stream;
    NSMutableData*  data;

}
-(instancetype)initWithOutputStream:(OutputStream*)os;

// A char value to 1-byte value form; written to the underlying output stream, high byte first write.
- (void)writeChar8:(int8_t)v;
- (void)writeCharWithInt:(int16_t)v;
//A short value to 2-byte value form; written to the underlying output stream, high byte first write.
- (void)writeShort:(int16_t)v;
//A int value to 4-byte value form; written to the underlying output stream, high byte first write.
- (void)writeIntWithInt:(int32_t)v;
//A long value to 8-byte value form; written to the underlying output stream, high byte first write.
- (void)writeLongWithLong:(int64_t)v;
//The machine independent way using a modified version of UTF-8 code writes a string based output stream.
- (void)writeUTFWithNSString:(NSString *)v;
//A NSData byte array is written to the output stream, to write the high byte.
- (void)writeBytes:(NSData *)v;
//The conversion of byte sequences.
//- (NSData *)toByteArray;

- (void) writeWithByteArray:(NSData*)bytes;

@end    