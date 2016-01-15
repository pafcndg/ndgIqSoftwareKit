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
 * Intel's prior express written permission.
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
#include <string.h>
#include "infra/tcmd/handler.h"
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "services/ble_service/ble_service_api.h"
#include "ble_service_dis.h"
#include "ble_service_bas.h"
#if defined(CONFIG_SERVICES_BLE_ISPP)
#include "ble_service_ispp.h"
#endif
#include "cfw/cproxy.h"
#include "infra/log.h"
#include "ble_protocol.h"
#include "ble_service_int.h"
#include "ble_service_utils.h"
#include "cfw/cfw_service.h"

#include "lib/ble/ble_app.h"

#ifdef CONFIG_BLE_SM_IO_CAP_TEST
#include "../../src/lib/ble/ble_app_internal.h"
#endif

#define ANS_LENGTH 80

struct _enable {
	uint8_t enable_flag;
	uint8_t mode;
	uint8_t name;
	uint8_t argc;
};

struct _tx_test {
	uint8_t op;
	uint8_t freq;
	uint8_t len;
	uint8_t pattern;
	uint8_t argc;
};

struct _rx_test {
	uint8_t op;
	uint8_t freq;
	uint8_t argc;
};

struct _test_set_tx_pwr {
	uint8_t dbm;
	uint8_t argc;
};

struct _advertise {
	uint8_t action;
	uint8_t options;
	uint8_t name;
	uint8_t argc;
};

struct _args_index {
	struct _enable enable;
	struct _tx_test tx_test;
	struct _rx_test rx_test;
	struct _test_set_tx_pwr test_set_tx_pwr;
	struct _advertise advertise;
};

static const struct _args_index _args = {
	.enable = {
		   .enable_flag = 2,
		   .mode = 3,
		   .name = 4,
		   .argc = 5},
	.tx_test = {
		    .op = 2,
		    .freq = 3,
		    .len = 4,
		    .pattern = 5,
		    .argc = 6},
	.rx_test = {
		    .op = 2,
		    .freq = 3,
		    .argc = 4},
	.test_set_tx_pwr = {
		    .dbm = 2,
		    .argc = 3},
	.advertise = {
			.action = 2,
			.options = 3,
			.name = 4,
			.argc = 5}
};

struct _info_for_rsp {
	struct tcmd_handler_ctx *ctx;
	cfw_service_conn_t *ble_service_conn;
	enum TEST_OPCODE test_opcode;
	uint8_t enable_flag;
};

struct _ble_service_tcmd_cb {
	cfw_service_conn_t *ble_service_conn;
	struct bt_conn *conn;
};

static struct _ble_service_tcmd_cb * _ble_service_tcmd_cb = NULL;


/*
 * @defgroup cfw_tcmd_ble BLE Test Commands
 * Interfaces to support BLE Test Commands.
 * @ingroup infra_tcmd
 * @{
 */

static void ble_init_svc_rsp(struct cfw_message *msg)
{
}

/*
 * Display message if status is KO.
 *
 * @param       status return value in RSP
 * @param[in]   ctx   The context to pass back to responses
 * @return
 */
static int32_t _check_status(int32_t status, struct tcmd_handler_ctx *ctx)
{
	if (status != BLE_STATUS_SUCCESS) {
		char answer[ANS_LENGTH];
		snprintf(answer, ANS_LENGTH, "KO %d", status);
		TCMD_RSP_ERROR(ctx, answer);
	}
	return status;
}

/*
 * Checks api return.
 *
 * @param       status return value by api
 * @param[in]   ctx   The context to pass back to responses
 * @return      none
 */
static int _api_check_status(int32_t status, struct _info_for_rsp *info_for_rsp)
{
	_check_status(status, info_for_rsp->ctx);
	if (status != E_OS_OK) {
		cproxy_disconnect(info_for_rsp->ble_service_conn);
		bfree(info_for_rsp);
	}
	return status;
}

static void version_print(struct cfw_message *msg,
		struct tcmd_handler_ctx *ctx)
{
	struct ble_version_rsp *resp = (struct ble_version_rsp *)msg;

	char buf[ANS_LENGTH];

	snprintf(buf, ANS_LENGTH, "%d.%d.%d", resp->version.major,
			resp->version.minor,
			resp->version.patch);
	TCMD_RSP_PROVISIONAL(ctx, buf);

	snprintf(buf, ANS_LENGTH, "%.20s",resp->version.version_string);
	TCMD_RSP_PROVISIONAL(ctx, buf);

	snprintf(buf, ANS_LENGTH, "Micro-sha1 : %02x%02x%02x%02x", resp->version.hash[0],
			      resp->version.hash[1], resp->version.hash[2],
			      resp->version.hash[3]);
	TCMD_RSP_FINAL(ctx, buf);
}

static void get_info_print(struct cfw_message *msg,
		struct tcmd_handler_ctx *ctx)
{
	struct ble_get_info_rsp *resp = (struct ble_get_info_rsp *)msg;
#ifdef CONFIG_TCMD_BLE_DEBUG
	int i;
#endif
	if (resp->info_type == BLE_INFO_BDA_NAME_REQ) {
		char buf[ANS_LENGTH];

		snprintf(buf, ANS_LENGTH, "BDA type: %d, address: %x:%x:%x:%x:%x:%x",
				resp->info_params.bda_name_params.bda.type,
				resp->info_params.bda_name_params.bda.val[5],
				resp->info_params.bda_name_params.bda.val[4],
				resp->info_params.bda_name_params.bda.val[3],
				resp->info_params.bda_name_params.bda.val[2],
				resp->info_params.bda_name_params.bda.val[1],
				resp->info_params.bda_name_params.bda.val[0]);
		TCMD_RSP_PROVISIONAL(ctx, buf);

		snprintf(buf, ANS_LENGTH, "GAP device name: %s",
				(char *)resp->info_params.bda_name_params.name);
		TCMD_RSP_PROVISIONAL(ctx, buf);

		snprintf(buf, ANS_LENGTH, "Number of bonded devices: %d",
				resp->info_params.bda_name_params.bonded_devs.addr_count);
#ifdef CONFIG_TCMD_BLE_DEBUG
		if (resp->info_params.bda_name_params.bonded_devs.addr_count > 0) {
			TCMD_RSP_PROVISIONAL(ctx, buf);
			snprintf(buf, ANS_LENGTH, "Device # | Type | Address/IRK");
			for (i = 0; i < resp->info_params.bda_name_params.bonded_devs.addr_count; i++) {
				TCMD_RSP_PROVISIONAL(ctx, buf);
				snprintf(buf, ANS_LENGTH, "%8d | %4x | %x:%x:%x:%x:%x:%x", i + 1,
						resp->info_params.bda_name_params.bonded_devs.addrs[i].type,
						resp->info_params.bda_name_params.bonded_devs.addrs[i].val[5],
						resp->info_params.bda_name_params.bonded_devs.addrs[i].val[4],
						resp->info_params.bda_name_params.bonded_devs.addrs[i].val[3],
						resp->info_params.bda_name_params.bonded_devs.addrs[i].val[2],
						resp->info_params.bda_name_params.bonded_devs.addrs[i].val[1],
						resp->info_params.bda_name_params.bonded_devs.addrs[i].val[0]);
			}
		}
#endif
		TCMD_RSP_FINAL(ctx, buf);
	}

}

static void _ble_tcmd_handle_msg(struct cfw_message *msg, void *data)
{
	char answer[ANS_LENGTH];
	struct _info_for_rsp *info_for_rsp = msg->priv;

	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_BLE_DTM_RSP:
		if (!_check_status(((struct ble_dtm_test_rsp *)msg)->status, info_for_rsp->ctx)) {
			if (info_for_rsp->test_opcode == BLE_TEST_END_DTM)
				snprintf(answer, ANS_LENGTH,
					 "RX results: Mode = %u. Nb = %u",
					 ((struct ble_dtm_test_rsp *)msg)->result.mode,
					 ((struct ble_dtm_test_rsp *)msg)->result.nb);
			else
				answer[0] = '\0';
			TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		}
		break;
	case MSG_ID_BLE_ENABLE_RSP:
		if (!_check_status(((struct ble_enable_rsp *)msg)->status, info_for_rsp->ctx)) {
			if (0 == info_for_rsp->enable_flag && _ble_service_tcmd_cb) {
				if (_ble_service_tcmd_cb->ble_service_conn) {
					cproxy_disconnect(_ble_service_tcmd_cb->ble_service_conn);
					_ble_service_tcmd_cb->ble_service_conn = NULL;
				}
				bfree(_ble_service_tcmd_cb);
			}
			TCMD_RSP_FINAL(info_for_rsp->ctx, "0");
		}
		break;
#ifdef CONFIG_SERVICES_BLE_GATTC
	case MSG_ID_BLE_DISCOVER_RSP:
		if (!_check_status(((struct ble_discover_rsp *)msg)->status, info_for_rsp->ctx)) {
			size_t i;
			char *p_answer = answer;
			// by default, indicate empty
			sprintf(p_answer, "None");
			for (i = 0; i < ((struct ble_discover_rsp *)msg)->attr_cnt && p_answer < &answer[ANS_LENGTH-1]; i++) {
				sprintf(p_answer, "%d-", ((struct ble_discover_rsp *)msg)->attrs[i].handle);
				p_answer += strlen(p_answer);
			}
			TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		}
		break;
	case MSG_ID_BLE_GET_REMOTE_DATA_RSP:
		if (!_check_status(((struct ble_get_remote_data_rsp *)msg)->status, info_for_rsp->ctx)) {
			size_t i;
			char *p_answer = NULL;
			p_answer = answer;
			for (i = 0; i < ((struct ble_get_remote_data_rsp *)msg)->data_length && p_answer < &answer[ANS_LENGTH-1]; i++) {
				sprintf(p_answer, "0x%02X-", ((struct ble_get_remote_data_rsp *)msg)->data[i]);
				p_answer += strlen(p_answer);
			}
			TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		}
		break;
	case MSG_ID_BLE_SUBSCRIBE_RSP:
		snprintf(answer, ANS_LENGTH, "sub_rsp %d %p",
				((struct ble_subscribe_rsp *)msg)->status,
				((struct ble_subscribe_rsp *)msg)->p_subscription);
		TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		break;
	case MSG_ID_BLE_UNSUBSCRIBE_RSP:
		snprintf(answer, ANS_LENGTH, "unsub_rsp %d",
				((struct ble_unsubscribe_rsp *)msg)->status);
		TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		break;
	case MSG_ID_BLE_NOTIF_EVT:
		pr_info(LOG_MODULE_BLE,"Notif stat=%d conn=%p val_hdl=%d size %d",
				((struct ble_notification_data_evt *)msg)->status,
				((struct ble_notification_data_evt *)msg)->conn,
				((struct ble_notification_data_evt *)msg)->char_handle,
				((struct ble_notification_data_evt *)msg)->len);
		goto _free_msg;
		break;
#endif
	case MSG_ID_BLE_CONNECT_RSP:
	case MSG_ID_BLE_DISCONNECT_RSP:
	case MSG_ID_BLE_RSSI_RSP:
	case MSG_ID_BLE_SET_REMOTE_DATA_RSP:
	case MSG_ID_BLE_UPDATE_DATA_RSP:
		if(!_check_status(((struct ble_conn_rsp *)msg)->status, info_for_rsp->ctx)) {
			snprintf(answer, ANS_LENGTH, "conn %p", ((struct ble_conn_rsp *)msg)->conn);
			TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		}
		break;
	case MSG_ID_BLE_SET_NAME_RSP:
	case MSG_ID_BLE_START_ADV_RSP:
	case MSG_ID_BLE_STOP_ADV_RSP:
	case MSG_ID_BLE_PASSKEY_SEND_REPLY_RSP:
		if(!_check_status(((struct ble_rsp *)msg)->status, info_for_rsp->ctx)) {
			TCMD_RSP_FINAL(info_for_rsp->ctx, "0");
		}
		break;
	case MSG_ID_BLE_CLEAR_BONDS_RSP:
		if(!_check_status(((struct ble_rsp *)msg)->status, info_for_rsp->ctx)) {
			union ble_set_sec_params params;

			params.dev_status = BLE_SEC_ST_NO_BONDED_DEVICES;
			ble_set_security_status(info_for_rsp->ble_service_conn, BLE_SEC_DEVICE_STATUS,
					&params, info_for_rsp);
			goto _free_msg;
		}
		break;
	case MSG_ID_BLE_INIT_SVC_RSP:
		if(!_check_status(((struct ble_init_service_rsp *)msg)->status, info_for_rsp->ctx)) {
			ble_init_svc_rsp(msg);
			TCMD_RSP_FINAL(info_for_rsp->ctx, "0");
		}
		break;
	case MSG_ID_BLE_CONNECT_EVT:
		if (((struct ble_connect_evt *)msg)->status == BLE_STATUS_SUCCESS)
			_ble_service_tcmd_cb->conn = ((struct ble_connect_evt *)msg)->conn;
		goto _free_msg;
		break;
	case MSG_ID_BLE_DISCONNECT_EVT:
		_ble_service_tcmd_cb->conn = NULL;
		goto _free_msg;
		break;
	case MSG_ID_BLE_ADV_TO_EVT:
		goto _free_msg;
		break;
	case MSG_ID_BLE_GET_VERSION_RSP:
		if(!_check_status(((struct ble_version_rsp *)msg)->status, info_for_rsp->ctx))
			version_print(msg, info_for_rsp->ctx);
		break;
	case MSG_ID_BLE_RSSI_EVT:
		pr_debug(LOG_MODULE_BLE, "RSSI: %d dBm", ((struct ble_rssi_evt *)msg)->rssi);
		goto _free_msg;
		break;
	case MSG_ID_BLE_GET_INFO_RSP:
		if(!_check_status(((struct ble_get_info_rsp *)msg)->status, info_for_rsp->ctx))
			get_info_print(msg, info_for_rsp->ctx);
		break;
#ifdef CONFIG_TCMD_BLE_DEBUG
	case MSG_ID_BLE_DBG_RSP:
		struct ble_dbg_req_rsp *rsp = (struct ble_dbg_req_rsp *) msg;
		snprintf(answer, ANS_LENGTH, "ble dbg: %d/0x%x  %d/%0x", rsp->u0, rsp->u0, rsp->u1, rsp->u1);
		TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		break;
#endif // CONFIG_TCMD_BLE_DEBUG
	default:
		snprintf(answer, ANS_LENGTH, "Default cfw handler. ID = %d.\n",
			CFW_MESSAGE_ID(msg));
		TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		break;
	}
	bfree(info_for_rsp);
_free_msg:
	cfw_msg_free(msg);
}

/*
 * Returns handle BLE service.
 *
 * @param[in]   ctx   The context to pass back to responses
 */
static cfw_service_conn_t *_get_service_conn(struct tcmd_handler_ctx *ctx)
{
	cfw_service_conn_t *ble_service_conn = NULL;

	/* if handle is NON-NULL, the service is already open */
	if (_ble_service_tcmd_cb && _ble_service_tcmd_cb->ble_service_conn)
		return _ble_service_tcmd_cb->ble_service_conn;
	if (!(ble_service_conn = cproxy_connect(BLE_SERVICE_ID, _ble_tcmd_handle_msg,
			    NULL)))
		TCMD_RSP_ERROR(ctx, "Cannot connect to BLE Service !");

	_ble_service_tcmd_cb = balloc(sizeof(struct _ble_service_tcmd_cb), NULL);
	memset(_ble_service_tcmd_cb, 0, sizeof(struct _ble_service_tcmd_cb));
	_ble_service_tcmd_cb->ble_service_conn = ble_service_conn;
	_ble_service_tcmd_cb->conn = NULL;

	return ble_service_conn;
}

/*
 * Check if service is available and launch ble_test api
 *
 * @param[in]   ctx        The context to pass back to responses
 * @param       param      Input parameters for BLE operation
 */
static void _ble_test_exec(struct tcmd_handler_ctx *ctx, struct ble_test_cmd param)
{
	struct _info_for_rsp *info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	int32_t ret;

	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}

	info_for_rsp->ctx = ctx;
	info_for_rsp->test_opcode = param.mode;

	ret = ble_test(info_for_rsp->ble_service_conn, &param, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
}

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_version(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	if (argc != 2)
		goto print_help;

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		TCMD_RSP_ERROR(ctx, "");
		return;
	}
	info_for_rsp->ctx = ctx;

	ret = ble_service_get_version(info_for_rsp->ble_service_conn, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble version");
}
DECLARE_TEST_COMMAND(ble, version, tcmd_ble_version);


/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_enable(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	int8_t enable_flag;
	int32_t mode = 0;
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;
	struct ble_enable_config en_config;
	char *p_name = NULL;

	if (argc < _args.enable.argc - 2)
		goto print_help;

	if ((enable_flag = atoi(argv[_args.enable.enable_flag])) != 0) {
		if (argc < _args.enable.argc - 1)
			goto print_help;

		mode = atoi(argv[_args.enable.mode]);

		if (argc == _args.enable.argc)
			p_name = argv[_args.enable.name];
	}

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}

#ifdef CONFIG_BLE_APP
	ble_app_set_device_name((uint8_t *)p_name);
#endif

	info_for_rsp->ctx = ctx;
	info_for_rsp->enable_flag = enable_flag;

	en_config.options = mode;
	en_config.p_bda = NULL;

	en_config.sm_config.io_caps = BLE_GAP_IO_NO_INPUT_NO_OUTPUT;
	en_config.sm_config.options = BLE_GAP_BONDING;
	en_config.sm_config.key_size = 16;

	ret = ble_enable(info_for_rsp->ble_service_conn, enable_flag,
			&en_config, info_for_rsp);
	if (_api_check_status(ret, info_for_rsp))
		return;

	if (enable_flag == 1 && mode == 0)  {
		int client_events[] = {
				MSG_ID_BLE_CONNECT_EVT,
				MSG_ID_BLE_DISCONNECT_EVT,
				MSG_ID_BLE_ADV_TO_EVT,
				MSG_ID_BLE_RSSI_EVT,
				MSG_ID_BLE_NOTIF_EVT,
		};
		ret = cfw_register_events(info_for_rsp->ble_service_conn,
				client_events,
				sizeof(client_events) / sizeof(int),
				info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	}
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble enable <enable_flag> <mode> [name]");
}

DECLARE_TEST_COMMAND_ENG(ble, enable, tcmd_ble_enable);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void ble_tx_test(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	struct ble_test_cmd param = { 0 };

	if (argc < _args.tx_test.op + 1)
		goto print_help;

	if (!strcmp(argv[_args.tx_test.op], "stop")) {
		param.mode = BLE_TEST_END_DTM;
	} else if (!strcmp(argv[_args.tx_test.op], "start")
		   && _args.tx_test.argc == argc) {
		param.mode = BLE_TEST_START_DTM_TX;
		param.tx.freq = atoi(argv[_args.tx_test.freq]);
		param.tx.len = atoi(argv[_args.tx_test.len]);
		param.tx.pattern = atoi(argv[_args.tx_test.pattern]);
	} else
		goto print_help;

	_ble_test_exec(ctx, param);
	return;

print_help:
	TCMD_RSP_ERROR(ctx,
		       "Usage: ble tx_test start|stop <freq> <len> <pattern>");
}

DECLARE_TEST_COMMAND_ENG(ble, tx_test, ble_tx_test);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void ble_rx_test(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	struct ble_test_cmd param = { 0 };

	if (argc < _args.rx_test.op + 1)
		goto print_help;

	if (!strcmp(argv[_args.tx_test.op], "stop")) {
		param.mode = BLE_TEST_END_DTM;
	} else if (!strcmp(argv[_args.rx_test.op], "start")
		   && _args.rx_test.argc == argc) {
		param.mode = BLE_TEST_START_DTM_RX;
		param.rx.freq = atoi(argv[_args.rx_test.freq]);
	} else
		goto print_help;

	_ble_test_exec(ctx, param);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble rx_test start|stop <freq>");
}

DECLARE_TEST_COMMAND_ENG(ble, rx_test, ble_rx_test);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void ble_test_set_tx_pwr(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	struct ble_test_cmd param = { 0 };

	if (argc != _args.test_set_tx_pwr.argc)
		goto print_help;

	param.mode = BLE_TEST_SET_TXPOWER;
	param.tx_pwr.dbm = atoi(argv[_args.test_set_tx_pwr.dbm]);
	_ble_test_exec(ctx, param);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble test_set_tx_pwr <dbm>");
}

DECLARE_TEST_COMMAND_ENG(ble, test_set_tx_pwr, ble_test_set_tx_pwr);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void ble_test_carrier(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	struct ble_test_cmd param = { 0 };

	if (argc < 3)
		goto print_help;;

	if (!strcmp(argv[2], "stop")) {
		param.mode = BLE_TEST_END_DTM;
	} else if (!strcmp(argv[2], "start")
		   && 4 == argc) {
		param.mode = BLE_TEST_START_TX_CARRIER;
		param.tx.freq = atoi(argv[3]);
	} else
		goto print_help;

	_ble_test_exec(ctx, param);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble tx_carrier start|stop <freq>");
}
DECLARE_TEST_COMMAND_ENG(ble, tx_carrier, ble_test_carrier);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_set_name(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	if (argc != 3)
		goto print_help;

	char name[BLE_MAX_DEVICE_NAME + 1] = {};
	strncpy(name, argv[2], BLE_MAX_DEVICE_NAME);

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}

	info_for_rsp->ctx = ctx;

	ret = ble_set_name(info_for_rsp->ble_service_conn,
			(const uint8_t *) name, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble set_name <name>");
}

DECLARE_TEST_COMMAND_ENG(ble, set_name, tcmd_ble_set_name);

#ifdef CONFIG_TCMD_BLE_DEBUG
/*
 * Test command to add BLE service : ble add_service <uuid>.
 *
 * - <uuid> - (DIS) 180a, (BAS) 180f
 * - Example: ble add_service 180F
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */
void tcmd_ble_add_service(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct bt_uuid svc_uuid;

	if (argc != 3)
		goto print_help;

	svc_uuid.type = (2 == strlen(argv[2])/2) ? BT_UUID_16 : BT_UUID_128;
	switch (svc_uuid.type) {
	case BT_UUID_16:
		svc_uuid.u16 = strtol(argv[2], NULL, 16);
		break;
	case BT_UUID_128: {
		int i;

		for(i = 1; i <= (sizeof(svc_uuid.u128)/sizeof(uint32_t)); i++) {
			char* walk_ptr = (argv[2] + 2 * (16 - sizeof(uint32_t) * i));
			((uint32_t *) &(svc_uuid.u128))[i - 1]= strtoul(walk_ptr, NULL, 16);
			*walk_ptr = '\0';
		}
	}
	break;
	default:
		TCMD_RSP_ERROR(ctx, "Invalid UUID type.");
		return;
	}

	struct _info_for_rsp *info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}

	info_for_rsp->ctx = ctx;

	if (BT_UUID_16 == svc_uuid.type) {
		switch (svc_uuid.u16) {
		case BLE_SVC_UUID_DEVICE_INFORMATION_SERVICE:
			/* Received enable RSP from BLE Core. Start registering services */
			ret = ble_init_service_dis(info_for_rsp->ble_service_conn,
					(struct _ble_register_svc *)info_for_rsp);
			break;
		case BLE_SVC_UUID_BATTERY_SERVICE:
			ret = ble_init_service_bas(info_for_rsp->ble_service_conn,
					(struct _ble_register_svc *)info_for_rsp);
			break;
		default:
			ret = E_OS_ERR_NOT_SUPPORTED;
			break;
		}
	} else {
		/* 128 bit UUID proprietary services */
		ret = E_OS_ERR_NOT_SUPPORTED;
#if defined(CONFIG_SERVICES_BLE_ISPP)
		ret = ble_init_service_ispp(info_for_rsp->ble_service_handle,
				info_for_rsp);
#endif
	}
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble add_service <uuid>");
}

DECLARE_TEST_COMMAND_ENG(ble, add_service, tcmd_ble_add_service);
#endif // CONFIG_TCMD_BLE_DEBUG

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_set_value(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;

	if (argc != 7)
		goto print_help;

	struct _info_for_rsp *info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	struct bt_conn *conn = (void *)strtoul(argv[2], NULL, 0);
	uint8_t type = strcmp(argv[6], "h")? 10 : 16;
	uint8_t level = strtol(argv[5], NULL, type);

	/* TODO: call API based on UUID */
	ret = ble_service_update_bat_level(info_for_rsp->ble_service_conn,
			conn, level, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble set_value <conn_ref> <s_uuid> <c_uuid> <value> <base>");
}

DECLARE_TEST_COMMAND_ENG(ble, set_value, tcmd_ble_set_value);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_advertise(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	int arg_left = argc;
	struct ble_adv_params params = { 0 };
	uint8_t flags = BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL;
	/* BLE_GAP_APPEARANCE_TYPE_GENERIC_WATCH 192 */
	uint8_t appearance[2] = { 0xc0, 0x00 };
	uint8_t manuf_data[2] = { 0x02, 0x00 };
	struct bt_eir ad[] = {
			{
				.len = 2,
				.type = BT_EIR_FLAGS,
				.data = &flags,
			},
			{
				.len = 3,
				.type = BT_EIR_GAP_APPEARANCE,
				.data = appearance,
			},
			{
				.len = 3,
				.type = BT_EIR_MANUFACTURER_DATA,
				.data = manuf_data,
			},
			{
				.len = 0,
				.type = BLE_ADV_TYPE_COMP_LOCAL_NAME,
				.data = NULL,
			},
			{}
	};

	if (argc < 3)
		goto print_help;

	if (!strcmp(argv[2], "start")) {
		if (argc < 4)
			goto print_help;
	}

	arg_left -= 3;
	struct _info_for_rsp *info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	params.adv_type = BT_LE_ADV_IND;

	if (argc > 3) {
		params.options = atoi(argv[_args.advertise.options]) &
				BLE_ADV_OPTIONS_MASK;
		arg_left--;
	}

	if (arg_left) {
		ad[3].len = strlen(argv[_args.advertise.name]) + 1;
		ad[3].data = argv[_args.advertise.name];
	}

	params.p_ad = ad;

	if (!strcmp(argv[2], "start")) {
		ret = ble_start_advertisement(info_for_rsp->ble_service_conn,
				&params, info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	} else if (!strcmp(argv[2], "stop")) {
		ret = ble_stop_advertisement(info_for_rsp->ble_service_conn, info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	} else
		goto print_help;

	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble advertise <start|stop> "
			"[<start advertise option bits>] [name]");
}

DECLARE_TEST_COMMAND_ENG(ble, advertise, tcmd_ble_advertise);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */

void tcmd_ble_send_passkey(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;
	struct ble_gap_sm_passkey params;

	if (argc != 4)
		goto print_help;

	struct bt_conn *conn = (void *)strtoul(argv[2], NULL, 0);

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	memcpy(params.passkey, argv[3], BLE_PASSKEY_LEN);
	params.type = BLE_GAP_SM_PK_PASSKEY;

	ret = ble_send_passkey(info_for_rsp->ble_service_conn,
			conn, &params, info_for_rsp);

	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble key <conn_ref> <pass_key>");
}

DECLARE_TEST_COMMAND_ENG(ble, key, tcmd_ble_send_passkey);

#ifdef CONFIG_BLE_SM_IO_CAP_TEST
/* Test command to set security params : ble security IO_CAPABILITY OPTION KEY_SIZE.
 *
 * - Example: ble security 3 1 16
 * - where 3 is the io capability, 1 the options 16 the key_size
 * IO capability possible values:
 *    BLE_GAP_IO_DISPLAY_ONLY = 0
 *    BLE_GAP_IO_DISPLAY_YESNO = 1
 *    BLE_GAP_IO_KEYBOARD_ONLY = 2
 *    BLE_GAP_IO_NO_INPUT_NO_OUTPUT = 3
 *    BLE_GAP_IO_KEYBOARD_DISPLAY = 4
 * OPTION possible values:
 *    BLE_GAP_BONDING = 0x01
 *    BLE_GAP_MITM = 0x04
 *    BLE_GAP_OOB = 0x08
 * SIZE - Maximum Encryption Key Size in the range 7 to 16
 * - Return: negative: failure, 0: success
 *
 * @param[in]   argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */

void tcmd_ble_sm_config_params(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	if (argc != 5)
		goto print_help;

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_handle = _get_service_handle(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	struct ble_gap_sm_config_params params;

	params.io_caps = strtol(argv[2], NULL, 10);
	params.options = strtol(argv[3], NULL, 10);
	params.key_size = strtol(argv[4], NULL, 10);

	ret = ble_app_store_sm_cfg(&params);

	_api_check_status(ret, info_for_rsp);

	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble security <IO_CAPABILITY> <OPTION> <KEY_SIZE>");
}

DECLARE_TEST_COMMAND_ENG(ble, security, tcmd_ble_sm_config_params);
#endif

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_info(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	if (argc != 2)
		goto print_help;

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		TCMD_RSP_ERROR(ctx, "");
		return;
	}
	info_for_rsp->ctx = ctx;

	ret = ble_service_get_info(info_for_rsp->ble_service_conn,
			BLE_INFO_BDA_NAME_REQ, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble info");
}
DECLARE_TEST_COMMAND(ble, info, tcmd_ble_info);

#ifdef CONFIG_SERVICES_BLE_GATTC
/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_discover(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	if (argc != 5 && argc != 7)
		goto print_help;

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		TCMD_RSP_ERROR(ctx, "");
		return;
	}
	info_for_rsp->ctx = ctx;

	struct ble_discover_params params;
	params.uuid.type = BT_UUID_16;
	params.uuid.u16 = strtoul(argv[4], NULL, 0);
	if (argc == 7) {
		params.handle_range.start_handle = strtoul(argv[5], NULL, 0);
		params.handle_range.end_handle = strtoul(argv[6], NULL, 0);
	}
	else {
		params.handle_range.start_handle = 1;
		params.handle_range.end_handle = 0xFFFF;
	}
	params.conn = (void *)strtoul(argv[2], NULL, 0);
	params.type = strtoul(argv[3], NULL, 0);

	ret = ble_discover(info_for_rsp->ble_service_conn,
			&params, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble discover <conn_ref> <type> <uuid> [start_handle] [end_handle]");
}
DECLARE_TEST_COMMAND(ble, discover, tcmd_ble_discover);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_write(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;
	uint8_t data[4];
	unsigned long int value;

	if (argc != 6)
		goto print_help;

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		TCMD_RSP_ERROR(ctx, "");
		return;
	}
	info_for_rsp->ctx = ctx;

	struct ble_set_remote_data_params params;
	params.with_resp = true;                         /**< write with response*/
	params.conn = (void *)strtoul(argv[2], NULL, 0);  /**< Connection handle */
	params.char_handle = strtoul(argv[3], NULL, 0);  /**< Characteristic handle */
	params.offset = strtoul(argv[4], NULL, 0);

	value = strtoul(argv[5], NULL, 0);  /**< Data to write */
	data[0] = value;
	data[1] = value >> 8;
	data[2] = value >> 16;
	data[3] = value >> 24;

	ret = ble_set_remote_data(info_for_rsp->ble_service_conn,
			&params, sizeof(data), data, info_for_rsp);
	_api_check_status(ret, info_for_rsp);

	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble write <conn_ref> <handle> <offset> <value>");
}
DECLARE_TEST_COMMAND(ble, write, tcmd_ble_write);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_read(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	if (argc != 5)
		goto print_help;

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		TCMD_RSP_ERROR(ctx, "");
		return;
	}
	info_for_rsp->ctx = ctx;

	struct ble_get_remote_data_params params;

	params.conn = (void *)strtoul(argv[2], NULL, 0);  /**< Connection handle */
	params.char_handle = strtoul(argv[3], NULL, 0);  /**< Characteristic handle */
	params.offset = strtoul(argv[4], NULL, 0);      /**< Offset */

	ret = ble_get_remote_data(info_for_rsp->ble_service_conn,
			&params, info_for_rsp);
	_api_check_status(ret, info_for_rsp);

	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble read <conn_ref> <handle> <offset>");
}
DECLARE_TEST_COMMAND(ble, read, tcmd_ble_read);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_subscribe(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;
	struct ble_subscribe_params params;

	if (argc != 6)
		goto print_help;

	params.conn = (void *)strtoul(argv[2], NULL, 0);
	params.ccc_handle = atoi(argv[3]);
	params.value = atoi(argv[4]);
	params.value_handle = atoi(argv[5]);

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}

	info_for_rsp->ctx = ctx;

	ret = ble_subscribe(info_for_rsp->ble_service_conn,
				&params, info_for_rsp);

	_api_check_status(ret, info_for_rsp);

	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble subscribe <conn_ref> <ccc_handle> <value> <value_handle>");
}

DECLARE_TEST_COMMAND(ble, subscribe, tcmd_ble_subscribe);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_unsubscribe(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;
	struct ble_unsubscribe_params params;

	if (argc != 4)
		goto print_help;

	params.conn = (void *)strtoul(argv[2], NULL, 0);
	params.p_subscription = (void *)strtoul(argv[3], NULL, 0);

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}

	info_for_rsp->ctx = ctx;

	ret = ble_unsubscribe(info_for_rsp->ble_service_conn,
				&params, info_for_rsp);

	_api_check_status(ret, info_for_rsp);

	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble unsubscribe <conn_ref> <subscribe_ref>");
}

DECLARE_TEST_COMMAND(ble, unsubscribe, tcmd_ble_unsubscribe);

#endif

#ifdef CONFIG_TCMD_BLE_DEBUG
#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
/*
 * Test command to set synchronization on or off : ble set_sync.
 *
 * - Example: ble set_sync on
 * - Return: negative: failure, 0: success
 *
 */
void tcmd_ble_set_sync(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	if (argc != 3)
		goto print_help;
	if(!strcmp(argv[2], "on"))
	{
		ble_service_bas_set_battery_updates(true);
		TCMD_RSP_FINAL(ctx, "BLE BAS auto sync activated");
	}
	else if(!strcmp(argv[2], "off"))
	{
		ble_service_bas_set_battery_updates(false);
		TCMD_RSP_FINAL(ctx, "BLE BAS auto sync desactivated");
	}
	else
		goto print_help;
	return;
print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble set_sync <on|off>");
}

DECLARE_TEST_COMMAND_ENG(ble, set_sync, tcmd_ble_set_sync);
#endif // CONFIG_SERVICES_BLE_BAS_USE_BAT

/** Test command to start ble rssi reporting: ble rssi <start|stop> [<delta_dBm> <min_count>].
 *
 * - Example: ble rssi start 5 3
 * - The default option is delta_dBm 5 and min_count 3
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */
void tcmd_ble_rssi(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	uint32_t delta_dBm = 5;
	uint32_t min_count = 3;
	struct rssi_report_params conf = {0};

	if (argc < 3 || argc > 5)
		goto print_help;

	struct _info_for_rsp *info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	if (argc > 3)
		delta_dBm = atoi(argv[3]);

	if (argc > 4)
		min_count = atoi(argv[4]);

	if (!strcmp(argv[2], "start")) {
		conf.op = BLE_GAP_RSSI_ENABLE_REPORT;
		conf.delta_dBm = delta_dBm;
		conf.min_count = min_count;
		ret = ble_set_rssi_report(info_for_rsp->ble_service_conn, _ble_service_tcmd_cb->conn, &conf, info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	} else if (!strcmp(argv[2], "stop")) {
		conf.op = BLE_GAP_RSSI_DISABLE_REPORT;
		ret = ble_set_rssi_report(info_for_rsp->ble_service_conn, _ble_service_tcmd_cb->conn, &conf, info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	} else
		goto print_help;
	pr_debug(LOG_MODULE_BLE, "tcmd_ble_rssi, conn: %p svc_hdl %d",
			_ble_service_tcmd_cb->conn, info_for_rsp->ble_service_conn);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble rssi <start|stop> [<delta_dBm> <min_count>]");
}

DECLARE_TEST_COMMAND_ENG(ble, rssi, tcmd_ble_rssi);

void tcmd_ble_dbg(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	struct ble_dbg_req_rsp *msg = (struct ble_dbg_req_rsp*)
		cfw_alloc_message_for_service(info_for_rsp->ble_service_conn,
			MSG_ID_BLE_DBG_REQ, sizeof(*msg), info_for_rsp);
	msg->u0 = (argc >= 3)? strtoul(argv[2], NULL, 0) : 0;
	msg->u1 = (argc >= 4)? strtoul(argv[3], NULL, 0) : 0;

	ret = cfw_send_message(msg);
	_api_check_status(ret, info_for_rsp);
}
DECLARE_TEST_COMMAND_ENG(ble, dbg, tcmd_ble_dbg);
#endif // CONFIG_TCMD_BLE_DEBUG

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_clear_bonds(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;

	struct _info_for_rsp *info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	ret = ble_clear_bonds(info_for_rsp->ble_service_conn, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
}
DECLARE_TEST_COMMAND_ENG(ble, clear, tcmd_ble_clear_bonds);

#if defined(CONFIG_SERVICES_BLE_CENTRAL)
/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_connect(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;
	bt_addr_le_t bd_addr;
	struct bt_le_conn_param conn_params;

	if (argc != 9)
		goto print_help;

	conn_params.interval_min = 0x0018;  /* 30 ms    */
	conn_params.interval_max = 0x0028;  /* 50 ms    */
	conn_params.timeout = 100;
	conn_params.latency = 8;

	bd_addr.type = strtol(argv[2], NULL, 16);
	if ( bd_addr.type > 1) {
		TCMD_RSP_ERROR(ctx, "Invalid BDA Type");
		return;
	}

	bd_addr.val[5] = strtol(argv[3], NULL, 16);
	bd_addr.val[4] = strtol(argv[4], NULL, 16);
	bd_addr.val[3] = strtol(argv[5], NULL, 16);
	bd_addr.val[2] = strtol(argv[6], NULL, 16);
	bd_addr.val[1] = strtol(argv[7], NULL, 16);
	bd_addr.val[0] = strtol(argv[8], NULL, 16);

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}

	info_for_rsp->ctx = ctx;

	ret = ble_connect(info_for_rsp->ble_service_conn,
				&bd_addr, &conn_params, info_for_rsp);

	_api_check_status(ret, info_for_rsp);

	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble connect <BDA type> <BDA value (without ':')>");
}

DECLARE_TEST_COMMAND(ble, connect, tcmd_ble_connect);

/*
 * documentation should be maintained in: wearable_device_sw/doc/test_command_syntax.md
 */
void tcmd_ble_disconnect(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	if (argc != 3)
		goto print_help;

	struct bt_conn *conn = (void *)strtoul(argv[2], NULL, 0);

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	ret = ble_disconnect(info_for_rsp->ble_service_conn, conn, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble disconnect <conn_ref>");
}

DECLARE_TEST_COMMAND(ble, disconnect, tcmd_ble_disconnect);

#endif

/* @} */
