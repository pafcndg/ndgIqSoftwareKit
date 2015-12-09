//
//  Array.h
//  Java-Bridge
//
//  Created by Bari.Levi on 11/20/14.
//  Copyright (c) 2014 Intel. All rights reserved.
//

#ifndef JavaBridge_Array_h
#define JavaBridge_Array_h

#import "JBDefines.h"
#import "JBIterator.h"
#import "JBComparator.h"

#define Array_Get(arr,index) [arr getWithInt:index]
#define Array_Set(arr,index, value) [arr replaceObjectAtIndex:index withObject:value]
#define Array_SetAndConsume(arr,index, value) [arr replaceObjectAtIndex:index withObject:value]


//typedef NSMutableArray Array;
//typedef Array Vector;

#define Array NSMutableArray
//#define Vector Array

NSUInteger JreDefaultFastEnumeration(id obj, NSFastEnumerationState* state, __unsafe_unretained id* stackbuf, NSUInteger len);

//void* Array_GetRef(Array* arr, int index);

@interface NSArray (Array)

- (int)size;

- (id)getWithInt:(int)location;

- (id)lastElement;

-(boolean) containsWithId:(id) anObject;

-(boolean) isEmpty;

-(int)indexOfWithId:(id)obj;

-(Array*)toArray;

-(id)getFirst;

@end

@interface NSMutableArray (MutableArray)

- (instancetype) initWithIComparator:(id < IComparator >) comp;

-(boolean) addWithId:(id)object;

- (void)add:(id)anObject;

-(void)removeFirst;

//- (void) addElementWithId:(id)anObject;

- (void) addWithInt:(int)index withId:(id)obj;

//-(bool)addAllWithJavaUtilCollection:(NSArray*)list;

-(void) setWithInt:(int)index withId:(id)obj;

//-(void) insertElementAtWithId:(NSObject*)obj withInt:(int)index;

- (void)clear;

-(boolean) removeWithId:(id)obj;

-(id)removeWithInt:(int)index;

-(instancetype)initWithInt:(int)size;

-(Byte*)mutableBytes;

-(id<Iterator>) iterator;

-(id<Iterator>)listIterator;

-(id<Iterator>)descendingIterator;

+(Array*) arrayWithObjects:(NSArray*)arr count:(int)count type:(Class)c;

-(Array*) initWithCollection:(Array*)arr;

+(Array*) arrayWithLength:(int)size type:(Class)c;

+(Array*) arrayWithLength:(int)size;

-(void) removeAllWithJavaUtilCollection:(Array*)arr;

+(Array*) arrayWithDoubles:(double[])arr count:(int)size;

-(Array*) toArrayWithNSObjectArray:(Array*)arr;

-(id) poll;

-(id) peek;

-(id) remove;

@end


@interface ArrayIterator : NSObject <Iterator>
{
    Array* arrRef;
    Array* arrRefReadOnly;
    int index;
    int lastRet;
}
-(instancetype) initWithArray:(Array*)array;

@end

@interface DescendingIterator : NSObject <Iterator>
{
    Array* arrRef;
    Array* arrRefReadOnly;
    int index;
    int lastRet;
}
-(instancetype) initWithArray:(Array*)array;

@end

#endif
