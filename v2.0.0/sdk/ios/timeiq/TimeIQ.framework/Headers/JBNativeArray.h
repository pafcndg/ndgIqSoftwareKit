//
//  JBNativeArray.h
//  TSO
//
//  Created by Bari Levi on 4/1/15.
//  Copyright (c) 2015 intel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NSNumber+JavaAPI.h"

#define JBDouble_NaN 0

// ----------------- INT -----------------
@interface IOSIntArray: NSObject
{
@public
    int* buffer_;
    int m_size;
}
-(int) size;

+(IOSIntArray*) arrayWithLength:(int)size;

+(IOSIntArray*) arrayWithInts:(int[]) arr count:(int)count;

@end

int IOSIntArray_Get(IOSIntArray* arr, int index);

int* IOSIntArray_GetRef(IOSIntArray* arr, int index);

// ----------------- DOUBLE -----------------
@interface IOSDoubleArray: NSObject
{
@public
    double* buffer_;
    int m_size;
}
+(IOSDoubleArray*) arrayWithLength:(int)size;

+(id)arrayWithDimensions:(int)dimensions lengths:(int*)lengths; 

-(int) size;
@end

double IOSDoubleArray_Get(IOSDoubleArray* arr, int index);

double* IOSDoubleArray_GetRef(IOSDoubleArray* arr, int index);



// ----------------- LONG -----------------
@interface IOSLongArray: NSObject
{
@public
    long64* buffer_;
    int m_size;
}
+(IOSLongArray*) arrayWithLength:(int)size;

-(int) size;

@end

long64 IOSLongArray_Get(IOSLongArray* arr, int index);

long64* IOSLongArray_GetRef(IOSLongArray* arr, int index);


