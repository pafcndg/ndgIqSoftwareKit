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
	 3600, 3635, 3645, 3655, 3665,
	 3675, 3685, 3695, 3705, 3720,
	 3800, 3975, 4061, 4274, 4298,
	 4300, 4302, 4305, 4307, 4309,
	 4311, 4313, 4316, 4318, 4320},
	{
	 3200, 3206, 3226, 3243, 3256,
	 3280, 3313, 3342, 3364, 3398,
	 3436, 3595, 3715, 3828, 3963,
	 3974, 3985, 3997, 4009, 4021,
	 4037, 4052, 4066, 4084, 4151},
	{
	 3200, 3217, 3263, 3334, 3371,
	 3411, 3443, 3467, 3487, 3510,
	 3540, 3654, 3766, 3877, 4022,
	 4032, 4043, 4054, 4067, 4078,
	 4090, 4102, 4116, 4130, 4165},
	{
	 3200, 3260, 3308, 3370, 3419,
	 3461, 3492, 3513, 3526, 3543,
	 3568, 3675, 3785, 3905, 4041,
	 4053, 4064, 4075, 4087, 4098,
	 4109, 4121, 4135, 4149, 4176},
};


