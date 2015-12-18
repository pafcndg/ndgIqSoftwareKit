
/* @(#)z_frexpf.c 1.0 98/08/13 */
/******************************************************************
* frexp
*
* Input:
*   d   - floating point value
*   exp - exponent value
*
* Output:
*   A floating point value in the range [0.5, 1).
*
* Description:
*   This routine breaks a floating point value into a number f and
*   an exponent exp such that d = f * 2 ^ exp.
*
*****************************************************************/
/*
* ====================================================
* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
*
* Developed at SunPro, a Sun Microsystems, Inc. business.
* Permission to use, copy, modify, and distribute this
* software is freely granted, provided that this notice
* is preserved.
* ====================================================
*/

#include "newmath.h"
#include "zmath.h"

float frexpf(float d, int *exp)
{
	__int32_t hx, ix;
	GET_FLOAT_WORD(hx, d);
	ix = 0x7fffffff & hx;
	*exp = 0;
	if (!FLT_UWORD_IS_FINITE(ix) || FLT_UWORD_IS_ZERO(ix)) return d;	/* 0,inf,nan */
	if (FLT_UWORD_IS_SUBNORMAL(ix)) {		/* subnormal */
		d *= 3.3554432000e+07;
		GET_FLOAT_WORD(hx,d);
		ix = hx&0x7fffffff;
		*exp = -25;
	}
	*exp += (ix>>23)-126;
	hx = (hx & 0x807fffff) | 0x3f000000;
	SET_FLOAT_WORD(d, hx);
	return d;
}

#ifdef _DOUBLE_IS_32BITS

double frexp(double x, int *exp)
{
	return (double)frexpf((float)x, exp);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
