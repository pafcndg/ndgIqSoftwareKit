/*
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "util/cunit_test.h"
#include "math.h"

/* This is needed by eventual math function usage */
int __errno;

#define PI   3.14159265358979323846

static volatile float acos_table[] = { 3.141593,
2.498091,
2.214298,
1.982313,
1.772154,
1.570796,
1.369438,
1.159279,
0.927295,
0.643501,
0.000000 };

static volatile float asin_table[] = { -1.570796,
-0.927295,
-0.643501,
-0.411517,
-0.201358,
0.000000,
0.201358,
0.411517,
0.643501,
0.927295,
1.570796 };

static volatile float atan_table[] = { -1.560797,
-1.558297,
-1.554131,
-1.545802,
-1.520838,
0.000000,
1.520838,
1.545802,
1.554131,
1.558297,
1.560797 };

static volatile float atan2_table[] = { -2.356194,
-2.158799,
-1.892547,
-1.570796,
-1.249046,
-0.982794,
-0.785398,
-2.553590,
-2.356194,
-2.034444,
-1.570796,
-1.107149,
-0.785398,
-0.588003,
-2.819842,
-2.677945,
-2.356194,
-1.570796,
-0.785398,
-0.463648,
-0.321751,
3.141593,
3.141593,
3.141593,
0.000000,
0.000000,
0.000000,
0.000000,
2.819842,
2.677945,
2.356194,
1.570796,
0.785398,
0.463648,
0.321751,
2.553590,
2.356194,
2.034444,
1.570796,
1.107149,
0.785398,
0.588003,
2.356194,
2.158799,
1.892547,
1.570796,
1.249046,
0.982794,
0.785398 };

static volatile float cos_table[] = { -1.000000,
-0.866025,
-0.500000,
-0.000000,
0.500000,
0.866025,
1.000000,
0.866025,
0.500000,
-0.000000,
-0.500000,
-0.866025,
-1.000000 };

static volatile float cosh_table[] = { 74.209946,
27.308233,
10.067662,
3.762196,
1.543081,
1.000000,
1.543081,
3.762196,
10.067662,
27.308233,
74.209946 };

static volatile float sin_table[] = { 0.000000,
-0.500000,
-0.866025,
-1.000000,
-0.866025,
-0.500000,
0.000000,
0.500000,
0.866025,
1.000000,
0.866025,
0.500000,
-0.000000 };

static volatile float sinh_table[] = { -74.203209,
-27.289917,
-10.017875,
-3.626860,
-1.175201,
0.000000,
1.175201,
3.626860,
10.017875,
27.289917,
74.203209 };

static volatile float tan_table[] = { -0.000000,
0.577350,
1.732051,
22877332.000000,
-1.732051,
-0.577350,
0.000000,
0.577350,
1.732051,
-22877332.000000,
-1.732051,
-0.577350,
0.000000 };

static volatile float tanh_table[] = { -0.999909,
-0.999329,
-0.995055,
-0.964028,
-0.761594,
0.000000,
0.761594,
0.964028,
0.995055,
0.999329,
0.999909 };

static volatile float exp_table[] = { 0.006738,
0.018316,
0.049787,
0.135335,
0.367879,
1.000000,
2.718282,
7.389056,
20.085537,
54.598148,
148.413162 };

static volatile float frexp_table[6][2] = { { -0.787500, 4 },
{ -0.575000, 4 },
{ -0.725000, 3 },
{ -0.600000, 2 },
{ 0.500000, 1 },
{ 0.550000, 3 } };

static volatile float ldexp_table[] = { -3.150000,
-4.600000,
-5.800000,
-4.800000,
4.000000,
35.200001 };

static volatile float log_table[] = { -0.693147,
-0.105361,
0.262364,
0.530628,
0.741937,
0.916291,
1.064711,
1.193922,
1.308333,
1.410987,
1.504077 };

static volatile float log10_table[] = { -0.301030,
-0.045758,
0.113943,
0.230449,
0.322219,
0.397940,
0.462398,
0.518514,
0.568202,
0.612784,
0.653212 };

static volatile float modf_table[6][2] = { { -0.600000, -12.000000 },
{ -0.200000, -9.000000 },
{ -0.800000, -5.000000 },
{ -0.400000, -2.000000 },
{ 0.000000, 1.000000 },
{ 0.400000, 4.000000 } };

static volatile float pow_table[] = { -12.600000,
158.760010,
-2000.376221,
-9.200000,
84.639999,
-778.687927,
-5.800000,
33.640003,
-195.112015,
-2.400000,
5.760000,
-13.824001,
1.000000,
1.000000,
1.000000,
0.033118,
0.743549,
16.693987 };

static volatile float sqrt_table[] = { 0.707107,
0.948683,
1.140175,
1.303841,
1.449138,
1.581139,
1.702939,
1.816590,
1.923538,
2.024846,
2.121320 };

static volatile float ceil_table[] = { -12.000000,
-9.000000,
-5.000000,
-2.000000,
1.000000,
5.000000 };

static volatile float fabs_table[] = { 12.600000,
9.200000,
5.800000,
2.400000,
1.000000,
4.400000 };

static volatile float floor_table[] = { -13.000000,
-10.000000,
-6.000000,
-3.000000,
1.000000,
4.000000 };

static volatile float fmod_table[] = { -0.200000,
-0.120001,
-0.640000,
-0.400000,
-0.100000,
-0.920000,
-0.200000,
-0.080000,
-0.280000,
-0.000000,
-0.060000,
-0.560000,
0.200000,
0.220000,
0.080000,
0.000000,
0.240000,
0.720000 };

int resultcheck(float output, float truevalue)
{
	float relative_error;

	if (truevalue == 0)
		relative_error = output - truevalue;
	else
		relative_error = (output - truevalue) / truevalue;

	if (relative_error < 0.001 && relative_error > -0.001) return 1;
	else return 0;
}

void lib_math_test()
{
	cu_print("##############################################################\n");
	cu_print("# Purpose of the lib math test:                               #\n");
	cu_print("# Check that all implemented functions in math.h return the   #\n");
	cu_print("# right results for a set of input values                     #\n");
	cu_print("##############################################################\n");

	int i;
	float input, result;

	/* arccos */
	for (i = 0; i <= 10; i++)
	{
		input = -1 + i * 0.2;
		result = acos(input);
		CU_ASSERT("acos", resultcheck(result, acos_table[i]));
	}

	/* arcsin */
	for (i = 0; i <= 10; i++)
	{
		input = -1 + i * 0.2;
		result = asin(input);
		CU_ASSERT("asin", resultcheck(result, asin_table[i]));
	}

	/* arctan */
	for (i = 0; i <= 10; i++)
	{
		input = -100 + i * 20;
		result = atan(input);
		CU_ASSERT("atan", resultcheck(result, atan_table[i]));
	}

	float input2;
	int j;
	/* arctan2 */
	for (i = 0; i <= 6; i++)
	{
		input = -90 + i * 30;
		for (j = 0; j <= 6; j++)
		{
			if (i == 3 && j == 3)
				continue;
			input2 = -90 + j * 30;
			result = atan2(input, input2);
			CU_ASSERT("atan2", resultcheck(result, atan2_table[7 * i + j]));
		}
	}

	/* cos */
	for (i = 0; i <= 12; i++)
	{
		input = -PI + i * PI / 6;
		result = cos(input);
		CU_ASSERT("cos", resultcheck(result, cos_table[i]));
	}

	/* cosh */
	for (i = 0; i <= 10; i++)
	{
		input = -5 + i;
		result = cosh(input);
		CU_ASSERT("cosh", resultcheck(result, cosh_table[i]));
	}

	/* sin */
	for (i = 0; i <= 12; i++)
	{
		input = -PI + i * PI / 6;
		result = sin(input);
		CU_ASSERT("sin", resultcheck(result, sin_table[i]));
	}

	/* sinh */
	for (i = 0; i <= 10; i++)
	{
		input = -5 + i;
		result = sinh(input);
		CU_ASSERT("sinh", resultcheck(result, sinh_table[i]));
	}

	/* tan */
	for (i = 0; i <= 12; i++)
	{
		input = -PI + i * PI / 6;
		result = tan(input);
		CU_ASSERT("tan", resultcheck(result, tan_table[i]));
	}

	/* tanh */
	for (i = 0; i <= 10; i++)
	{
		input = -5 + i;
		result = tanh(input);
		CU_ASSERT("tanh", resultcheck(result, tanh_table[i]));
	}

	/* exp */
	for (i = 0; i <= 10; i++)
	{
		input = -5 + i;
		result = exp(input);
		CU_ASSERT("exp", resultcheck(result, exp_table[i]));
	}

	int exponent;

	/* frexp */
	for (i = 0; i <= 5; i++)
	{
		input = -12.6 + i * 3.4;
		result = frexp(input, &exponent);
		CU_ASSERT("frexp", resultcheck(result, frexp_table[i][0]) ||
			  resultcheck(exponent, frexp_table[i][1]));
	}

	/* ldexp */
	for (i = 0; i <= 5; i++)
	{
		input = -12.6 + i * 3.4;
		exponent = -2 + i;
		result = ldexp(input, exponent);
		CU_ASSERT("ldexp", resultcheck(result, ldexp_table[i]));
	}

	/* log */
	for (i = 0; i <= 10; i++)
	{
		input = 0.5 + i * 0.4;
		result = log(input);
		CU_ASSERT("log", resultcheck(result, log_table[i]));
	}

	/* log10 */
	for (i = 0; i <= 10; i++)
	{
		input = 0.5 + i * 0.4;
		result = log10(input);
		CU_ASSERT("log10", resultcheck(result, log10_table[i]));
	}

	float integer;
	/* modf */
	for (i = 0; i <= 5; i++)
	{
		input = -12.6 + i * 3.4;
		result = modf(input, &integer);
		CU_ASSERT("modf", resultcheck(result, modf_table[i][0]) ||
			  resultcheck(integer, modf_table[i][1]));
	}

	/* pow */
	for (i = 0; i <= 5; i++)
	{
		input = -12.6 + i * 3.4;
		for (j = 0; j <= 2; j++)
		{
			if (input < 0)
				input2 = 1 + j;
			else
				input2 = -2.3 + j * 2.1;

			result = pow(input, input2);
			CU_ASSERT("pow", resultcheck(result, pow_table[3 * i + j]));
		}
	}

	/* square root */
	for (i = 0; i <= 10; i++)
	{
		input = 0.5 + i * 0.4;
		result = sqrt(input);
		CU_ASSERT("sqrt", resultcheck(result, sqrt_table[i]));
	}

	/* ceil */
	for (i = 0; i <= 5; i++)
	{
		input = -12.6 + i * 3.4;
		result = ceil(input);
		CU_ASSERT("ceil", resultcheck(result, ceil_table[i]));
	}

	/* fabs */
	for (i = 0; i <= 5; i++)
	{
		input = -12.6 + i * 3.4;
		result = fabs(input);
		CU_ASSERT("fabs", resultcheck(result, fabs_table[i]));
	}

	/* floor */
	for (i = 0; i <= 5; i++)
	{
		input = -12.6 + i * 3.4;
		result = floor(input);
		CU_ASSERT("floor", resultcheck(result, floor_table[i]));
	}

	/* fmod */
	for (i = 0; i <= 5; i++)
	{
		input = -12.6 + i * 3.4;
		for (j = 0; j <= 2; j++)
		{
			input2 = -0.4 + j * 0.66;
			result = fmod(input, input2);
			CU_ASSERT("fmod", resultcheck(result, fmod_table[3 * i + j]));
		}
	}

}
