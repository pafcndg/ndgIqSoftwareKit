
/* @(#)z_numtestf.c 1.0 98/08/13 */
/******************************************************************
* Numtest
*
* Input:
*   x - pointer to a floating point value
*
* Output:
*   An integer that indicates what kind of number was passed in:
*     NUM = 3 - a finite value
*     NAN = 2 - not a number
*     INF = 1 - an infinite value
*           0 - zero
*
* Description:
*   This routine returns an integer that indicates the character-
*   istics of the number that was passed in.
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

int numtestf(float x)
{
	__int32_t wx;
	int exp;

	GET_FLOAT_WORD(wx, x);

	exp = (wx & 0x7f800000) >> 23;

	/* Check for a zero input. */
	if (x == 0.0)
	{
		return (0);
	}

	/* Check for not a number or infinity. */
	if (exp == 0xff)
	{
		if (wx & 0x7fffff)
			return (NAN);
		else
			return (INF);
	}

	/* Otherwise it's a finite value. */
	else
		return (NUM);
}

#ifdef _DOUBLE_IS_32BITS

int numtest(double x)
{
	return numtestf((float)x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
