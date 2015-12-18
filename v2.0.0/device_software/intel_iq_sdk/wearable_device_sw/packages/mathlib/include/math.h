/****************************************************************************************
*
* BSD LICENSE
*
* Copyright(c) 2015 Intel Corporation.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* * Neither the name of Intel Corporation nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************************/

#ifndef __MATH_H__
#define __MATH_H__

#include "newmath.h"

static inline float acos(float x)
{
	return asinef(x, 1);
}

static inline float asin(float x)
{
	return asinef(x, 0);
}

static inline float atan(float x)
{
	return atanf(x);
}

static inline float atan2(float y, float x)
{
	return atan2f(y, x);
}

static inline float cos(float x)
{
	return cosf(x);
}

static inline float cosh(float x)
{
	return coshf(x);
}

static inline float sin(float x)
{
	return sinef(x, 0);
}

static inline float sinh(float x)
{
	return sinhf(x);
}

static inline float tan(float x)
{
	return tanf(x);
}

static inline float tanh(float x)
{
	return tanhf(x);
}

static inline float exp(float x)
{
	return expf(x);
}

static inline float frexp(float x, int *exponent)
{
	return frexpf(x, exponent);
}

static inline float ldexp(float x, int exponent)
{
	return ldexpf(x, exponent);
}

static inline float log(float x)
{
	return logf(x);
}

static inline float log10(float x)
{
	return log10f(x);
}

static inline float modf(float x, float *integer)
{
	return modff(x, integer);
}

static inline float pow(float x, float y)
{
	return powf(x, y);
}

static inline float sqrt(float x)
{
	return __ieee754_sqrtf(x);
}

static inline float ceil(float x)
{
	return ceilf(x);
}

static inline float fabs(float x)
{
	return fabsf(x);
}

static inline float floor(float x)
{
	return floorf(x);
}

static inline float fmod(float x, float y)
{
	return fmodf(x, y);
}

#endif /* __MATH_H__ */
