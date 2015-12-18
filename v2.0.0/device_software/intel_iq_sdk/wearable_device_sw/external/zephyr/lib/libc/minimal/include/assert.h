/**
 assert.h
*/

#ifndef __INC_assert_h__
#define __INC_assert_h__

#include <stdio.h>

/* This prints an "Assertion failed" message and aborts. */
void __assert_fail(/*const char* __assertion, const char* __file, 
    unsigned int __line, const char* __function*/)
    __attribute__ ((__noreturn__)) ;

#undef assert

#ifdef NDEBUG           /* required by ANSI standard */
#define assert(__e) ((void)0)
#else                   /* !NDEBUG */
#define assert(__e) ((__e) ? (void)0 : __assert_fail(/*#__e, __FILE__, __LINE__, __func__*/))
#endif

#endif /* __INC_assert_h__ */
