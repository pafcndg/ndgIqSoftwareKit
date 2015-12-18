
/* @(#)z_floorf.c 1.0 98/08/13 */
/*****************************************************************
* floor
*
* Input:
*   x  - floating point value
*
* Output:
*   Smallest integer less than x.
*
* Description:
*   This routine returns the smallest integer less than x.
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

float floorf(float x)
{
	float f, y;

	if (x > -1.0 && x < 1.0)
		return (x >= 0 ? 0 : -1.0);

	y = modff(x, &f);

	if (y == 0.0)
		return (x);

	return (x >= 0 ? f : f - 1.0);
}

#ifdef _DOUBLE_IS_32BITS
double floor(double x)
{
	return (double)floorf((float)x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
