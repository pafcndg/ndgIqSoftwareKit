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
#include "drivers/serial_bus_access.h"

#define TCMD_SPI_BUS_ID            2
#define TCMD_SPI_SLAVE             3
#define TCMD_SPI_BUFFER_LEN_WR     4
#define TCMD_SPI_TX_BUF_BYTES      5

#define MAX_NB              30
#define OUT_BUFFER_SIZE     30
#if !defined(_STRINGIFY)
#define _STRINGIFY(s) #s
#endif
#define STRINGIFY(s) _STRINGIFY(s)

static void sba_spi_generic_callback(sba_request_t * request)
{
	uint8_t idx = 0;
	int written_char = 0;
	char out_message[OUT_BUFFER_SIZE];

	struct tcmd_handler_ctx *req_ctx =
		(struct tcmd_handler_ctx *)request->priv_data;

	out_message[0] = 0;
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
 * Useful functions
 */
static inline SBA_BUSID bus_id_lookup(char *arr)
{
	if (!strcmp(arr, "spim0")) {
		return SBA_SPI_MASTER_0;
	} else if (!strcmp(arr, "spim1")) {
		return SBA_SPI_MASTER_1;
	} else if (!strcmp(arr, "spis0")) {
		return SBA_SPI_SLAVE_0;
#ifdef CONFIG_ARC
	} else if (!strcmp(arr, "ss_spim0")) {
		return SBA_SS_SPI_MASTER_0;
	} else if (!strcmp(arr, "ss_spim1")) {
		return SBA_SS_SPI_MASTER_1;
#endif
	} else {
		return -1;	//problem
	}
}

static inline SPI_SLAVE_ENABLE spi_slave_lookup(char *arr)
{
	int slave_number = atoi(arr);
	switch (slave_number) {
	case 0:
		return SPI_NO_SE;
	case 1:
		return SPI_SE_1;
	case 2:
		return SPI_SE_2;
	case 4:
		return SPI_SE_3;
	case 8:
		return SPI_SE_4;
	default:
		return -1;	//problem
	}
}

/**@brief Test command to transfer (send/receive) data on a SPI port:
 * spi tx <bus_id> <slave_addr> <len> <byte0> [<byte1>... <byte n>]
 * spi rx <bus_id> <slave_addr> <len>
 * spi trx <bus_id> <slave_addr> <len_to_write> [<byte0> <byte1>... <byte n>] <len_to_read>
 *
 * @param[in] argc Number of arguments in the Test Command (including group and name),
 * @param[in] argv Table of null-terminated buffers containing the arguments
 * @param[in] ctx The opaque context to pass to responses
 */
static void spi_handler(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint8_t idx = 0;
	char out_message[14];
	bool is_length = false;
	DRIVER_API_RC rc = DRV_RC_OK;
	/* allocate space for sba_request */
	sba_request_t *sba_request =
		(sba_request_t *) balloc(sizeof(sba_request_t), NULL);
	if (!strcmp(argv[1], "tx")) {
		if (argc < 6) {
			TCMD_RSP_ERROR(ctx, "check param");
			goto exit;
		}
		sba_request->request_type = SBA_TX;
	}
	else if (!strcmp(argv[1], "rx")) {
		if (argc != 5) {
                        TCMD_RSP_ERROR(ctx, "check param");
                        goto exit;
                }
		sba_request->request_type = SBA_RX;
	}
	else if (!strcmp(argv[1], "trx")) {
		if (argc < 6) {
                        TCMD_RSP_ERROR(ctx, "check param");
                        goto exit;
                }
		sba_request->request_type = SBA_TRANSFER;
	}
	sba_request->full_duplex = 0;
	sba_request->status = 1;
	sba_request->callback = sba_spi_generic_callback;
	sba_request->priv_data = ctx;
	sba_request->tx_len = 0;
	sba_request->rx_len = 0;
	sba_request->rx_buff = NULL;
	sba_request->tx_buff = NULL;
	sba_request->bus_id = bus_id_lookup(argv[TCMD_SPI_BUS_ID]);
	/* check bus_id */
	if (sba_request->bus_id== -1) {
		TCMD_RSP_ERROR(ctx, "<bus_id> ?");
		goto exit;
	}
	sba_request->addr.cs = (SPI_SLAVE_ENABLE) spi_slave_lookup(argv[TCMD_SPI_SLAVE]);
	/* check slave_address */
	if (sba_request->addr.cs == -1) {
		TCMD_RSP_ERROR(ctx, "<slave_address> ?");
		goto exit;
	}

	if (sba_request->request_type == SBA_TX || sba_request->request_type == SBA_TRANSFER)
	{
		sba_request->tx_len = (uint32_t) strtol(&argv[TCMD_SPI_BUFFER_LEN_WR][0], NULL, 0);
		/* check nb of bytes to be sent */
		if (sba_request->tx_len > (argc - 5)) {
			TCMD_RSP_ERROR(ctx,"<len> ?");
			goto exit;
		}
		if (sba_request->tx_len){
			is_length = true;
			/* process bytes to send */
			uint8_t *spi_buffer = balloc(sba_request->tx_len, NULL);
			for (idx = 0; idx < sba_request->tx_len; idx++) {
				spi_buffer[idx] =
					(uint8_t) strtol(&argv[TCMD_SPI_TX_BUF_BYTES + idx][0], NULL, 0);
			}
			sba_request->tx_buff = spi_buffer;
		}
	}
	if (sba_request->request_type == SBA_RX || sba_request->request_type == SBA_TRANSFER)
	{
		sba_request->rx_len = (uint32_t) strtol(&argv[argc - 1][0], NULL, 0);
		/* check nb of bytes to be read */
		if (sba_request->rx_len > MAX_NB / 3) {
			TCMD_RSP_ERROR(ctx,"Enter val under: "STRINGIFY(MAX_NB / 3));
			goto exit;
		}
		if (sba_request->rx_len){
			is_length = true;
			/* d. Instanciate a receive buffer, set request and execute it */
			sba_request->rx_buff = balloc(sba_request->rx_len, NULL);
		}
	}
	if (is_length == false)
	{
		TCMD_RSP_ERROR(ctx, NULL);
		goto exit;
	}
	rc = sba_exec_request(sba_request);
	if (rc != DRV_RC_OK) {
		snprintf(out_message, sizeof(out_message), "driver KO, %d", rc);
		TCMD_RSP_ERROR(ctx, out_message);
		goto exit;
	}
exit:
	if (sba_request->tx_buff) {
		bfree(sba_request->tx_buff);
	}
	if (sba_request->rx_buff) {
		bfree(sba_request->rx_buff);
	}
	bfree(sba_request);
	return;
}

/* spi tx, rx and trx test command declarations */
DECLARE_TEST_COMMAND(spi, tx, spi_handler);
DECLARE_TEST_COMMAND(spi, rx, spi_handler);
DECLARE_TEST_COMMAND(spi, trx, spi_handler);
