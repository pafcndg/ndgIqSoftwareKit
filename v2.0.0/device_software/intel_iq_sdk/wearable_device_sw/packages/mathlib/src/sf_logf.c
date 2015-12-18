
/* @(#)z_logf.c 1.0 98/08/13 */
/******************************************************************
* Logarithm
*
* Input:
*   x - floating point value
*
* Output:
*   natural logarithm of x
*
* Description:
*   This routine returns the natural logarithm of x.
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

float logf(float x)
{
	return (logarithmf(x, 0));
}

#ifdef _DOUBLE_IS_32BITS

double log(double x)
{
	return (double)logf((float)x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
