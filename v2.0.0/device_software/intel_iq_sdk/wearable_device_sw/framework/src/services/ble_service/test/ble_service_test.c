/* INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
  *
  * The source code contained or described herein and all documents related to
  * the source code ("Material") are owned by Intel Corporation or its suppliers
  * or licensors.
  * Title to the Material remains with Intel Corporation or its suppliers and
  * licensors.
  * The Material contains trade secrets and proprietary and confidential information
  * of Intel or its suppliers and licensors. The Material is protected by worldwide
  * copyright and trade secret laws and treaty provisions.
  * No part of the Material may be used, copied, reproduced, modified, published,
  * uploaded, posted, transmitted, distributed, or disclosed in any way without
  * Intel’s prior express written permission.
  *
  * No license under any patent, copyright, trade secret or other intellectual
  * property right is granted to or conferred upon you by disclosure or delivery
  * of the Materials, either expressly, by implication, inducement, estoppel or
  * otherwise.
  *
  * Any license under such intellectual property rights must be express and
  * approved by Intel in writing
  *
  ******************************************************************************/

#include <unistd.h>
#include "cfw/cfw_internal.h"
#include "cfw/cfw_messages.h"
#include "services/ble_service/ble_service_gatt.h"
#include "services/ble_service/ble_service_api.h"
#include "ble_service_bas.h"
#include "service_queue.h"
#include "infra/log.h"
#include "ble_service_int.h"
#include "ble_service_dis.h"
#include "ble_service_bas.h"

static cfw_service_conn_t *ble_service_conn = NULL;
extern void test_ble_enable_request(void);

struct _ble_register_svc {
	 /**< callback function to execute on MSG_ID_xxx_RSP
	  * @param reg this buffer */
	void (*func_cback)(struct _ble_register_svc *reg);
	int cback_param; /**< register parameter @ref BLE_APP_SVC_REG */
};


struct _ble_service_test_cb {
	uint32_t counter;
	uint16_t conn_h;
	uint8_t battery_level;
};

static struct _ble_service_test_cb _ble_service_test_cb;
const uint8_t ble_dev_name_change[] = "TEST_CHANGE_NAME";

static void handle_msg_id_ble_enable_rsp(struct cfw_message *msg)
{
	if (((struct ble_generic_msg *)msg)->status == BLE_STATUS_SUCCESS) {
		struct _ble_register_svc param;

		ble_init_service_dis(ble_service_conn, &param);
	} else if (((struct ble_generic_msg *)msg)->status == BLE_STATUS_PENDING) {
		/* repeat enable request as waiting for open of ble core */
#ifdef CONFIG_BLE_CORE_TEST
		sleep(1); /* hack to give some time to ble core to respond */
		/* starting the test command */
		test_ble_enable_request();
#endif
	} else
		pr_error(LOG_MODULE_BLE, "ble enable failed with 0x%x",
				((struct ble_generic_msg *)msg)->status);
}

static void handle_msg_id_ble_init_svc_rsp(struct cfw_message *msg)
{
	struct ble_init_service_rsp *rsp =
			(struct ble_init_service_rsp *)msg;

	if (BLE_STATUS_SUCCESS == rsp->status) {
		if (BLE_SVC_UUID_DEVICE_INFORMATION_SERVICE == rsp->svc_uuid.u16) {
			struct _ble_register_svc params;
			ble_init_service_bas(ble_service_conn, &params);
		} else {
			ble_start_advertisement(ble_service_conn, BLE_NO_ADV_OPT, NULL, NULL);
		}
	} else {
		pr_error(LOG_MODULE_BLE, "FATAL error: 0x%x", rsp->status);
	}
}

static void handle_msg_id_ble_start_adv_rsp(struct cfw_message *msg)
{
	(void) msg;
	//for test only
	ble_set_name(ble_service_conn, ble_dev_name_change, NULL);
}
static void handle_msg_id_ble_set_name_rsp(struct cfw_message *msg)
{
	struct ble_rsp *rsp = (struct ble_rsp *)msg;

	if (BLE_STATUS_SUCCESS == rsp->status) {
		pr_debug(LOG_MODULE_BLE, "BLE name changed success");
	}
	else{
		pr_debug(LOG_MODULE_BLE, "BLE name changed unsuccess");
	}
}

static void handle_msg_id_ble_disconnect_rsp(struct cfw_message *msg)
{
	struct ble_disconnect_rsp *rsp = (struct ble_disconnect_rsp *)msg;

	if (BLE_STATUS_SUCCESS == rsp->status) {
		pr_debug(LOG_MODULE_BLE, "BLE disc success");
	}
	else{
		pr_debug(LOG_MODULE_BLE, "BLE disc unsuccess");
	}
}

static void client_ble_service_handle_message(struct cfw_message *msg,
					      void *param)
{
	(void)param;

	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_OPEN_SERVICE_RSP:
	{
		ble_service_conn =
		    (cfw_service_conn_t *) ((cfw_open_conn_rsp_msg_t *)
					     msg)->service_conn;
		int events[] = {
				MSG_ID_BLE_CONNECT_EVT,
				MSG_ID_BLE_DISCONNECT_EVT,
		};
		cfw_register_events(ble_service_conn, events,
				sizeof(events)/sizeof(int),
				&_ble_service_test_cb);
	}
		break;
	case MSG_ID_CFW_REGISTER_EVT_RSP:
		break;
	case MSG_ID_BLE_ENABLE_RSP:
		handle_msg_id_ble_enable_rsp(msg);
		break;
	case MSG_ID_BLE_INIT_SVC_RSP:
		handle_msg_id_ble_init_svc_rsp(msg);
		break;
	case MSG_ID_BLE_START_ADV_RSP:
		handle_msg_id_ble_start_adv_rsp(msg);
		break;
	case MSG_ID_BLE_SET_NAME_RSP:
		handle_msg_id_ble_set_name_rsp(msg);
		break;
	case MSG_ID_BLE_UPDATE_DATA_RSP:
		break;
	case MSG_ID_BLE_DISCONNECT_RSP:
		handle_msg_id_ble_disconnect_rsp(msg);
		break;
	case MSG_ID_BLE_CONNECT_EVT: {
		struct ble_connect_evt *evt =
				(struct ble_connect_evt *)msg;
		_ble_service_test_cb.conn_h = evt->conn_handle;
		_ble_service_test_cb.counter = 1;
	}
		break;
	case MSG_ID_BLE_DISCONNECT_EVT: {
		struct ble_disconnect_evt *evt =
				(struct ble_disconnect_evt *)msg;
		_ble_service_test_cb.conn_h = 0xffff;
		_ble_service_test_cb.counter = 0;
	}
		break;

	default:
		cfw_print_default_handle_error_msg(LOG_MODULE_BLE, CFW_MESSAGE_ID(msg));
		break;
	}
	cfw_msg_free(msg);
}

void test_ble_service_init(void)
{
	cfw_client_t *ble_client =
	    cfw_client_init(get_service_queue(), client_ble_service_handle_message,
		     "Client BLE");
	_ble_service_test_cb.conn_h = BLE_SVC_GAP_HANDLE_INVALID;
	cfw_open_service_conn(ble_client, BLE_SERVICE_ID, "BLE Test Client");
}

const uint8_t ble_dev_name[] = "Curie 1.0";
void test_ble_enable_request(void)
{
	struct ble_enable_config en_config;

	const struct ble_gap_connection_params conn_params =
	    {MSEC_TO_1_25_MS_UNITS(80), MSEC_TO_1_25_MS_UNITS(150), 0, MSEC_TO_10_MS_UNITS(6000)};

	en_config.options = 0;
	en_config.p_name = (uint8_t *) ble_dev_name;
	en_config.p_bda = NULL;
	en_config.appearance = BLE_GAP_APPEARANCE_TYPE_GENERIC_WATCH;
	en_config.central_conn_params = en_config.peripheral_conn_params = conn_params;

	ble_enable(ble_service_conn, 1, &en_config, NULL);
}

void test_ble_service_update_battery_level(uint8_t level)
{
	if (_ble_service_test_cb.counter) {
		if (((++_ble_service_test_cb.counter) % 10) == 0) {
			int ret;

			_ble_service_test_cb.battery_level = level;
			ret = ble_service_update_bat_level(ble_service_conn,
					_ble_service_test_cb.conn_h, level,
					&_ble_service_test_cb);
			_ble_service_test_cb.counter = 0; /* restart it in response */
			if (ret)
				pr_warning(LOG_MODULE_BLE, "failed to update bat lvl");
		}
	}
}
