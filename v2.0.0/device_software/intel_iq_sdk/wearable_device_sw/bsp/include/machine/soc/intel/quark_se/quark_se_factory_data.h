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

#ifndef __QUARK_SE_FACTORY_DATA_H_
#define __QUARK_SE_FACTORY_DATA_H_

/**
 * @defgroup infra_soc_factory_data Intel&reg; Quark SE&trade; Factory Data
 * Intel&reg; Quark SE&trade; Factory Data API only.
 * @ingroup infra
 * @{
 */

/**
 * oem_data have a reserved soc_data field for SoC specific use
 * For Intel&reg; Quark SE&trade;, soc_data is used to store internal oscillator
 * trim data.
 */

/** Magic string value mandatory in all quark_se_soc_data struct. */
#define FACTORY_QUARK_SE_DATA_MAGIC "$SP!"

/** The version supported by this code. Increment each time the definition of
 * the quark_se_soc_data struct is modified */
#define FACTORY_QUARK_SE_DATA_VERSION 0x01


struct quark_se_factory_data {
    uint8_t magic[4]; /* $SP! */
    uint16_t version; /* 0x01 */
    uint16_t reserved; /* for future use */
    uint16_t osc_trim; /* internal oscillator trim data */
    uint8_t reserved1[6]; /* for future use */
} __packed;


/** @} */

#endif /** __QUARK_SE_FACTORY_DATA_H_ */
