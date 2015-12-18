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
	uint16_t conn_h;
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

/*
 * Return error string for status.
 *
 * @param       status
 * @return      error string
 */
static uint8_t * ble_start_advertise_err_code(ble_status_t status)
{
	switch (status) {
		case BLE_STATUS_WRONG_STATE:
			return (uint8_t *)"ADV ongoing, only ADV data is updated";
			break;
		case BLE_STATUS_ERROR_PARAMETER:
			return (uint8_t *)"Incorrect ADV parameters";
			break;
		default:
			return (uint8_t *)"ADV Error";
			break;
	}
}

/*
 * Display message if status is KO.
 *
 * @param       status return value in RSP
 * @param[in]   ctx    The context to pass back to responses
 * @return
 */
static int32_t _check_start_adv_status(int32_t status, struct tcmd_handler_ctx *ctx)
{
	if (status != BLE_STATUS_SUCCESS) {
		char answer[ANS_LENGTH];
		snprintf(answer, ANS_LENGTH, "KO %s", ble_start_advertise_err_code(status));
		TCMD_RSP_ERROR(ctx, answer);
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

	if (resp->info_type == BLE_INFO_BDA_NAME_REQ) {
		char buf[ANS_LENGTH];

		snprintf(buf, ANS_LENGTH, "BDA type: %d, address: %x:%x:%x:%x:%x:%x",
				resp->info_params.bda_name_params.bda.type,
				resp->info_params.bda_name_params.bda.addr[5],
				resp->info_params.bda_name_params.bda.addr[4],
				resp->info_params.bda_name_params.bda.addr[3],
				resp->info_params.bda_name_params.bda.addr[2],
				resp->info_params.bda_name_params.bda.addr[1],
				resp->info_params.bda_name_params.bda.addr[0]);
		TCMD_RSP_PROVISIONAL(ctx, buf);

		snprintf(buf, ANS_LENGTH, "GAP device name: %s",
				(char *)resp->info_params.bda_name_params.name);
		TCMD_RSP_FINAL(ctx, buf);
	}

}

static void _ble_tcmd_handle_msg(struct cfw_message *msg, void *data)
{
	char answer[ANS_LENGTH];
	struct _info_for_rsp *info_for_rsp = msg->priv;

	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_BLE_DTM_RSP:
		if (!_check_status(((struct ble_dtm_result_msg *)msg)->status, info_for_rsp->ctx)) {
			if (info_for_rsp->test_opcode == BLE_TEST_END_DTM)
				snprintf(answer, ANS_LENGTH,
					 "RX results: Mode = %u. Nb = %u",
					 ((struct ble_dtm_result_msg *)msg)->result.mode,
					 ((struct ble_dtm_result_msg *)msg)->result.nb);
			else
				answer[0] = '\0';
			TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		}
		break;
	case MSG_ID_BLE_ENABLE_RSP:
		if (!_check_status(((struct ble_generic_msg *)msg)->status, info_for_rsp->ctx)) {
			if (0 == info_for_rsp->enable_flag && _ble_service_tcmd_cb) {
				if (_ble_service_tcmd_cb->ble_service_conn) {
					cproxy_disconnect(_ble_service_tcmd_cb->ble_service_conn);
					_ble_service_tcmd_cb->ble_service_conn = NULL;
				}
				bfree(_ble_service_tcmd_cb);
			}
			TCMD_RSP_FINAL(info_for_rsp->ctx, "OK 0");
		}
		break;
	case MSG_ID_BLE_START_ADV_RSP:
		if(!_check_start_adv_status(((struct ble_generic_msg *)msg)->status,
				info_for_rsp->ctx))
			TCMD_RSP_FINAL(info_for_rsp->ctx, "OK 0");
		break;
	case MSG_ID_BLE_SET_NAME_RSP:
	case MSG_ID_BLE_DISCONNECT_RSP:
	case MSG_ID_BLE_STOP_ADV_RSP:
	case MSG_ID_BLE_UPDATE_DATA_RSP:
	case MSG_ID_BLE_RSSI_RSP:
		if(!_check_status(((struct ble_generic_msg *)msg)->status, info_for_rsp->ctx))
			TCMD_RSP_FINAL(info_for_rsp->ctx, "OK 0");
		break;
	case MSG_ID_BLE_CLEAR_BONDS_RSP:
		if(!_check_status(((struct ble_generic_msg *)msg)->status, info_for_rsp->ctx)) {
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
			TCMD_RSP_FINAL(info_for_rsp->ctx, "OK 0");
		}
		break;
	case MSG_ID_BLE_CONNECT_EVT:
		_ble_service_tcmd_cb->conn_h = ((struct ble_connect_evt *)msg)->conn_handle;
		goto _free_msg;
		break;
	case MSG_ID_BLE_DISCONNECT_EVT:
		_ble_service_tcmd_cb->conn_h = BLE_SVC_GAP_HANDLE_INVALID;
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
	case MSG_ID_BLE_DBG_RSP:;
		struct ble_dbg_msg *rsp = (struct ble_dbg_msg *) msg;
		snprintf(answer, ANS_LENGTH, "ble dbg: %d/0x%x  %d/%0x", rsp->u0, rsp->u0, rsp->u1, rsp->u1);
		TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		break;
#endif // CONFIG_TCMD_BLE_DEBUG
	case MSG_ID_BLE_PASSKEY_SEND_REPLY_RSP:
		pr_info(LOG_MODULE_BLE,"passkey sending status 0x%x",((struct ble_rsp *)msg)->status);
		break;
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
	_ble_service_tcmd_cb->conn_h = BLE_SVC_GAP_HANDLE_INVALID;

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
 * Test command to retrieve BLE version: ble version.
 *
 * - Return:       negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */
void tcmd_ble_version(int argc, char **argv, struct tcmd_handler_ctx *ctx)
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
 * Test command to start BLE enable : ble enable <enable_flag> <mode> [name].
 *
 * - <enable_flag>: 0 - disable; 1 - enable;
 * - <mode>:        0, 1, 2, ... - @ref BLE_OPTIONS
 * - [name]:        Local Name of the Local BLE device
 * - Return:       negative: failure, 0: success
 * - Example 1: ble enable 1 0 AppCTB - enables BLE with name = "AppCTB"
 * in normal mode; mode = 0 - Normal Mode; mode = 1 - Test Mode
 * - Example 2: ble enable 1 0 - enables BLE in normal mode (test mode OFF)
 * - Example 3: ble enable 0 - disables BLE
 * - Example 4: ble enable 1 1 "DtmCTB"- enable BLE test mode (dtm) with name =
 * "DtmCTB"
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */
void tcmd_ble_enable(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	int8_t enable_flag;
	int32_t mode = 0;
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;
	struct ble_enable_config en_config;
	char name[BLE_MAX_DEVICE_NAME + 1] = {'\0'};
	const struct ble_gap_connection_params conn_params =
		    {MSEC_TO_1_25_MS_UNITS(80), MSEC_TO_1_25_MS_UNITS(150), 0, MSEC_TO_10_MS_UNITS(6000)};

	if (argc < _args.enable.argc - 2)
		goto print_help;

	if ((enable_flag = atoi(argv[_args.enable.enable_flag])) != 0) {
		if (argc < _args.enable.argc - 1)
			goto print_help;

		mode = atoi(argv[_args.enable.mode]);

		if (argc == _args.enable.argc)
			strncpy(name, argv[_args.enable.name], BLE_MAX_DEVICE_NAME);
	}

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}

	info_for_rsp->ctx = ctx;
	info_for_rsp->enable_flag = enable_flag;

	en_config.options = mode;
	en_config.p_name = (uint8_t *)name;
	en_config.p_bda = NULL;
	en_config.appearance = BLE_GAP_APPEARANCE_TYPE_GENERIC_WATCH;
	en_config.central_conn_params = en_config.peripheral_conn_params = conn_params;

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
 * Test command to start BLE dtm tx : ble tx_test start|stop <freq> <len> <pattern>.
 *
 * - <freq>: Phys. channel no - actual frequency = 2024 + freq x 2Mhz, freq < 40
 * - <len> : Payload length
 * - <pattern>: Packet type
 * - Example 1: ble tx_test start 20 4 0
 *		pattern = 0 - PRBS9, 1 - 0X0F, 2 - 0X55, 255 - Bit pattern 1 repeated
 * - Example 2: ble tx_test stop
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
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
 * Test command to start BLE dtm rx : ble rx_test start|stop <freq>.
 *
 * - <freq>: 2024 + freq x 2Mhz, freq < 40
 * - Example 1: ble rx_test start 20
 * - Example 2: ble rx_test stop
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
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
 * Test command to set BLE tx power: ble test_set_tx_pwr <dbm>.
 *
 * - <dbm>: -XYZdBm...+4dBm
 * - Example: ble test_set_tx_pwr -16
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
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

/* Test command to start/stop ble carrier test: ble tx_carrier start|stop <freq>.
 *
 * - <freq>: 2024 + freq x 2Mhz, freq < 40
 * - Example: ble tx_carrier start 10
 * - Example: ble tx_carrier stop
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
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
 * Test command to set the name of BLE device : ble set_name <name>.
 *
 * - Example: ble set_name TestCTB
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
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
 * Test command to BLE set battery level : ble set_value <conn_handle> <s_uuid> <c_uuid> <value> <type>.
 *
 * - Example: ble set_val 3 180f 2a19 64 h
 * - 3    - handle of connection, maybe 65535 (0xffff) if disconnected
 * - 180f - battery service
 * - 2a19 - uuid characteristic to update
 * - 64   - hexa value of 100% battery level
 * - h    - hexa type for value=64 (use d for decimal)
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
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

	uint16_t conn_h = strtol(argv[2], NULL, 10);
	uint8_t type = strcmp(argv[6], "h")? 10 : 16;
	uint8_t level = strtol(argv[5], NULL, type);

	/* TODO: call API based on UUID */
	ret = ble_service_update_bat_level(info_for_rsp->ble_service_conn,
			conn_h, level, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble set_value <conn_handle> <s_uuid> <c_uuid> <value> <type>");
}

DECLARE_TEST_COMMAND_ENG(ble, set_value, tcmd_ble_set_value);

/*
 * Test command to start BLE advertisement: ble advertise <start|stop>
 * [<advertise option bits>] [name].
 *
 * - Advertise option bits selects the interval and timeout as defined at: \ref BLE_ADV_OPTIONS
 * - Bit 0: Slow advertisement interval
 * - Bit 1: Ultra fast advertisement interval
 * - Bit 2: Short advertisement timeout
 * - Bit 3: NO advertisement timeout
 * - Bit 4: Non-discoverable advertisement, minimum advertisement data
 *
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */
void tcmd_ble_advertise(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	int arg_left = argc;
	struct ble_adv_data_params params;
	uint8_t ad[BLE_MAX_ADV_SIZE];
	uint32_t options = BLE_NO_ADV_OPT;
	uint8_t *p = ad;
	uint8_t *p_name = NULL;

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

	if (argc > 3) {
		options = atoi(argv[_args.advertise.options]);
		arg_left--;
	}

	if (arg_left) {
		p_name = balloc(strlen(argv[_args.advertise.name] + 1), NULL);
		strcpy(p_name, argv[_args.enable.name]);
	}

	params.adv_type = BLE_GAP_ADV_TYPE_ADV_IND;
	params.sd_len = 0;
	params.ad_len = 0;
	params.p_le_addr = NULL;
	params.p_sd = NULL;
	params.p_ad = ad;

	uint8_t flag = (options & BLE_NON_DISC_ADV) ?
			BLE_SVC_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED :
			BLE_SVC_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
	p += ble_enc_adv_flags(p, flag);

	p += ble_enc_adv_appearance(p, BLE_GAP_APPEARANCE_TYPE_GENERIC_WATCH);

	p += ble_enc_adv_manuf(p, INTEL_MANUFACTURER, NULL, 0);

	if (p_name) {
		p += ble_enc_adv_name(p, params.ad_len, p_name,
			strlen((char *)p_name));
	}

	params.ad_len = p - params.p_ad;

	if (!strcmp(argv[2], "start")) {
		ret = ble_start_advertisement(info_for_rsp->ble_service_conn,
				options, &params, info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	} else if (!strcmp(argv[2], "stop")) {
		ret = ble_stop_advertisement(info_for_rsp->ble_service_conn, info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	} else
		goto print_help;

	if (p_name)
		bfree(p_name);

	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble advertise <start|stop> "
			"[<start advertise option bits>] [name]");
}

DECLARE_TEST_COMMAND_ENG(ble, advertise, tcmd_ble_advertise);

/* Test command to send pass key : ble key <conn_handle> <PASKEY>.
 *
 * - Example: ble key 123456
 * - where 123456 is the key displayed in companion app
 * - Return: negative: failure, 0: success
 *
 * @param[in]   argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */

void tcmd_ble_send_passkey(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;
	struct ble_gap_sm_passkey params;

	if (argc != 4)
		goto print_help;

	uint16_t conn_handle = strtol(argv[2], NULL, 10);

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	memcpy(params.passkey, argv[3], BLE_PASSKEY_LEN);
	params.type = BLE_GAP_SM_PASSKEY;

	ret = ble_send_passkey(info_for_rsp->ble_service_conn,
			conn_handle, &params,info_for_rsp);

	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble key <conn_handle> <PASSKEY 6 digits>");
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
 * Test command to retrieve ble related infos
 *
 * Currently supported infos
 * - ble info: BDA and current name
 *
 * - Return:       negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */
void tcmd_ble_info(int argc, char **argv, struct tcmd_handler_ctx *ctx)
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

#ifdef CONFIG_TCMD_BLE_DEBUG
/*
 * Test command to disconnect : ble disconnect <conn_handle>.
 *
 * - Example: ble disconnect 3
 * - 3 is the connection handle found in logs when remote device connect
 * to the device
 * - Return: negative: failure, 0: success
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
 */
void tcmd_ble_disconnect(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	if (argc != 3)
		goto print_help;

	uint16_t conn_handle = strtol(argv[2], NULL, 16);

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	ret = ble_disconnect(info_for_rsp->ble_service_conn,
			(const uint16_t) conn_handle, info_for_rsp);
	_api_check_status(ret, info_for_rsp);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble disconnect <conn_handle>");
}

DECLARE_TEST_COMMAND_ENG(ble, disconnect, tcmd_ble_disconnect);

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
		conf.conn_hdl = _ble_service_tcmd_cb->conn_h;
		conf.op = BLE_GAP_RSSI_ENABLE_REPORT;
		conf.delta_dBm = delta_dBm;
		conf.min_count = min_count;
		ret = ble_set_rssi_report(info_for_rsp->ble_service_conn, &conf, info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	} else if (!strcmp(argv[2], "stop")) {
		conf.conn_hdl = _ble_service_tcmd_cb->conn_h;
		conf.op = BLE_GAP_RSSI_DISABLE_REPORT;
		ret = ble_set_rssi_report(info_for_rsp->ble_service_conn, &conf, info_for_rsp);
		_api_check_status(ret, info_for_rsp);
	} else
		goto print_help;
	pr_debug(LOG_MODULE_BLE, "tcmd_ble_rssi, conn_hdl: %d svc_hdl %d",
			_ble_service_tcmd_cb->conn_h, info_for_rsp->ble_service_conn);
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: ble rssi <start|stop> [<delta_dBm> <min_count>]");
}

DECLARE_TEST_COMMAND_ENG(ble, rssi, tcmd_ble_rssi);

void tcmd_ble_dbg(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	struct _info_for_rsp *info_for_rsp;

	info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	if (!(info_for_rsp->ble_service_conn = _get_service_conn(ctx))) {
		bfree(info_for_rsp);
		return;
	}
	info_for_rsp->ctx = ctx;

	struct ble_dbg_msg *msg = (struct ble_dbg_msg*)
		cfw_alloc_message_for_service(info_for_rsp->ble_service_conn,
			MSG_ID_BLE_DBG_REQ, sizeof(*msg), info_for_rsp);
	msg->u0 = (argc >= 3)? strtoul(argv[2], NULL, 0) : 0;
	msg->u1 = (argc >= 4)? strtoul(argv[3], NULL, 0) : 0;

	ret = cfw_send_message(msg);
	_api_check_status(ret, info_for_rsp);
}
DECLARE_TEST_COMMAND(ble, dbg, tcmd_ble_dbg);
#endif // CONFIG_TCMD_BLE_DEBUG

/*
 * Test command to clear BLE bondings
 *
 *
 * @param[in]	argc	Number of arguments in the Test Command (including group and name),
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The context to pass back to responses
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

/* @} */
