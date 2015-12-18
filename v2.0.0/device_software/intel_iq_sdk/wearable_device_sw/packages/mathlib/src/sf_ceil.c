
/* @(#)z_ceilf.c 1.0 98/08/13 */
/*****************************************************************
* ceil
*
* Input:
*   x  - floating point value
*
* Output:
*   Smallest integer greater than x.
*
* Description:
*   This routine returns the smallest integer greater than x.
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

float ceilf(float x)
{
	float f, y;

	y = modff(x, &f);

	if (y == 0.0)
		return (x);
	else if (x > -1.0 && x < 1.0)
		return (x > 0 ? 1.0 : 0.0);
	else
		return (x > 0 ? f + 1.0 : f);
}

#ifdef _DOUBLE_IS_32BITS
double ceil(double x)
{
	return (double)ceilf((float)x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
