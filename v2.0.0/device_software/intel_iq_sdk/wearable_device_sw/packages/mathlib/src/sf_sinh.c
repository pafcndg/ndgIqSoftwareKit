
/* @(#)z_sinhf.c 1.0 98/08/13 */
/******************************************************************
* Hyperbolic Sine
*
* Input:
*   x - floating point value
*
* Output:
*   hyperbolic sine of x
*
* Description:
*   This routine returns the hyperbolic sine of x.
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

float sinhf(float x)
{
	return (sinehf(x, 0));
}

#ifdef _DOUBLE_IS_32BITS

double sinh(double x)
{
	return (double)sinhf((float)x);
}

#endif /* _DOUBLE_IS_32BITS */
