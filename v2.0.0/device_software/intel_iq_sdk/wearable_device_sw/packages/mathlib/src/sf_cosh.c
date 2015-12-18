
/* @(#)z_coshf.c 1.0 98/08/13 */
/******************************************************************
* Hyperbolic Cosine
*
* Input:
*   x - floating point value
*
* Output:
*   hyperbolic cosine of x
*
* Description:
*   This routine returns the hyperbolic cosine of x.
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

float coshf(float x)
{
	return (sinehf(x, 1));
}

#ifdef _DOUBLE_IS_32BITS
double cosh(double x)
{
	return (double)sinehf((float)x, 1);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
