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

#include "os/os.h"

#include "services/em_service/battery_LUT.h"

/*
 * @brief Completion of 4 Lookup table :
 * dflt_lookup_tables[0] corresponding to a period of charge
 * dflt_lookup_tables[1] corresponding to Temperature below +6C
 * dflt_lookup_tables[2] corresponding to Temperature between +7C and +12C
 * dflt_lookup_tables[3] corresponding to Temperature above 20C
 *
 * @remark for each lookup table :
 *  - voltage measured @ index ∈ [0;9] are respectively corresponding to fuel gauge ∈ [0;9] %
 *  - voltage measured @ index ∈ [15;24] are respectively corresponding to fuel gauge ∈ [91;100] %
 *  - if voltage measured is between :
 *      - voltage corresponding to the one for 10% and 30%
 *      - voltage corresponding to the one for 31% and 50%
 *      - voltage corresponding to the one for 51% and 70%
 *      - voltage corresponding to the one for 71% and 90%
 *      A linearization is realized
 */
const uint16_t dflt_lookup_tables[BATTPROP_LOOKUP_TABLE_COUNT][BATTPROP_LOOKUP_TABLE_SIZE] = {
	{
	 3350, 3421, 3513, 3575, 3620,
	 3661, 3692, 3718, 3729, 3734,
	 3739, 3836, 3902, 4036, 4226,
	 4237, 4248, 4258, 4269, 4281,
	 4292, 4304, 4315, 4327, 4330},
	{
	 3200, 3235, 3252, 3264, 3276,
	 3294, 3317, 3342, 3363, 3385,
	 3402, 3550, 3672, 3809, 4021,
	 4032, 4043, 4056, 4069, 4083,
	 4100, 4119, 4135, 4165, 4223},
	{
	 3200, 3253, 3285, 3330, 3359,
	 3383, 3409, 3432, 3456, 3477,
	 3496, 3629, 3723, 3858, 4080,
	 4090, 4101, 4113, 4127, 4140,
	 4153, 4169, 4185, 4211, 4237},
	{
	 3200, 3276, 3330, 3373, 3407,
	 3433, 3458, 3478, 3495, 3510,
	 3524, 3650, 3742, 3886, 4099,
	 4111, 4122, 4134, 4147, 4160,
	 4172, 4188, 4204, 4230, 4248},
};

