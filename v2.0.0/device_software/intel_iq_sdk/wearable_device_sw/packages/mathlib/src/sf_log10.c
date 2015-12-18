
/* @(#)z_log10f.c 1.0 98/08/13 */
/******************************************************************
* Logarithm
*
* Input:
*   x - floating point value
*
* Output:
*   logarithm of x
*
* Description:
*   This routine returns the logarithm of x (base 10).
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

float log10f(float x)
{
	return (logarithmf(x, 1));
}

#ifdef _DOUBLE_IS_32BITS

double log10(double x)
{
	return (double)log10f((float)x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
