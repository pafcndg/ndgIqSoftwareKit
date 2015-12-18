/*******************************************************************************
 *
 * BSD LICENSE
 *
 * Copyright(c) 2015 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "drivers/ss_adc.h"
#include "machine.h"
#include "util/cunit_test.h"

#define TEST_CLK_RATIO      1024
#define TEST_DATA_LEN       1
#define TEST_DLY            50
#define TEST_RESOLUTION     12
#define TEST_CHANNEL        10
#define REPETITIVE_ENTRIES  4
#define TEST_TIMEOUT        0xFFFFF /* TODO make timeout more precise */
#define NUM_ADC_CHANNELS    19
#define TEST_ADC_DEV        SS_ADC_ID

/*  No special HW setup is needed.  */

volatile bool adc_rx_complete = false;
volatile bool adc_err_detect  = false;

static void adc_rx(uint32_t dev_id, void *priv);
static void adc_err(uint32_t dev_id, void *priv);

#define WAIT_RX_COMPLETE_OR_ERR  \
    do { \
        uint32_t timeout = TEST_TIMEOUT;\
        while((adc_rx_complete == false) \
                && (adc_err_detect == false) \
                && (timeout >= 0)) \
            timeout --; \
    }while(0)

static void adc_read(uint32_t channel_id, uint32_t num_entries)
{
    ss_adc_cfg_data_t cfg = { 0 };
    io_adc_seq_entry_t entries[num_entries];
    io_adc_seq_table_t seq_tbl;
    uint32_t    data[32] = { 0 };
    uint32_t    data_len = num_entries;
    uint32_t    result = 0;
    uint32_t    resolution = TEST_RESOLUTION;
    struct device* dev = &pf_device_ss_adc;

    cfg.in_mode         = SINGLED_ENDED;
    cfg.out_mode        = PARALLEL;
    cfg.serial_dly      = 1;
    cfg.seq_mode        = (num_entries > 1) ? REPETITIVE:SINGLESHOT;
    cfg.capture_mode    = FALLING_EDGE;
    cfg.clock_ratio     = TEST_CLK_RATIO;
    cfg.cb_rx           = adc_rx;
    cfg.cb_err          = adc_err;
    cfg.sample_width    = ss_adc_res_to_sample_width(resolution);
    cfg.priv            = NULL;


    ss_adc_set_config(dev, &cfg);

    for (uint32_t i = 0; i < num_entries; i++) {
        entries[i].channel_id = channel_id;
        entries[i].sample_dly = TEST_DLY;
    }

    seq_tbl.entries = entries;
    seq_tbl.num_entries = num_entries;
    if ((adc_rx_complete) || (adc_err_detect)) {
        adc_rx_complete = false;
        adc_err_detect = false;
    }
    ss_adc_read(dev, &seq_tbl, data, data_len);
    /* Wait for read to complete */
    WAIT_RX_COMPLETE_OR_ERR;
    CU_ASSERT("ADC completion int not received\n",
		(adc_rx_complete == true));
    CU_ASSERT("ADC error int received\n", (adc_err_detect == false));

    uint32_t j = 0;
    for(j=0 ;j < data_len; j++){
        result = ss_adc_data_to_mv(data[j], resolution);
        cu_print("Channel[%d] = %d - %d.%d Volts\n",
		    seq_tbl.entries[j].channel_id,
		    data[j],
		    result/1000, result%1000);
    }
}

static void adc_read_oneshot_test(void)
{
    int i;
    cu_print("Starting ADC read oneshot Test\n");
    cu_print(" Gathers oneshot samples from all ADC channels.\n");
    for (i = 0; i < NUM_ADC_CHANNELS; i++)
        adc_read(i, 1);
}

static void adc_read_repetitive_test (void)
{
    cu_print("Starting ADC read repetitive Test\n");
    cu_print(" Gathers %d repetitive samples from ADC channel %d.\n", REPETITIVE_ENTRIES, TEST_CHANNEL);
    adc_read(TEST_CHANNEL, REPETITIVE_ENTRIES);
}


void adc_test(void)
{
    cu_print("#################################\n");
    cu_print("Starting ADC Test\n");
    cu_print("#################################\n");

    adc_read_oneshot_test();
    adc_read_repetitive_test();

    cu_print("#################################\n");
    cu_print("End of ADC test\n");
    cu_print("#################################\n");
}

static void adc_rx(uint32_t dev_id, void *priv)
{
    adc_rx_complete = true;
}


static void adc_err(uint32_t dev_id,  void *priv)
{
    cu_print(" - ADC error int on %d\n", dev_id);
    adc_err_detect = true;
}
