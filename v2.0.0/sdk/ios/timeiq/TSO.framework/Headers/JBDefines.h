//
//  JBDefines.h
//  places-api
//
//  Created by Bari Levi on 2/23/15.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#ifndef JavaBridge_JBDefines_h
#define JavaBridge_JBDefines_h

#import <Foundation/Foundation.h>
#import "NSObject+JavaAPI.h"
#import "JBLog.h"
#import "NSLock+JavaAPI.h"

//#define null nil
//#define this self


typedef BOOL boolean;
typedef short char16;
typedef short char16;
typedef long long long64;
typedef long long32;
typedef char char8;

//typedef NSEnumerator Enumerator;
//typedef NSMutableData ByteArray;
//typedef NSMutableData IOSCharArray;
//typedef NSMutableString StringBuffer;
//typedef NSMutableString StringBuilder;
//typedef Class JavaLangReflectType;

#define Enumerator NSEnumerator
#define ByteArray NSMutableData
#define IOSCharArray NSMutableData
#define StringBuffer NSMutableString
#define StringBuilder NSMutableString
#define JavaLangReflectType Class
#define LinkedList JBQueue

#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

#define OBJC_METHOD_FAMILY_NONE
#define J2OBJC_FIELD_SETTER(x,y,z) println("");
//#define J2OBJC_STATIC_FIELD_GETTER(x,y,z) println("");
#define print(x)  NSLog(@"%@",x)
#define println(x)  NSLog(@"%@\n",x)

#define URShift64(n,count) (n>>count)
__attribute__((always_inline)) inline int doubleToInt(double d);
__attribute__((always_inline)) inline long long doubleToLong(double d);
__attribute__((always_inline)) inline unichar doubleToUnichar(double d);

__attribute__ ((unused)) static inline id nil_chk(id __unsafe_unretained p)
{
    NSCAssert(p!=nil, @"NULL POINTER EXCEPTION");
    return p;
}

__attribute__ ((unused)) static inline id check_class_cast(id __unsafe_unretained p, Class clazz);
__attribute__ ((unused)) static inline id check_protocol_cast(id __unsafe_unretained p, Protocol *protocol);

NSString* Strcat(const char *pTypes, ...);

#define J2OBJC_STATIC_INIT(CLASS) \
__attribute__((always_inline)) inline void CLASS##_init() { \
if (!__builtin_expect(CLASS##_initialized, YES)) { \
[CLASS class]; \
} \
}
/*!
 * Defines the getter for a static variable. For class "Foo" and field "bar_"
 * with type "int" the getter will have the following signature:
 *   inline int Foo_get_bar_();
 *
 * @define J2OBJC_STATIC_FIELD_GETTER
 * @param CLASS The class containing the static variable.
 * @param FIELD The name of the static variable.
 * @param TYPE The type of the static variable.
 */
#define J2OBJC_STATIC_FIELD_GETTER(CLASS, FIELD, TYPE) \
__attribute__((always_inline)) inline TYPE CLASS##_get_##FIELD() { \
CLASS##_init(); \
return CLASS##_##FIELD; \
}

//#define NIL_CHECK(type,p) ((type *) nil_chk(p))
#import <Foundation/Foundation.h>

__attribute__((always_inline)) inline int doubleToInt(double d)
{
    int tmp = [[NSNumber numberWithDouble:d] intValue];
    return tmp;
//    return tmp == (int) 0x80000000 ? (d >= 0 ? 0x7FFFFFFF : tmp) : tmp;
}
__attribute__((always_inline)) inline long long doubleToLong(double d)
{
    long long tmp = [[NSNumber numberWithDouble:d] longLongValue];
    return tmp;
//    long long tmp = (long long) d;
//    return (unsigned long long) tmp == 0x8000000000000000LL ?
//    (d >= 0 ? 0x7FFFFFFFFFFFFFFFL : tmp) : tmp;
}
__attribute__((always_inline)) inline unichar doubleToUnichar(double d)
{
    unichar tmp = [[NSNumber numberWithDouble:d] unsignedCharValue];
    return tmp;

//    unsigned tmp = (unsigned) d;
//    return tmp > 0xFFFF || (tmp == 0 && d > 0) ? 0xFFFF : (unichar) tmp;
}
//TODO: bari impl
// Marked as unused to avoid a clang warning when this file is included
// but NIL_CHK isn't used.
//__attribute__ ((unused)) static inline id nil_chk(id __unsafe_unretained p) ;

// Separate methods for class and protocol cast checks are used to reduce
// overhead, since the difference is statically known.
__attribute__ ((unused)) static inline id check_class_cast(id __unsafe_unretained p, Class clazz) {
    //#if !defined(J2OBJC_DISABLE_CAST_CHECKS)
    //    return (!p || [p isKindOfClass:clazz]) ? p : [NSObject throwClassCastException];
    //#else
    //    return p;
    //#endif
    return p;
    
}

__attribute__ ((unused)) static inline id check_protocol_cast(id __unsafe_unretained p,
                                                              Protocol *protocol) {
    //#if !defined(J2OBJC_DISABLE_CAST_CHECKS)
    //    return (!p || [p conformsToProtocol:protocol]) ? p : [NSObject throwClassCastException];
    //#else
    //    return p;
    //#endif
    return p;
    
}
#define ABSTRACT_METHOD \
 [NSException raise:NSInternalInconsistencyException \
format:@"You must override method %@:%@:%d",  NSStringFromClass ([self class]), NSStringFromSelector(_cmd), __LINE__]

#define DEAD_CODE \
[NSException raise:NSInternalInconsistencyException \
format:@"This is dead code  %@:%@:%d",  NSStringFromClass ([self class]), NSStringFromSelector(_cmd), __LINE__]

#define NO_WAY_CODE \
[NSException raise:NSInternalInconsistencyException \
format:@"Not supposed to get here %@:%@:%d",  NSStringFromClass ([self class]), NSStringFromSelector(_cmd), __LINE__]

#define ERROR_CODE \
[NSException raise:NSInternalInconsistencyException \
format:@"Arriving this codes is not reasonable %@:%@:%d",  NSStringFromClass ([self class]), NSStringFromSelector(_cmd), __LINE__]

#define NOT_SUPPORTED_CODE \
[NSException raise:NSInternalInconsistencyException \
format:@"Arriving this codes is not supported case in IOS %@:%@:%d",  NSStringFromClass ([self class]), NSStringFromSelector(_cmd), __LINE__]


//__PRETTY_FUNCTION__

#endif
