
/* @(#)z_atan2f.c 1.0 98/08/13 */
/******************************************************************
* Arctangent2
*
* Input:
*   v, u - floating point values
*
* Output:
*   arctan2 of v / u
*
* Description:
*   This routine returns the arctan2 of v / u.
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

float atan2f(float v, float u)
{
	return (atangentf(0.0, v, u, 1));
}

#ifdef _DOUBLE_IS_32BITS
double atan2(double v, double u)
{
	return (double)atangentf(0.0, (float)v, (float)u, 1);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
