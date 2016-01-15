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

#ifndef __BLE_SERVICE_GAP_H__
#define __BLE_SERVICE_GAP_H__

#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "infra/version.h"
#include "ble_service.h"
// For bt_addr_le_t
#include "zephyr/bluetooth/hci.h"
/**
 * @defgroup ble_core_gap_api BLE Core GAP/GATT APIs
 *
 * BLE Core GAP/GATT APIs used by BLE service.
 *
 * @ingroup ble_service
 * @{
 */

/** Macro to convert milliseconds to a specific unit */
#define MSEC_TO_0_625_MS_UNITS(TIME) (((TIME) * 1000) / 625)
#define MSEC_TO_1_25_MS_UNITS(TIME) (((TIME) * 1000) / 1250)
#define MSEC_TO_10_MS_UNITS(TIME) ((TIME) / 10)

#define BLE_PASSKEY_LEN 6

/**
 * BLE GAP Status return codes.
 */
enum BLE_SVC_GAP_STATUS_CODES {
	BLE_SVC_GAP_STATUS_SUCCESS = BLE_STATUS_SUCCESS, /**< GAP success */
	BLE_SVC_GAP_STATUS_ERROR = BLE_STATUS_GATT_BASE, /**< Generic GAP error */
	BLE_SVC_GAP_STATUS_INVALID_UUID_LIST,		 /**< Invalid UUID list provided (e.g. advertisement) */
	/* TODO: add more status codes */
};

#define BLE_GAP_SEC_RAND_LEN    8		/**< Random Security number length (64 bits) */
#define BLE_GAP_SEC_MAX_KEY_LEN 16		/**< Maximum security key len (LTK, CSRK) */

/**
 * Device GAP name characteristic write permission.
 *
 * If the characteristic shall be writable, use a combination of the values
 * defined in @ref BLE_GAP_SEC_MODES
 */
#define BLE_DEVICE_NAME_WRITE_PERM GAP_SEC_NO_PERMISSION

/**@brief Structure containing list of bonded devices. */
struct ble_bonded_devices
{
#ifdef CONFIG_TCMD_BLE_DEBUG
	bt_addr_le_t    addrs[8];      /**< Pointer to an array of device address pointers, pointing to addresses to be used in whitelist. NULL if none are given. */
#endif
	uint8_t       addr_count;      /**< Count of device addresses in array. */
};

/**
 * Connection requested parameters.
 */
struct ble_gap_connection_params {
	uint16_t interval_min;	/**< minimal connection interval: range 0x0006 to 0x0c80 (unit 1.25ms) */
	uint16_t interval_max;	/**< maximum connection interval: range 0x0006 to 0x0c80 must be bigger then min! */
	uint16_t slave_latency;	/**< maximum connection slave latency: 0x0000 to 0x01f3 */
	uint16_t link_sup_to;	/**< link supervision timeout: 0x000a to 0x0c80 (unit 10ms) */
};

/**
 * Security manager configuration parameters.
 *
 * options and io_caps will define there will be a passkey request or not.
 * It is assumed that io_caps and options are compatible.
 */
struct ble_gap_sm_config_params {
	uint8_t options;	/**< Security options (@ref BLE_GAP_SM_OPTIONS) */
	uint8_t io_caps;	/**< I/O Capabilities to allow passkey exchange (@ref BLE_GAP_IO_CAPABILITIES) */
	uint8_t key_size;	/**< Maximum encryption key size (7-16) */
};

/**
 * Advertising types, see BT spec vol 6, Part B, chapter 2.3.
 */
enum GAP_ADV_TYPES {
	ADV_IND = 0x00,		/**< Connectable undirected advertising */
	ADV_DIRECT_IND = 0x01,	/**< Connectable high duty cycle advertising */
	ADV_NONCONN_IND = 0x02,	/**< Non connectable undirected advertising */
	ADV_SCAN_IND = 0x06,	/**< Scannable undirected advertising */
	ADV_SCAN_RSP = 0x81,	/**< Scan response, only a return value in @ref ble_gap_adv_data_evt */
	ADV_RESERVED		/* keep last */
};

/**
 * Advertisement options.
 */
enum BLE_GAP_ADV_OPTIONS {
	BLE_GAP_OPT_ADV_DEFAULT = 0,		/**< no specific option */
	BLE_GAP_OPT_ADV_WHITE_LISTED = 0x02	/**< use white list and only report whitelisted devices */
};

/**
 * LE security modes.
 *
 * see BT spec  PART C, 10.2
 *
 * - Security mode 1
 *   - Level 1: No security at all (service may use data signing)
 *   - Level 2: Unauthenticated (no MITM protection pairing with encryption
 *   - Level 3: Authenticated (MITM protection) pairing with encryption
 *   - Level 4: Authenticated (MITM protection) LE Secure Connection wi
 *
 * - Security mode 2 (data signing)
 *   - Level 1: Unauthenticated pairing with data signing
 *   - Level 2: Authenticated (MITM protection) with data signing
 */
enum BLE_GAP_SEC_MODES {
	GAP_SEC_NO_PERMISSION = 0,	/**< No access permitted. */
	GAP_SEC_LEVEL_1,
	GAP_SEC_LEVEL_2,
	GAP_SEC_LEVEL_3,
	GAP_SEC_LEVEL_4,
	GAP_SEC_MODE_1 = 0x10,
	GAP_SEC_MODE_2 = 0x20		/**< only used for data signing, level 1 or 2 */
};

/**
 * GAP security manager options for bonding/authentication procedures, see Vol 3: Part H, 3.5.
 */
enum BLE_GAP_SM_OPTIONS {
	BLE_GAP_BONDING = 0x01,		/**< SMP supports bonding */
	BLE_GAP_MITM = 0x04,		/**< SMP requires Man In The Middle protection */
	BLE_GAP_OOB = 0x08		/**< SMP supports Out Of Band data */
};

/**
 * IO capabilities, see Vol 3: PART H, 3.5.
 */
enum BLE_GAP_IO_CAPABILITIES {
	BLE_GAP_IO_DISPLAY_ONLY = 0,
	BLE_GAP_IO_DISPLAY_YESNO = 1,
	BLE_GAP_IO_KEYBOARD_ONLY = 2,
	BLE_GAP_IO_NO_INPUT_NO_OUTPUT = 3,
	BLE_GAP_IO_KEYBOARD_DISPLAY = 4
};

/**
 * Security manager pairing parameters.
 */
struct ble_gap_sm_pairing_params {
	uint8_t auth_level;	/**< authentication level see @ref BLE_GAP_SM_OPTIONS */
};

/**
 * Security manager passkey type.
 */
enum BLE_GAP_SM_PASSKEY_TYPE {
	BLE_GAP_SM_PK_NONE = 0, /**< No key (may be used to reject). */
	BLE_GAP_SM_PK_PASSKEY,  /**< Security data is a 6-digit passkey. */
	BLE_GAP_SM_PK_OOB,         /**< Security data is 16 bytes of OOB data */
};

/**
 * Security reply to incoming security request.
 */
struct ble_gap_sm_passkey {
	uint8_t type;			/**< Security data type in this reply @ref BLE_GAP_SM_PASSKEY_TYPE */
	union {
		uint8_t passkey[6];	/**< 6 digits (string) */
		uint8_t oob[16];	/**< 16 bytes of OOB security data */
	};
};

/**
 * RSSI operation definition.
 */
enum BLE_GAP_RSSI_OPS {
	BLE_GAP_RSSI_DISABLE_REPORT = 0,
	BLE_GAP_RSSI_ENABLE_REPORT
};

/**
 * GAP security manager status codes.
 */
enum BLE_GAP_SM_STATUS {
	BLE_GAP_SM_ST_START_PAIRING,	/*< Pairing has started */          //!< BLE_GAP_SM_ST_START_PAIRING
	BLE_GAP_SM_ST_BONDING_COMPLETE,	/*< Bonding has completed */     //!< BLE_GAP_SM_ST_BONDING_COMPLETE
	BLE_GAP_SM_ST_LINK_ENCRYPTED,	/*< Link is encrypted */           //!< BLE_GAP_SM_ST_LINK_ENCRYPTED
	BLE_GAP_SM_ST_SECURITY_UPDATE,	/*< Link keys have been updated *///!< BLE_GAP_SM_ST_SECURITY_UPDATE
};

enum BLE_SVC_GAP_TIMEOUT_REASON {
	BLE_SVC_GAP_TO_ADV,	/**< Advertisement Stopped. */
	BLE_SVC_GAP_TO_SEC_REQ,	/**< Security Request took too long. */
	BLE_SVC_GAP_TO_SCAN,	/**< Scanning stopped. */
	BLE_SVC_GAP_TO_CONN,	/**< Connection Link timeout. */
};

/**
 * Advertisement data structure (central role).
 */
struct ble_gap_adv_data_evt {
	bt_addr_le_t remote_bda;/**< address of remote device */
	int8_t rssi;		/**< signal strength compared to 0 dBm */
	uint8_t type;		/**< type of advertisement data or scan response @ref GAP_ADV_TYPES */
	uint8_t len;		/**< length of advertisement data or scan response data */
	uint8_t data[];		/**< Advertisement or scan response data */
};

/**
 * Connection Parameter update request event.
 */
struct ble_gap_conn_param_update_req_evt {
	struct ble_gap_connection_params param;
};

/**
 * RSSI report parameters
 */
struct rssi_report_params {
	uint8_t op;             /**< RSSI operation @ref BLE_GAP_RSSI_OPS */
	uint8_t delta_dBm;      /**< minimum RSSI dBm change to report a new RSSI value */
	uint8_t min_count;      /**< number of delta_dBm changes before sending a new RSSI report */
};

/** Test Mode opcodes. */
enum TEST_OPCODE {
	BLE_TEST_INIT_DTM = 0x01,	/**< Put BLE controller in HCI UART DTM test mode */
	BLE_TEST_START_DTM_RX = 0x1d,	/**< LE receiver test HCI opcode */
	BLE_TEST_START_DTM_TX = 0x1e,	/**< LE transmitter test HCI opcode */
	BLE_TEST_END_DTM = 0x1f,	/**< End LE DTM TEST */
	/* vendor specific commands start at 0x80 */
	BLE_TEST_SET_TXPOWER = 0x80,	/**< Set Tx power. To be called before start of tx test */
	BLE_TEST_START_TX_CARRIER,	/**< Start Tx Carrier Test */
};
/**
 * Rx direct test mode data structure.
 */
struct ble_dtm_rx_test {
	uint8_t freq;		/**< rf channel 0x00 - 0x27, resulting F = 2402 MHz + [freq * 2 MHz] */
};

/**
 * Tx direct test mode data structure
 */
struct ble_dtm_tx_test {
	uint8_t freq;		/**< rf channel 0x00 - 0x27 where resulting F = 2402 + [freq * 2 MHz] */
	uint8_t len;		/**< length of test data payload for each packet */
	uint8_t pattern;	/**< packet payload pattern type, 0x00 - 0x02 mandatory */
};

/**
 * Tx power settings data structure.
 */
struct ble_set_txpower {
	int8_t dbm;		/**< Tx power level to set (e.g. -30: -30 dBm). Depends on BLE Controller */
};

/**
 * RX test result data.
 */
struct ble_dtm_test_result {
	uint16_t mode;
	uint16_t nb;
};

/**
 * Direct Test mode command params
 */
struct ble_test_cmd {
	uint8_t mode;		/**< test mode to execute @ref TEST_OPCODE */
	union {
		struct ble_dtm_rx_test rx;
		struct ble_dtm_tx_test tx;
		struct ble_set_txpower tx_pwr;	/**< Tx power to use for Tx tests. */
	};
};

/** @} */

#endif /* __BLE_SERVICE_GAP_H__ */
