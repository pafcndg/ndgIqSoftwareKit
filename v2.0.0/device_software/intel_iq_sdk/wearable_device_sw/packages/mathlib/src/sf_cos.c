
/* @(#)z_cosf.c 1.0 98/08/13 */
/******************************************************************
* Cosine
*
* Input:
*   x - floating point value
*
* Output:
*   cosine of x
*
* Description:
*   This routine returns the cosine of x.
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

float cosf(float x)
{
	return (sinef(x, 1));
}

#ifdef _DOUBLE_IS_32BITS

double cos(double x)
{
	return (double)sinef((float)x, 1);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
