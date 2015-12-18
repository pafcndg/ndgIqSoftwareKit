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
#include "ble_ispp.h"
#include "infra/log.h"

#define ISPP_LOG(x) "ISPP: " x

static bool ispp_open = false;

bool ble_get_ispp_status(void) {
	return ispp_open;
}

static int _check_status(int32_t status)
{
	switch (status) {
	case 0:
		break;
	case BLE_SVC_GATT_STATUS_WRONG_STATE:
		pr_debug(LOG_MODULE_BLE, "ISPP wrong state. ");
		break;
	case BLE_SVC_GATT_STATUS_INVALID_ATTR_LEN:
		pr_debug(LOG_MODULE_BLE, "ISPP Exceeding max MTU (256 bytes). ");
		break;
	case BLE_SVC_GATT_STATUS_INSUF_RESOURCE:
		pr_debug(LOG_MODULE_BLE, "ISPP Not enough memory to store the message. ");
		break;
	case BLE_SVC_GATT_STATUS_BUSY:
		pr_debug(LOG_MODULE_BLE, "ISPP Max pending number of messages has been reached. ");
		break;
	case BLE_SVC_GATT_STATUS_NOT_FOUND:
		pr_debug(LOG_MODULE_BLE, "ISPP Read: empty queue. ");
		break;
	default:
		pr_debug(LOG_MODULE_BLE, "ISPP: ERROR: %x ", status);
	}
	return status;
}

void on_ble_ispp_evt(const ble_ispp_event_s *ispp_evt) {

	switch (ispp_evt->event) {
		case BLE_ISPP_OPEN:
			{
				pr_info(LOG_MODULE_BLE,
					    ISPP_LOG("link open, MTU=%d bytes"),
					    ispp_evt->evt.open.mtu);
				uint32_t flag = interrupt_lock();
				ispp_open = true;
				interrupt_unlock(flag);
				break;
			}
		case BLE_ISPP_CLOSE:
			{
				pr_info(LOG_MODULE_BLE, ISPP_LOG("link closed"));
				uint32_t flag = interrupt_lock();
				ispp_open = false;
				interrupt_unlock(flag);
				break;
			}
		case BLE_ISPP_RX_COMPLETE:
			{
				uint16_t size = ispp_evt->evt.rx.size;
				if (size > 0) {
					pr_info(LOG_MODULE_BLE, ISPP_LOG("RX [%d] bytes"), size);
					struct ble_ispp_rx_message *msg = ble_ispp_read();
					if (!msg)
						break;
					uint32_t err_code =
						ble_ispp_write(NULL, msg->buf, msg->len);
					_check_status(err_code);
					bfree(msg);
				} else
					pr_error(LOG_MODULE_BLE, ISPP_LOG("RX empty message"));
				break;
			}
		case BLE_ISPP_TX_COMPLETE:
			{
				int status = ispp_evt->evt.tx.status;
				if (status == E_OS_OK) {
					pr_info(LOG_MODULE_BLE,
					        ISPP_LOG("TX [%d] bytes"),
					        ispp_evt->evt.tx.len);
				} else
					pr_error(LOG_MODULE_BLE, ISPP_LOG("TX failed [%d]"), status);
				break;
			}
		default:
			{
				pr_error(LOG_MODULE_MAIN,
				         ISPP_LOG("unknown evt: %d\n"),
				         ispp_evt->event);
				break;
			}
	}
}
