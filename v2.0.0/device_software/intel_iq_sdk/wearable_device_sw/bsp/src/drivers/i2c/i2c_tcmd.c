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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "infra/tcmd/handler.h"
#include "infra/device.h"
#include "drivers/serial_bus_access.h"

#define ARGS_BUS_ID_IDX      2
#define BUFFER_LENGTH        128
#define READ_BUFFER_LENGTH   1

struct args_number {
    uint32_t write;
    uint32_t probe;
    uint32_t rx;
    uint32_t read;
};

struct args_index_generic {
    uint32_t slave_adr;
    uint32_t index;
};

static const struct args_number args_no = {
    .write  = 5,
    .probe  = 4,
    .rx     = 5,
    .read   = 6,
};
/* used for tx and trx */
static const struct args_index_generic args_idx = {
    .slave_adr = ARGS_BUS_ID_IDX + 1,
    .index     = ARGS_BUS_ID_IDX + 2
};

/*
 * @addtogroup infra_tcmd
 * @{
 */

/*
 * @defgroup infra_tcmd_i2c I2C Test Commands
 * Interfaces to support I2C Test Commands.
 * @{
 */
static void sba_i2c_generic_callback(sba_request_t *request)
{
    uint8_t idx =0;
    uint32_t written_char = 0;
    char out_message[BUFFER_LENGTH];
    out_message[0] = 0;

    struct tcmd_handler_ctx *req_ctx =
        (struct tcmd_handler_ctx *)request->priv_data;
    if (request->status == 0) {
        for (idx = 0; idx < request->rx_len; idx++) {
            if (written_char < sizeof(out_message)) {
                written_char += snprintf(out_message + written_char,
                        sizeof(out_message) - written_char,
                        " %02x", request->rx_buff[idx]);
            }
        }
        TCMD_RSP_FINAL(req_ctx, out_message);
    } else {
         TCMD_RSP_ERROR(req_ctx, NULL);
    }
    if (request->tx_buff) {
        bfree(request->tx_buff);
    }
    if (request->rx_buff) {
        bfree(request->rx_buff);
    }
    bfree(request);
}

/*
 * Returns bus_id based on user arguments.
 *
 */
static inline SBA_BUSID calculate_bus_id(const char *arr)
{
    if (!strcmp(arr, "i2c0"))
        return SBA_I2C_MASTER_0;
    else if (!strcmp(arr, "i2c1"))
        return SBA_I2C_MASTER_1;
#ifdef CONFIG_ARC
    else if (!strcmp(arr, "ss_i2c0"))
        return SBA_SS_I2C_MASTER_0;
    else if (!strcmp(arr, "ss_i2c1"))
        return SBA_SS_I2C_MASTER_1;
#endif
    else
        return -1;
}

/*
 * Test command to start tx: [arc.]i2c write <bus_id> <slave_addr> <register> [value1... valueN]
 * Test command to probe a device: [arc.]i2c probe <bus_id> <slave_addr>
 * Test command to start rx: [arc.]i2c rx <bus_id> <slave_addr> <len>
 * Test command to read data: [arc.]i2c read <bus_id> <slave_addr> <register> <len>
 *
 * @param[in]   argc       Number of arguments in the Test Command (including group and name),
 * @param[in]   argv       Table of null-terminated buffers containing the arguments
 * @param[in]   ctx        The context to pass back to responses
 */
static void i2c_common_handler(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
    uint8_t idx;
    DRIVER_API_RC rc;
    static uint8_t *tx_buff;
    sba_request_t *sba_request =
        (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
    if (!strcmp(argv[1], "probe" ))
    {
        if (argc != args_no.probe)
            goto print_help;
        sba_request->request_type = SBA_TX;
        sba_request->tx_len       = 1;
        tx_buff = balloc(sba_request->tx_len, NULL);
        tx_buff[0] = (uint8_t)strtol(argv[3], NULL, 16);
        sba_request->tx_buff      = tx_buff;
        sba_request->rx_len       = 0;
        sba_request->rx_buff      = NULL;
    }
    else if (!strcmp(argv[1], "write"))
    {
        if (argc < args_no.write)
            goto print_help;
        sba_request->request_type = SBA_TX;
        sba_request->tx_len = argc - args_idx.index;
        tx_buff = balloc(sba_request->tx_len, NULL);
        for (idx = 0; idx < sba_request->tx_len ; idx++) {
            if (BUFFER_LENGTH <= idx)
                goto print_help;
            tx_buff[idx] =
                (uint8_t)strtol(&argv[args_idx.index + idx][0], NULL, 16);
        }
        sba_request->tx_buff      = tx_buff;
        sba_request->rx_len       = 0;
        sba_request->rx_buff      = NULL;
    }
    else if (!strcmp(argv[1], "rx"))
    {
        if (argc != args_no.rx)
            goto print_help;
        sba_request->request_type = SBA_RX;
        sba_request->tx_len       = 0;
        sba_request->tx_buff      = NULL;
        sba_request->rx_len = (uint32_t) strtol(&argv[argc - 1][0], NULL, 0);
        if (BUFFER_LENGTH < sba_request->rx_len)
            goto print_help;
        sba_request->rx_buff = balloc(sba_request->rx_len, NULL);
    }
    else if (!strcmp(argv[1], "read"))
    {
        if (argc < args_no.write)
            goto print_help;
        sba_request->request_type = SBA_TRANSFER;
        sba_request->tx_len       = READ_BUFFER_LENGTH;
        tx_buff = balloc(sba_request->tx_len, NULL);
        tx_buff[0] = (uint8_t)strtol(argv[args_idx.index], NULL, 16);
        sba_request->tx_buff      = tx_buff;
        sba_request->rx_len = (uint32_t) strtol(&argv[argc - 1][0], NULL, 0);
        if (BUFFER_LENGTH < sba_request->rx_len)
            goto print_help;
        sba_request->rx_buff = balloc(sba_request->rx_len, NULL);
    }
    sba_request->bus_id = calculate_bus_id(argv[ARGS_BUS_ID_IDX]);
    /* check bus_id */
    if (sba_request->bus_id == -1) {
        goto print_help;
    }
    sba_request->addr.slave_addr = (uint32_t)strtol(argv[args_idx.slave_adr], NULL, 16);
    sba_request->status       = 0;
    sba_request->callback     = sba_i2c_generic_callback;
    sba_request->priv_data    = ctx;
    rc = sba_exec_request(sba_request);
    if (rc != DRV_RC_OK) {
        TCMD_RSP_ERROR(ctx, NULL);
        goto print_help;
    }
print_help:
    if (sba_request->tx_buff) {
        bfree(sba_request->tx_buff);
    }
    if (sba_request->rx_buff) {
        bfree(sba_request->rx_buff);
    }
    bfree(sba_request);
    return;
}
DECLARE_TEST_COMMAND(i2c, read, i2c_common_handler);
DECLARE_TEST_COMMAND(i2c, rx, i2c_common_handler);
DECLARE_TEST_COMMAND(i2c, probe, i2c_common_handler);
DECLARE_TEST_COMMAND(i2c, write, i2c_common_handler);
