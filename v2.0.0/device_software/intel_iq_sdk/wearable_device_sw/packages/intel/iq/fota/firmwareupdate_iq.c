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

#include <stdint.h>
#include "infra/log.h"
#include "infra/pm.h"
#include "drivers/spi_flash.h"
#include "soc_config.h"
#include "os/os.h"
#include "project_mapping.h"
#include "machine.h"

/* Topic manager */
#include "iq/itm/itm.h"
#include "iq/itm/topic_application.h"

/* Iq initialization */
#include "iq/init_iq.h"

/* Protobufs */
#include "pb_decode.h"
#include "pb_encode.h"
#include "IntelFirmwareUpdate.pb.h"

/* UUID */
#include "iq/uuid_iq.h"

/* BLE */
#include "lib/ble/ble_app.h"
#include "services/ble_service/ble_service_gap_api.h"

/* BOOT */
#include "infra/boot.h"

#define FIRMWARE_UPDATE_TOPIC_INDEX       0
#define FIRMWARE_UPDATE_STATE_TOPIC_INDEX 1
#define FIRMWARE_MAX_SIZE                 (512*1024)
/* Compute nb blocks of flash in order to receive 512kb */
#define NB_SECTOR_TO_ERASE                FIRMWARE_MAX_SIZE / SERIAL_FLASH_BLOCK_SIZE
#define FLASH_START_ADDRESS               SPI_FOTA_START_BLOCK

enum flash_state {
	STARTED,
	IN_PROGRESS,
	NONE
};

static enum flash_state state = NONE;

const uint8_t _firmware_update_topic[] = FIRMWAREUPDATE_UUID;
const uint8_t _firmware_updatestate_topic[] = FIRMWAREUPDATESTATE_UUID;

static uint8_t const * const _firmwareupdate_topics[] = {
	[FIRMWARE_UPDATE_TOPIC_INDEX] = _firmware_update_topic,
	[FIRMWARE_UPDATE_STATE_TOPIC_INDEX] = _firmware_updatestate_topic,
	NULL
};

static int _app_id = 0xFF;

static uint8_t erase_spi_flash()
{
	uint8_t ret = DRV_RC_OK;

	ret = spi_flash_sector_erase((struct device *)&pf_sba_device_flash_spi0,
				    FLASH_START_ADDRESS,
				    NB_SECTOR_TO_ERASE);
	return ret;
}

static uint8_t write_spi_flash(uint32_t address, uint8_t *data, uint8_t len)
{
	unsigned int data_written;
	uint8_t ret = DRV_RC_OK;

	ret = spi_flash_write_byte((struct device *)&pf_sba_device_flash_spi0,
				   address + FLASH_START_ADDRESS,
				   len, &data_written, data);
	return ret;
}

static void _on_itm_request(uint8_t con_id, uint8_t req_id, uint8_t topic_ix, const uint8_t * data, uint16_t len) {

	bool is_already_sent = false;
	/* Deserialize protobuf to get request data */
	pb_istream_t stream = pb_istream_from_buffer((uint8_t*)data, len);
	if (topic_ix == FIRMWARE_UPDATE_TOPIC_INDEX) {
		uint8_t flash_ret = DRV_RC_OK;
		uint8_t request_status = TOPIC_STATUS_OK;
		uint8_t response_status = TOPIC_STATUS_OK;
		intel_FirmwareUpdate proto = intel_FirmwareUpdate_init_default;
		if (pb_decode(&stream, intel_FirmwareUpdate_fields, &proto)) {
			switch (proto.firmware_update_cmd) {
			case intel_FirmwareUpdate_firmwareOperation_START:
				if (state == NONE) {
					pr_debug(LOG_MODULE_IQ,
					"FOTA: flashing firmware, size is : %d (none -> start)",
					proto.firmware_total_length);
					if (proto.firmware_total_length < FIRMWARE_MAX_SIZE) {
						/* Since the erase takes a while, send OK response before
						* the BLE communication timeout */
						itm_response(con_id, req_id, TOPIC_STATUS_OK, NULL, 0);
						is_already_sent = true;
						flash_ret = erase_spi_flash();
						if (flash_ret == DRV_RC_OK) {
							state = STARTED;
							/* FIXME local_task_sleep_ms */
							local_task_sleep_ms(1000);
							reboot(TARGET_RECOVERY);
						}
					}
					else
						/* Bad request */
						request_status = TOPIC_STATUS_INVALID;
				}
				else
					/* Bad request */
					request_status = TOPIC_STATUS_INVALID;
				break;

			case intel_FirmwareUpdate_firmwareOperation_CONTINUE:
				if (state == IN_PROGRESS || state == STARTED) {
					if (proto.has_data && proto.has_offset) {
						flash_ret = write_spi_flash(proto.offset,
						proto.data.bytes,
						proto.data.size);
					} else if (!proto.has_offset) {
						/* Bad request */
						request_status = TOPIC_STATUS_INVALID;
					}
					else if (!proto.has_data)
						/* Bad request */
						request_status = TOPIC_STATUS_INVALID;
				}
				else
					/* Bad request */
					request_status = TOPIC_STATUS_INVALID;

				if (state == STARTED ) {
					pr_debug(LOG_MODULE_IQ, "FOTA: flash start -> continue");
					state = IN_PROGRESS;
					struct ble_gap_connection_params con_params = {6, 10, 0, 100};
					/* Speed up the connection before starting the firmware udpate */
					ble_app_conn_update(&con_params);
					pr_debug(LOG_MODULE_IQ, "FOTA: speed up connection");
				}
				break;

			case intel_FirmwareUpdate_firmwareOperation_CANCEL:
				if (state == IN_PROGRESS) {
					pr_debug(LOG_MODULE_IQ, "FOTA: flash canceled");
					/* Since the erase takes a while, send OK response before
					 * the BLE communication timeout */
					itm_response(con_id, req_id, TOPIC_STATUS_OK, NULL, 0);
					is_already_sent = true;
					flash_ret = erase_spi_flash();
					if (flash_ret == DRV_RC_OK)
						state = NONE;
				}
				else if (state == STARTED)
					state = NONE;
				else
					/* Bad request */
					request_status = TOPIC_STATUS_INVALID;
				break;

			default:
				pr_error(LOG_MODULE_IQ, "FOTA: Unexpected command");
				break;
			}

			/* Check if response has been already sent in case of flash erase */
			if (is_already_sent == false) {
				if (flash_ret != DRV_RC_OK)
					response_status = TOPIC_STATUS_FAIL;
				else
					response_status = request_status;

				itm_response(con_id, req_id, response_status, NULL, 0);
			}

			/* reboot the device in OTA mode when flash is complete */
			if (proto.offset + proto.data.size == proto.firmware_total_length) {
				state = NONE;
				pr_debug(LOG_MODULE_IQ,"FOTA: flash complete\n");
				/* FIXME local_task_sleep_ms */
				local_task_sleep_ms(1000);
				reboot(TARGET_OTA);
			}
		} else
			pr_error(LOG_MODULE_IQ, ERROR_MSG_DECODING_FAILURE_FOR_UUID, _firmwareupdate_topics[topic_ix]);
	}
	else if (topic_ix == FIRMWARE_UPDATE_STATE_TOPIC_INDEX) {
		size_t proto_length;
		bool proto_status;
		uint8_t proto_buffer[intel_FirmwareUpdateState_size];
		intel_FirmwareUpdateState proto = intel_FirmwareUpdateState_init_default;
		/* Send flash status */
		pb_ostream_t proto_ostream = pb_ostream_from_buffer(proto_buffer, sizeof(proto_buffer));
		proto.has_firmware_update_state = true;
		proto.firmware_update_state = state;

		proto_status = pb_encode(&proto_ostream, intel_FirmwareUpdateState_fields, &proto);
		proto_length = proto_ostream.bytes_written;

		/* Then just check for any errors. */
		if (!proto_status) {
			pr_error(LOG_MODULE_IQ, ERROR_MSG_ENCODING_FAILURE_FOR_UUID, _firmwareupdate_topics[topic_ix]);
			itm_response(con_id, req_id, TOPIC_STATUS_FAIL, NULL, 0);
		} else
			itm_response(con_id, req_id, TOPIC_STATUS_OK, proto_buffer, proto_length);
	} else {
		pr_error(LOG_MODULE_IQ, "Invalid FOTA request");
	}
}

void firmwareupdate_iq_init(T_QUEUE queue)
{
	UNUSED(queue);
	static struct topic_application app_desc =
	{
		.topic_array = _firmwareupdate_topics,
		.on_request = _on_itm_request
	};

	/* Set the current state */
	enum boot_targets boot_event = get_boot_target();
	if (boot_event == TARGET_RECOVERY)
		state = STARTED;
	else
		state = NONE;

	/* Interface with the Topic Manager */
	_app_id = itm_add_application(&app_desc);
	if (_app_id >= 0)
		itm_start_application(_app_id);
	init_done();
}


