//
//  DataInputStream.h
//
//

#import <Foundation/Foundation.h>
#import "JBInputStream.h"

@interface DataInputStream : InputStream

- (id)initWithInputStream:(InputStream *)is;

/**
 * Skips bytes, block until all bytes are skipped.
 *
 * @param n -  the number of bytes to be skipped 
 */
- (void)skipBytesWithInt:(int)n;

/**
 * Reads a string.
 *
 * @param length - length of string to read
 */
- (NSString *)readUTF;

/** Reads in a boolean. */
- (BOOL)readBoolean;

/** Reads an 8 bit byte. */
- (Byte)readByte;

/** Reads a 8 bit char. */
- (char)readChar8;

/** Reads 16 bit short.  */
- (short)readShort;

/** Reads a 16 bit char. */
- (char)readChar;

/** Reads a 32 bit int. */
- (int)readInt;

/** Reads a 64 bit long. */
- (long64)readLong;
- (long64)readLong64;

/** Reads a 32 bit float. */
- (float)readFloat;

/** Reads a 64 bit double. */
- (double)readDouble;

-(int) readWithByteArray:(ByteArray*)data;

-(NSInputStream*) getNSInputStream;

@end
