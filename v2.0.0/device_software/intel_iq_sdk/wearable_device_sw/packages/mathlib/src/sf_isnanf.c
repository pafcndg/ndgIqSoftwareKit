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

/*
* __isnanf(x) returns 1 is x is nan, else 0;
*/

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

int isnanf(float x)
{
	__int32_t ix;
	GET_FLOAT_WORD(ix, x);
	ix &= 0x7fffffff;
	return FLT_UWORD_IS_NAN(ix);
}

#ifdef _DOUBLE_IS_32BITS

int
_DEFUN(__isnand, (x),
double x)
{
	return __isnanf((float)x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
