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
#include "ble_service_msg.h"
#include "ble_service.h"

// Forward declarations
struct ble_gap_sm_key_reply_req_msg;

/**
 * @defgroup ble_core_service_gap_api BLE Core Service GAP/GATT APIs
 *
 * BLE Core service GAP/GATT APIs used by BLE service.
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

/**
 * BLE GAP addr types.
 *
 * BLE GAP supported address types
 */
enum BLE_ADDR_TYPES {
	BLE_ADDR_PUBLIC = 0,				/**< BD address assigned by IEEE */
	BLE_ADDR_PRIVATE_RANDOM_STATIC,			/**< Random address */
	BLE_ADDR_RRIVATE_RANDOM_PRIVATE_RESOLVABLE,	/**< Resolvable Private Random address */
	BLE_ADDR_PRIVATE_RANDOM_PRIVATE_NONRESOLVABLE	/**< Non-resolvable Private Random address */
};

/**
 * BT/BLE address Length.
 */
#define BLE_ADDR_LEN 6

#define BLE_GAP_SEC_RAND_LEN    8		/**< Random Security number length (64 bits) */
#define BLE_GAP_SEC_MAX_KEY_LEN 16		/**< Maximum security key len (LTK, CSRK) */

#define BLE_SVC_GAP_HANDLE_INVALID 0xffff	/**< Invalid GAP connection handle */

/**
 * BT/BLE name Length
 */

#define BLE_DEVICE_NAME_LEN     20 /**< Max BLE device name length, spec size: 248 */
/**
 * Device GAP name characteristic write permission.
 *
 * If the characteristic shall be writable, use a combination of the values
 * defined in @ref BLE_GAP_SEC_MODES
 */
#define BLE_DEVICE_NAME_WRITE_PERM GAP_SEC_NO_PERMISSION

typedef struct {
	uint8_t type;			/**< BLE Address type @ref BLE_ADDR_TYPES */
	uint8_t addr[BLE_ADDR_LEN];	/**< BD address, little endian format */
} ble_addr_t;

/**
 * GAP device roles.
 */
enum BLE_ROLES {
	BLE_ROLE_INVALID = 0,
	BLE_ROLE_PERIPHERAL = 0x01,
	BLE_ROLE_CENTRAL = 0x02
};

typedef uint8_t ble_role_t;

struct ble_enable_rsp;
struct ble_version_rsp;
struct ble_start_advertisement_req_msg;
struct ble_disconnect_rsp;
struct ble_gap_sm_key_reply_req_msg;
struct ble_conn_update_rsp;
struct ble_enable_req_msg;
struct ble_dbg_msg;

/**
 * Generic BLE Status Response.
 * Short status response for commands not returning any additional data
 */
struct ble_rsp {
	struct cfw_message header;	/**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;		/**< Response status @ref BLE_STATUS */
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
 * Connection values.
 */
struct ble_gap_connection_values {
	uint16_t interval;		/**< Connection interval (unit 1.25 ms) */
	uint16_t latency;		/**< Connection latency (unit interval) */
	uint16_t supervision_to;	/**< Connection supervision timeout (unit 10ms)*/
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
 * Initial GAP configuration
 */
struct ble_wr_config_params {
	ble_addr_t bda;
	uint8_t bda_present;
	uint8_t device_name[BLE_DEVICE_NAME_LEN];	/**< GAP Device name, NULL terminated! */
	uint8_t name_present;
	uint16_t appearance;	/**< see BLE spec */
	int8_t tx_power;
	struct ble_gap_connection_params peripheral_conn_params;	/**< Peripheral preferred */
	struct ble_gap_connection_params central_conn_params;		/**< Central supported range */
	struct ble_gap_sm_config_params sm_config;			/**< Security manager configuration */
};

/** Read BD address response. */
typedef struct {
	struct cfw_message header;	/**< Component framework message header (@ref cfw),
	                                     MUST be first element of structure */
	ble_status_t status;		/**< Response status @ref BLE_STATUS */
	ble_addr_t bd;			/**< if status ok @ref ble_addr_t */
} ble_bda_rd_rsp_t;

struct ble_gap_adv_rsp_data {
	uint8_t *p_data;		/**< max 31 bytes! */
	uint8_t len;
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

typedef struct {
	uint8_t irk[BLE_GAP_SEC_MAX_KEY_LEN];	/**< Identity Resolving Key (IRK) */
} ble_gap_irk_info_t;

struct ble_gap_whitelist_info {
	ble_addr_t *p_bd;		/**< Array of bd addresses */
	ble_gap_irk_info_t *p_key;	/**< Array of irk keys (for address resolution offload) */
	uint8_t bd_count;		/**< number of bd addresses */
	uint8_t key_count;		/**< number of keys */
};

/**
 * Advertisement options.
 */
enum BLE_GAP_ADV_OPTIONS {
	BLE_GAP_OPT_ADV_DEFAULT = 0,		/**< no specific option */
	BLE_GAP_OPT_ADV_WHITE_LISTED = 0x02	/**< use white list and only report whitelisted devices */
};

/**
 * Advertisement parameters.
 */
struct ble_gap_adv_params {
	uint16_t timeout;
	uint16_t interval_min;	/**< min interval 0xffff: use default 0x0800 */
	uint16_t interval_max;	/**< max interval 0xffff: use default 0x0800 */
	uint8_t type;		/**< advertisement types @ref GAP_ADV_TYPES */
	uint8_t filter_policy;	/**< filter policy to apply with white list */
	ble_addr_t p_peer_bda;	/**< bd address of peer device in case of directed advertisement */
	uint8_t options;	/**< options see @ref BLE_GAP_ADV_OPTIONS (to be ORed) */
};

/**
 * Generic BLE Status. Response
 * Short status response for commands not returning any additional data
 */
typedef struct {
	struct cfw_message header;	/**< Component framework message header (@ref cfw),
					     MUST be first element of structure */
	ble_status_t status;		/**< Response status @ref BLE_STATUS */
	uint32_t wl_handle;		/**< reference handle. to be used for clearing it later */
} ble_gap_wr_white_list_rsp_t;


/**
 * Appearance read response message.
 */
typedef struct {
	struct cfw_message header;	/**< Component framework message header (@ref cfw),
					     MUST be first element of structure */
	ble_status_t status;		/**< Response status @ref BLE_STATUS */
	uint16_t uuid;			/**< value of GAP appearance characteristic */
} ble_rd_appearance_rsp_t;

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

struct ble_gap_svc_local_name {
	uint8_t sec_mode;	/**< security mode for writing device name, @ref BLE_GAP_SEC_MODES */
	uint8_t authorization;	/**< 0: no authorization, 1: authorization required */
	uint8_t len;		/**< device name length (0-248) */
	uint8_t name_array[BLE_DEVICE_NAME_LEN];	/**< name to to write */
};

enum BLE_GAP_SVC_ATTR_TYPE {
	GAP_SVC_ATTR_NAME = 0,		/**< Device Name, UUID 0x2a00 */
	GAP_SVC_ATTR_APPEARANCE,	/**< Appearance, UUID 0x2a01 */
	GAP_SVC_ATTR_PPCP = 4,		/**< Peripheral Preferred Connection Parameters (PPCP), UUID 0x2a04 */
	GAP_SVC_ATTR_CAR = 0xa6,	/**< Central Address Resolution (CAR), UUID 0x2aa6, BT 4.2 */
};

struct ble_gap_service_write_params {
	uint16_t attr_type;		/**< GAP Characteristics attribute type  @ref BLE_GAP_SVC_ATTR_TYPE */
	union {
		struct ble_gap_svc_local_name name;
		uint16_t appearance;				/**< Appearance UUID */
		struct ble_gap_connection_params conn_params;	/**< Preferred Peripheral Connection Parameters */
		uint8_t car;					/**< Central Address Resolution support 0: no, 1: yes */
	};
};

struct ble_gap_service_read_rsp {
	struct cfw_message header;	/**< Component framework message header (@ref cfw),
					     MUST be first element of structure */
	ble_status_t status;		/**< status of read operation @ref BLE_STATUS,
					     in case failure union shall be empty */
	uint16_t attr_type;		/**< type of attribute returned (valid even in failure case! */
	union {
		struct ble_gap_svc_local_name name;
		uint16_t appearance;				/**< Appearance UUID */
		struct ble_gap_connection_params conn_params;	/**< Preferred Peripheral Connection Parameters */
		uint8_t car;					/** Central Address Resolution support 0: no, 1: yes */
	};
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
	BLE_GAP_SM_KEY_TYPE_NONE = 0,	/**< No key (may be used to reject). */
	BLE_GAP_SM_PASSKEY,		/**< Security data is a 6-digit passkey. */
	BLE_GAP_SM_OOB,			/**< Security data is 16 bytes of OOB data */
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

enum BLE_GAP_SCAN_OPTIONS {
	BLE_GAP_SCAN_DEFAULT = 0,		/**< no specific option */
	BLE_GAP_SCAN_ACTIVE = 0x01,		/**< do an active scan (request scan response */
	BLE_GAP_SCAN_WHITE_LISTED = 0x02	/**< Use white list and only report whitelisted devices */
};

enum BLE_GAP_SET_OPTIONS {
	BLE_GAP_SET_CH_MAP = 0,		/**< Set channel map */
};

typedef struct {
	uint16_t conn_handle;		/**< connection on which to change channel map */
	uint8_t map[5];			/**< 37 bits are used of the 40 bits (LSB) */
} ble_gap_channel_map_t;

/**
 * GAP option data structure.
 */
typedef union {
	ble_gap_channel_map_t ch_map;	/**< BLE channel map to set see BT spec */
} ble_gap_option_t;

/**
 * Scan parameters.
 *
 * @note Check BT core spec for high low duty cycle interval & window size!
 */
struct ble_gap_scan_params {
	uint16_t timeout;		/**< scan timeout in s, 0 never */
	uint16_t interval;		/**< interval: 0x4 - 0x4000. (unit: 0.625ms), use default: 0xffff (0x0010) */
	uint16_t window;		/**< Window: 0x4 - 0x4000. (unit: 0.625ms), use default 0xffff (= 0x0010) */
	uint8_t options;		/**< scan options, ORed options from @ref BLE_GAP_SCAN_OPTIONS */
};

/**
 * Connect event.
 */
struct ble_gap_connect_evt {
	uint16_t conn_handle;				/**< Connection handle */
	struct ble_gap_connection_values conn_values;	/**< Connection values */
	uint8_t role;					/**< role in this connection */
	ble_addr_t peer_bda;				/**< address of peer device */
};

/**
 * Disconnect event.
 */
struct ble_gap_disconnect_evt {
	uint16_t conn_handle;	/**< Connection handle */
	uint8_t hci_reason;	/**< HCI disconnect reason */
};

/**
 * Updated connection event.
 */
struct ble_gap_conn_update_evt {
	uint16_t conn_handle;				/**< Connection handle */
	struct ble_gap_connection_values conn_values;	/**< New connection values */
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

/**
 * Security manager pairing status event.
 */
struct ble_gap_sm_status_evt {
	uint16_t conn_handle;	/**< Connection handle */
	uint8_t status;		/**< Security manager status @ref BLE_GAP_SM_STATUS */
	uint8_t gap_status;	/**< Result of SM procedure, non-null indicates failure */
};

/**
 * Security manager passkey display event.
 */
struct ble_gap_sm_passkey_disp_evt {
	uint16_t conn_handle;			/**< Connection handle */
	uint8_t passkey[BLE_PASSKEY_LEN];	/**< Passkey to be displayed */
};

/**
 * Security manager passkey request event.
 */
struct ble_gap_sm_passkey_req_evt {
	uint16_t conn_handle;	/**< Connection handle */
	uint8_t key_type;	/**< Passkey or OBB data see @ref BLE_GAP_SM_PASSKEY_TYPE */
};

/**
 * GAP/SMP security result status code.
 * see Vol 3: Part H, chapter 3.5.5.
 */
enum BLE_GAP_SEC_RESULT_STATUS {
	BLE_GAP_SEC_STATUS_SUCCESS = 0,		/**< bonding/pairing completed successfully */
	BLE_GAP_SEC_STATUS_PASSKEY_ENTRY_FAILED,/**< passkey entry failed */
	BLE_GAP_SEC_STATUS_OOB_NOT_AVAILABLE,	/**< Out of Band data is not available */
	BLE_GAP_SEC_STATUS_AUTH_REQUIREMENTS,	/**< Authentication requirements not met due to IO cap */
	BLE_GAP_SEC_STATUS_CONFIRM_VALUE,	/**< Confirm value does not match calculated value */
	BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPPORTED, /**< Pairing not supported by the device  */
	BLE_GAP_SEC_STATUS_ENC_KEY_SIZE,	/**< Encryption key size insufficient */
	BLE_GAP_SEC_STATUS_SMP_CMD_UNSUPPORTED,	/**< Unsupported SMP command on this device */
	BLE_GAP_SEC_STATUS_UNSPECIFIED,		/**< Failure due to unspecified reason */
	BLE_GAP_SEC_STATUS_REPEATED_ATTEMPTS,	/**< Pairing/authent disallowed due to too little time elapsed since last attempt */
	BLE_GAP_SEC_STATUS_INVALID_PARAMS,	/**< Invalid parameters due to length or parameters */
	/* 4.2 spec only ? */
	BLE_GAP_SEC_STATUS_DHKEY_CHECK_FAILED,	/**< Remote device indicates that DHKey does not match local calculated key */
	BLE_GAP_SEC_STATUS_NUMERIC_COMP_FAILED,	/**< values in numeric key comparison protocol do not match */
	BLE_GAP_SEC_STATUS_BREDR_PAIRING_INPROGRESS,/**< Failure due to BR/EDR pairing request */
	BLE_GAP_SEC_STATUS_CROSS_TSPRT_KEY_GEN_DIS,
						/**< BR/EDR link key generation can not be use for LE keys handling */
};

enum BLE_SVC_GAP_TIMEOUT_REASON {
	BLE_SVC_GAP_TO_ADV,	/**< Advertisement Stopped. */
	BLE_SVC_GAP_TO_SEC_REQ,	/**< Security Request took too long. */
	BLE_SVC_GAP_TO_SCAN,	/**< Scanning stopped. */
	BLE_SVC_GAP_TO_CONN,	/**< Connection Link timeout. */
};

/**
 * GAP timeout event (e.g. protocol error).
 */
struct ble_gap_timout_evt {
	uint16_t conn_handle;	/**< Connection handle */
	int reason;		/**< reason for timeout @ref BLE_SVC_GAP_TIMEOUT_REASON */
};

/**
 * Advertisement data structure (central role).
 */
struct ble_gap_adv_data_evt {
	ble_addr_t remote_bda;	/**< address of remote device */
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
 * RSSI signal strength event.
 */
struct ble_gap_rssi_evt {
	uint16_t conn_handle;	/**< Connection handle */
	int8_t rssi_lvl;	/**< RSSI level (compared to 0 dBm) */
};

/**
 * RSSI report parameters
 */
struct rssi_report_params {
	uint16_t conn_hdl;	/**< Connection handle */
	uint8_t op;	/**< RSSI operation @ref BLE_GAP_RSSI_OPS */
	uint8_t delta_dBm;	/**< minimum RSSI dBm change to report a new RSSI value */
	uint8_t min_count;	/**< number of delta_dBm changes before sending a new RSSI report */
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

/** Generic request op codes.
 * This allows to access some non connection related commands like DTM.
 */
enum BLE_GAP_GEN_OPS {
	DUMMY_VALUE = 0,		/**< Not used now. */
};

/**
 * Generic command parameters.
 *
 * @note Independent of connection!
 */
struct ble_gap_gen_cmd_params {
	uint8_t op_code;	/**< @ref BLE_GAP_GEN_OPS */
};

struct ble_version_response {
	struct version_header version;
	struct ble_version_rsp *p_rsp; /**< Pointer to response data structure */
};

struct ble_gap_sm_config_rsp {
	struct ble_enable_rsp *p_rsp; /**< Pointer to response data structure */
	ble_status_t status; /**< Result of SM init */
	uint8_t enable;            /**< enable state: 0:Disabled, 1:Enabled */
	ble_addr_t bd_addr;
	uint32_t state; /**< State of bond DB */
};

struct ble_gap_disconnect_req_params {
	uint16_t conn_handle;   /**< Connection handle */
	uint8_t reason;         /**< Reason of the disconnect */
};

struct ble_gap_sm_pairing_req_params {
	uint16_t conn_handle;                    /**< Connection on which bonding procedure is executed */
	struct ble_gap_sm_pairing_params params; /**< Local authentication/bonding parameters */
};

struct ble_gap_sm_key_reply_req_params {
	uint16_t conn_handle;                 /**< Connection on which bonding is going on */
	struct ble_gap_sm_passkey params;     /**< Bonding security reply */
};

struct ble_gap_connect_update_params {
	uint16_t conn_handle;
	struct ble_gap_connection_params params;
};

struct ble_gap_cancel_connect_params {
	const ble_addr_t  bd;                 /**< Address of device for which the connection shall be canceled */
};

struct ble_gap_set_option_params {
	uint8_t op;                   /**< Option to set @ref BLE_GAP_SET_OPTIONS */
	const ble_gap_option_t p_opt; /**< Address of device for which the connection shall be canceled ble_gap_option_t */
};

struct ble_gap_service_read_params {
	uint16_t type;                /**< Type of GAP data characteristic to read @ref BLE_GAP_SVC_ATTR_TYPE */
};

struct ble_response {
	ble_status_t status;        /**< Status of the operation */
	void * p_priv;
};

struct ble_service_read_bda_response {
	ble_status_t status;            /**< Status of the operation */
	ble_addr_t bd;                  /**< If @ref status ok @ref ble_addr_t */
	void * p_priv;
};

struct ble_service_write_response {
	ble_status_t status;            /**< Status of the operation */
	uint16_t attr_type;             /**< GAP Characteristics attribute type  @ref BLE_GAP_SVC_ATTR_TYPE */
	void * p_priv;                  /**< Pointer to the priv data of the request */
};

struct debug_params {
	uint32_t u0;  /** user parameter */
	uint32_t u1;  /** user parameter */
};

struct debug_response {
	ble_status_t status;   /**< Status of the operation */
	uint32_t u0;           /** user parameter */
	uint32_t u1;           /** user parameter */
	void * p_priv;         /**< Pointer to the priv data of the request */
};

struct ble_dtm_result_msg {
	struct cfw_message header;              /**< Component framework message header (@ref cfw),
	                                             MUST be first element of structure */
	ble_status_t status;                    /**< Status of the operation */
	struct ble_dtm_test_result result;	/**< Result data of DTM RX test */
};

/**
 * Generic request message response or event.
 */
struct ble_generic_msg {
	struct cfw_message header;	/**< Component framework message header (@ref cfw),
					     MUST be first element of structure */
	ble_status_t status;            /**< Status of the operation */
	uint8_t op_code;		/**< Opcode to which this message is applicable @ref BLE_GAP_GEN_OPS */
};

/**
 * Debug request or response message
 */
struct ble_gap_dbg_msg {
	struct cfw_message header;
	uint32_t u0;
	uint32_t u1;
};

/**
 * Function invoked by the BLE service when a new connection is established.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_connect_evt(const struct ble_gap_connect_evt * p_evt);

/**
 * Function invoked by the BLE service when a connection is lost.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_disconnect_evt(const struct ble_gap_disconnect_evt * p_evt);

/**
 * Function invoked by the BLE service when a connection is updated.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_conn_update_evt(const struct ble_gap_conn_update_evt * p_evt);

/**
 * Function invoked by the BLE service upon a security manager event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_sm_status_evt(const struct ble_gap_sm_status_evt * p_evt);

/**
 * Function invoked by the BLE service upon security manager display event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_sm_passkey_display_evt(const struct ble_gap_sm_passkey_disp_evt * p_evt);

/**
 * Function invoked by the BLE service upon security manager passkey
 * request event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_sm_passkey_req_evt(const struct ble_gap_sm_passkey_req_evt * p_evt);

/**
 * Function invoked by the BLE service upon timeout event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_to_evt(const struct ble_gap_timout_evt * p_evt);

/**
 * Function invoked by the BLE service upon RSSI event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_rssi_evt(const struct ble_gap_rssi_evt * p_evt);

/**
 * Set Enable configuration parameters (BD address, etc).
 *
 * The response to this request is received through @ref on_ble_gap_set_enable_config_rsp
 *
 * This shall put the controller stack into a usable (enabled) state.
 * Hence this should be called first!
 *
 * @param p_config     BLE write configuration
 * @param p_req        Pointer to private data
 *
 */
void ble_gap_set_enable_config_req(const struct ble_wr_config_params * p_config,
		struct ble_enable_req_msg *p_req);

/**
 * Response to @ref ble_gap_set_enable_config_req.
 *
 * @param p_params Response
 */
void on_ble_gap_set_enable_config_rsp(const struct ble_response * p_params);

/**
 * Read BD address from Controller.
 *
 * The response to this request is received through @ref on_ble_gap_read_bda_rsp
 *
 * @param p_priv Pointer to private data
 */
void ble_gap_read_bda_req(void * p_priv);

/**
 * Response to @ref ble_gap_read_bda_req.
 *
 * @param p_params Response
 */
void on_ble_gap_read_bda_rsp(const struct ble_service_read_bda_response * p_params);

/**
 * Write Advertisement data to BLE controller.
 *
 * The response to this request is received through @ref on_ble_gap_wr_adv_data_rsp
 *
 * Store advertisement data in BLE controller. It needs to be done BEFORE starting advertisement
 *
 * @param p_data   adv data to store in BLE controller
 * @param dlen  scan response data to store in controller, can be NULL
 * @param p_sr_data       pointer to private data
 * @param srdlen       pointer to private data
 * @param p_req pointer to private data
 *
 */
void ble_gap_wr_adv_data_req(uint8_t * p_data, uint8_t dlen, uint8_t * p_sr_data, uint8_t srdlen,
		struct ble_start_advertisement_req_msg *p_req);

/**
 * Response to @ref ble_gap_wr_adv_data_req.
 *
 * @param p_params Response
 */
void on_ble_gap_wr_adv_data_rsp(const struct ble_response * p_params);

/**
 * Write white list to the BLE controller.
 *
 * The response to this request is received through @ref on_ble_gap_wr_white_list_rsp
 *
 * Store white in BLE controller. It needs to be done BEFORE starting advertisement or
 * start scanning
 *
 * @param bd_array      array of bd addresses
 * @param bd_array_size size of bd addresses array
 * @param irk_array     array of irk keys (for address resolution offload)
 * @param irk_array_size size of irk keys array
 * @param p_priv        pointer to private data
 */
void ble_gap_wr_white_list_req(ble_addr_t * bd_array, uint8_t bd_array_size,
		ble_gap_irk_info_t * irk_array, uint8_t irk_array_size, void * p_priv);

/**
 * Response to @ref ble_gap_wr_white_list_req.
 *
 * @param p_params Response
 */
void on_ble_gap_wr_white_list_rsp(const struct ble_response * p_params);
/**
 * Clear previously stored white list.
 *
 * The response to this request is received through @ref on_ble_gap_clr_white_list_rsp
 *
 * @param p_priv Pointer to private data
 */
void ble_gap_clr_white_list_req(void * p_priv);

/**
 * Response to @ref ble_gap_clr_white_list_req.
 *
 * @param p_params Response
 */
void on_ble_gap_clr_white_list_rsp(const struct ble_response * p_params);

/**
 * Start advertising.
 *
 * The response to this request is received through @ref on_ble_gap_start_advertise_rsp
 *
 * @param p_params advertisement
 * @param p_req Pointer to private data
 */
void ble_gap_start_advertise_req(const struct ble_gap_adv_params * p_params,
		struct ble_start_advertisement_req_msg *p_req);

/**
 * Response to @ref ble_gap_start_advertise_req.
 *
 * @param p_params Response
 */
void on_ble_gap_start_advertise_rsp(const struct ble_response * p_params);

/**
 * Stop advertising.
 *
 * The response to this request is received through @ref on_ble_gap_stop_advertise_rsp
 *
 * @param p_priv Pointer to private data
 */
void ble_gap_stop_advertise_req(void * p_priv);

/**
 * Response to @ref ble_gap_stop_advertise_req.
 *
 * @param p_params Response
 */
void on_ble_gap_stop_advertise_rsp(const struct ble_response * p_params);

/**
 * Update connection.
 *
 * The response to this request is received through @ref on_ble_gap_conn_update_rsp
 *
 * This function's behavior depends on the role of the connection:
 * - in peripheral mode, it sends an L2CAP signaling connection parameter
 *   update request based the values in <i>p_conn_param</i> argument,
 *   and the action can be taken by the central at link layer
 * - in central mode, it will send a link layer command to change the
 *   connection values based on the values in <i>p_conn_param</i> argument where the
 *   connection interval is interval_min.
 *
 * When the connection is updated, function event on_ble_gap_conn_update_evt is called.
 *
 * @param p_params Connection parameters
 * @param p_rsp pointer to private data
 */
void ble_gap_conn_update_req(const struct ble_gap_connect_update_params * p_params,
		struct ble_conn_update_rsp *p_rsp);

/**
 * Response to @ref ble_gap_conn_update_req.
 *
 * @param p_params Response
 */
void on_ble_gap_conn_update_rsp(const struct ble_response * p_params);

/**
 * Disconnect connection (peripheral or central role).
 *
 * The response to this request is received through @ref on_ble_gap_disconnect_rsp
 *
 * @param p_params Connection to terminate
 * @param p_rsp Pointer to private data
 */
void ble_gap_disconnect_req(const struct ble_gap_disconnect_req_params * p_params,
		struct ble_disconnect_rsp *p_rsp);
/**
 * Response to @ref ble_gap_disconnect_req.
 *
 * @param p_params Response
 */
void on_ble_gap_disconnect_rsp(const struct ble_response * p_params);

/**
 * Write GAP Service Attribute Characteristics.
 *
 * The response to this request is received through @ref on_ble_gap_service_write_rsp
 *
 * @param p_params data of the characteristic to write
 * @param p_priv Pointer to private data
 */
void ble_gap_service_write_req(const struct ble_gap_service_write_params * p_params,
		struct cfw_message *p_priv);

/**
 * Response to @ref ble_gap_service_write_req.
 *
 * @param p_params Response
 */
void on_ble_gap_service_write_rsp(const struct ble_service_write_response * p_params);

/**
 * Read GAP Service Characteristics.
 *
 * The response to this request is received through @ref on_ble_gap_service_read_rsp
 *
* @param ble_gap_service_read GAP service characteristic to read
* @param p_priv Pointer to private data
 */
void ble_gap_service_read_req(const struct ble_gap_service_read_params * ble_gap_service_read, void * p_priv);

/**
 * Response to @ref ble_gap_service_read_req.
 *
 * @param p_params Response
 */
void on_ble_gap_service_read_rsp(const struct ble_response * p_params);

/**
 * Configuring the security manager.
 *
 * The response to this request is received through @ref on_ble_gap_sm_config_rsp
 *
 * @param p_params local authentication/bonding parameters
 * @param p_rsp Pointer to private data
 */
void ble_gap_sm_config_req(const struct ble_gap_sm_config_params * p_params,
		struct ble_enable_rsp *p_rsp);

/**
 * Response to @ref ble_gap_sm_config_req.
 *
 * @param p_params Response
 */
void on_ble_gap_sm_config_rsp(struct ble_gap_sm_config_rsp * p_params);

/**
 * Initiate the bonding procedure (central).
 *
 * The response to this request is received through @ref on_ble_gap_sm_pairing_rsp
 *
 * @param p_params Connection to initiate with its parameters
 * @param p_priv Pointer to private data
 */
void ble_gap_sm_pairing_req(const struct ble_gap_sm_pairing_req_params * p_params, void * p_priv);

/**
 * Response to @ref ble_gap_sm_pairing_req.
 *
 * @param p_params Response
 */
void on_ble_gap_sm_pairing_rsp(const struct ble_response * p_params);

/**
 * Reply to an incoming passkey request event.
 *
 * The response to this request is received through @ref on_ble_gap_sm_passkey_reply_rsp
 *
 * @param p_params connection on which bonding is going on and  bonding security reply
 * @param p_req Pointer to private data
 */
void ble_gap_sm_passkey_reply_req(const struct ble_gap_sm_key_reply_req_params * p_params,
		struct ble_gap_sm_key_reply_req_msg *p_req);

/**
 * Response to @ref ble_gap_sm_passkey_reply_req.
 *
 * @param p_params Response
 */
void on_ble_gap_sm_passkey_reply_rsp(const struct ble_response * p_params);

/**
 * Clear bonds
 *
 * The response to this request is received through @ref on_ble_gap_sm_clear_bonds_rsp
 *
 * @param p_rsp Pointer to response data structure
 */
void ble_gap_sm_clear_bonds_req(struct ble_rsp *p_rsp);

/**
 * Response to @ref ble_gap_sm_clear_bonds_req.
 *
 * @param p_params Response
 */
void on_ble_gap_sm_clear_bonds_rsp(const struct ble_response * p_params);

/**
 * Enable or disable the reporting of the RSSI value.
 *
 * The response to this request is received through @ref on_ble_gap_set_rssi_report_rsp
 *
 * @param params RSSI report parameters
 * @param p_priv  pointer to private data
 */
void ble_gap_set_rssi_report_req(const struct rssi_report_params * params,
			    void * p_priv);

/**
 * Response to @ref ble_gap_set_rssi_report_req.
 *
 * @param p_params Response
 */
void on_ble_gap_set_rssi_report_rsp(const struct ble_response * p_params);

/**
 * Start scanning for BLE devices doing advertisement.
 *
 * The response to this request is received through @ref on_ble_gap_start_scan_rsp
 *
 * @param p_params scan parameters
 * @param p_priv Pointer to private data
 */
void ble_gap_start_scan_req(const struct ble_gap_scan_params * p_params, void * p_priv);

/**
 * Response to @ref ble_gap_start_scan_req.
 *
 * @param p_params Response
 */
void on_ble_gap_start_scan_rsp(const struct ble_response * p_params);

/**
 * Stop scanning.
 *
 * The response to this request is received through @ref on_ble_gap_stop_scan_rsp
 *
 * @param p_priv Pointer to private data
 */
void ble_gap_stop_scan_req(void * p_priv);

/**
 * Response to @ref ble_gap_stop_scan_req.
 *
 * @param p_params Response
 */
void on_ble_gap_stop_scan_rsp(const struct ble_response * p_params);

/**
 * Connect to a Remote Device.
 *
 * The response to this request is received through @ref on_ble_gap_connect_rsp
 *
 * @param p_params connection parameters @ref ble_gap_connection_params
 * @param p_priv Pointer to private data
 */
void ble_gap_connect_req(const struct ble_gap_connection_params * p_params, void * p_priv);

/**
 * Response to @ref ble_gap_connect_req.
 *
 * @param p_params Response
 */
void on_ble_gap_connect_rsp(const struct ble_response * p_params);

/**
 * Cancel an ongoing connection attempt.
 *
 * The response to this request is received through @ref on_ble_gap_cancel_connect_rsp
 *
 * @param p_params contains address of device for which the connection shall be canceled
 * @param p_priv Pointer to private data
 */
void ble_gap_cancel_connect_req(const struct ble_gap_cancel_connect_params * p_params, void * p_priv);

/**
 * Response to @ref ble_gap_cancel_connect_req.
 *
 * @param p_params Response
 */
void on_ble_gap_cancel_connect_rsp(const struct ble_response * p_params);

/**
 * Set a gap option (channel map etc) on a connection.
 *
 * The response to this request is received through @ref on_ble_gap_set_option_rsp
 *
 * @param p_params contains gap options parameters @ref ble_gap_set_option_params
 * @param p_priv Pointer to private data
 */
void ble_gap_set_option_req(const struct ble_gap_set_option_params * p_params, void * p_priv);

/**
 * Response to @ref ble_gap_set_option_req.
 *
 * @param p_params Response
 */
void on_ble_gap_set_option_rsp(const struct ble_response * p_params);

/**
 * Generic command
 *
 * The response to this request is received through @ref on_ble_gap_generic_cmd_rsp
 *
 * @param p_params contains Generic command parameters.
 * @param p_priv Pointer to private data
 */
void ble_gap_generic_cmd_req(const struct ble_gap_gen_cmd_params * p_params, void * p_priv);

/**
 * Response to @ref ble_gap_generic_cmd_req.
 *
 * @param p_params Response
 */
void on_ble_gap_generic_cmd_rsp(const struct ble_response * p_params);

/**
 * Get ble_core version.
 *
 * The response to this request is received through @ref on_ble_gap_get_version_rsp
 *
 * @param rsp Pointer to response data structure
 */
void ble_gap_get_version_req(struct ble_version_rsp *rsp);

/**
 * Response to @ref ble_gap_get_version_req.
 *
 * @param p_params Response
 */
void on_ble_gap_get_version_rsp(const struct ble_version_response * p_params);

/**
 * Init DTM mode.
 *
 * The response to this request is received through @ref on_ble_gap_dtm_init_rsp
 *
 * @param p_rsp Pointer to response data structure
 */
void ble_gap_dtm_init_req(struct ble_enable_rsp *p_rsp);

/**
 * Response to @ref ble_gap_dtm_init_req.
 *
 * @param p_rsp Pointer to response data structure
 */
void on_ble_gap_dtm_init_rsp(struct ble_enable_rsp *p_rsp);

/**
 * Send generic debug command
 *
 * The response to this request is received through @ref on_ble_gap_dbg_rsp
 *
 * @param p_params debug parameters
 * @param p_rsp private data
 */
void ble_gap_dbg_req(const struct debug_params * p_params, struct ble_dbg_msg *p_rsp);

/**
 * Response to @ref ble_gap_dbg_req.
 *
 * @param p_params Response
 */
void on_ble_gap_dbg_rsp(const struct debug_response * p_params);


/** @} */

#endif /* __BLE_SVC_API_H__ */
