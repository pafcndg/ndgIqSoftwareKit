/*
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __COMPILER_H
#error "Please don't include compiler-gcc.h directly, include compiler.h instead."
#endif

/*--------------- GCC specific builtins ---------------*/
#define HAVE_SAME_TYPE(a,b) __builtin_types_compatible_p(__typeof__(a),__typeof__(b))

/*--------------- Function inlining control ---------------*/

/* noinline is to instruct the compiler to never inline the requested function.
  Syntax : noinline int foo(int a) { ... } */
#ifndef noinline
#define noinline         __attribute__((noinline))
#endif

/* __naked should be used to implement the body of an assembly function, while allowing the compiler
   to construct the requisite function declaration for the assembler.
  Syntax: __naked into foo() { ... }
 */
#ifndef __naked
#define __naked         __attribute__((naked))
#endif

/*--------------- Function and variable control ---------------*/

/* __maybe_unused is for variables, for function arguments and for functions
   which may or may not be used depending on macro preprocessing.
   This macro can be used to safely fix warning of type "parameter unused".
  Syntax :
   - for fct parameter : int foo(int __maybe_unused a) { ... }
   - for function : int __maybe_unused foo(int a);
   - for variable : int foo(int a) { int __maybe_unused b; ....}
   - for pointer  : int foo(int a) { int __maybe_unused * b; ....} */
#ifndef __maybe_unused
#define __maybe_unused     __attribute__((unused))
#endif

/* __unused is for variables, for function arguments and for functions
   which are not used depending on macro preprocessing.
   This macro can be used to safely fix warning of type "parameter unused".
  Syntax :
   - for fct parameter : int foo(int __unused a) { ... }
   - for function : int __unused foo(int a);
   - for variable : int foo(int a) { int __unused b; ....}
   - for pointer  : int foo(int a) { int __unused * b; ....} */
#ifndef __unused
#define __unused    __attribute__((__unused__))
#endif

/* __used attached to a function indicates that this function attribute informs the compiler that
 * a static function is to be retained in the object file, even if it is unreferenced.
   Syntax : static int keep_this(int) __attribute__((used));     // retained in object file
 */
#ifndef __used
#define __used			__attribute__((__used__))
#endif


/*--------------- Function general control ---------------*/

/* __deprecated is to inform that the corresponding function is now deprecated.
   This is the only warning that will not break a build.
  Syntax : int __deprecated foo(int a); */
#ifndef __deprecated
#define __deprecated        __attribute__((deprecated))
#endif

/* __noreturn is to inform that the function is never returning.
  Syntax  : int __noreturn foo(int a); */
#ifndef __noreturn
#define __noreturn          __attribute__((noreturn))
#endif

/* __alias is to inform that this function attribute enables you to specify multiple aliases
   for functions
  Syntax  : void bar(void) __attribute__((alias("foo"))); */
#ifndef __alias
#define __alias(symbol) __attribute__((alias(#symbol)))
#endif

/*  __attribute_const attached to a function indicates that function is not allowed to read global
 *  memory.
   Syntax: int Function_Attributes_const_0(int b) __attribute__ ((const));
 */
#ifndef __attribute_const
#define __attribute_const __attribute__((__const__))
#endif

/*--------------- Code and variable relocation ---------------*/

/* DATA_SECTION is to place the corresponding variable into a specific
   section that will be placed later on by the linker file.
  Syntax : int DATA_SECTION(section_name) a; */
#define DATA_SECTION(x)    __attribute__((section((#x))))

/* CODE_SECTION is the doing the same as DATA_SECTION but for function.
  Syntax : int CODE_SECTION(section_name) foo(int a); */
#define CODE_SECTION(x)    __attribute__((section((#x))))

/* __aligned is to specify a particular byte alignement for the specified variable.
  Syntax : int __aligned(128) a; */
#ifndef __aligned
#define __aligned(x)    __attribute__((aligned(x)))
#endif

/* PACKED attached to struct or union type definition, specifies that each member (other than
 * zero-width bit-fields) of the structure or union is placed to minimize the memory required.
 * When attached to an enum definition, it indicates that the smallest integral type should be
 * used.
 */
#ifndef __packed
#define __packed        __attribute__((__packed__))
#endif

/* __weak attached to a function indicates that the function definition can be overridden by another
 * definition or can be left undefined.
 */
#ifndef __weak
#define __weak __attribute__((weak))
#endif

/* __section attribute enables code to be placed in different sections of the image.
  Syntax: void foobar (void) __attribute__ ((section ("bar"))); */
#ifndef __section
# define __section(x) __attribute__ ((__section__(x)))
#endif

/*--------------- Optimization barrier ---------------*/
/* BARRIER defines a memory barrier.
   The "volatile" is due to gcc bugs. */
#define BARRIER() __asm__ __volatile__("": : :"memory")

/* __always_inline indicates that a function must be inlined.
  Syntax: int max(int x, int y) __attribute__((always_inline)); */
#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif

/*----------- Miscellaneous -------------------------------*/
#define __printf(a, b)	__attribute__((format(printf, a, b)))

/* __visibility function attribute affects the visibility of ELF symbols
  Syntax: int foo  __attribute__((visibility("default"))) ;
 */
#define __visibility(x)	__attribute__((visibility(#x)))

#ifdef CONFIG_ARC
#define _Usually(x) __builtin_expect(!!((x)), 1)
#define _Rarely(x) __builtin_expect(!!((x)), 0)
#define _sr(_src_, _reg_) __builtin_arc_sr((unsigned int)_src_, _reg_)
#define _lr(_reg_) __builtin_arc_lr(_reg_)
#define _nop()
#endif
