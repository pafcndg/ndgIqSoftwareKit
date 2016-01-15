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

#ifndef GAP_INTERNAL_H_
#define GAP_INTERNAL_H_

#include <stdint.h>
/* For bt_addr_le_t */
#include "zephyr/bluetooth/hci.h"
#include "infra/version.h"

/* Must be the same with ble_service_gap_api.h ! */
#define BLE_GAP_SEC_MAX_KEY_LEN 16		/**< Maximum security key len (LTK, CSRK) */
#define BLE_PASSKEY_LEN 6

/* Must be the same with BLE_GAP_SM_OPTIONS ! */
/**
 * GAP security manager options for bonding/authentication procedures, see Vol 3: Part H, 3.5.
 */
enum BLE_CORE_GAP_SM_OPTIONS {
	BLE_CORE_GAP_BONDING = 0x01,		/**< SMP supports bonding */
	BLE_CORE_GAP_MITM = 0x04,		/**< SMP requires Man In The Middle protection */
	BLE_CORE_GAP_OOB = 0x08		/**< SMP supports Out Of Band data */
};

/* Must be the same with BLE_CORE_GAP_SM_PASSKEY_TYPE ! */
/**
 * Security manager passkey type.
 */
enum BLE_CORE_GAP_SM_PASSKEY_TYPE {
	BLE_CORE_GAP_SM_PK_NONE = 0,   /**< No key (may be used to reject). */
	BLE_CORE_GAP_SM_PK_PASSKEY,    /**< Security data is a 6-digit passkey. */
	BLE_CORE_GAP_SM_PK_OOB,        /**< Security data is 16 bytes of OOB data */
};

/* Must be the same with BLE_GAP_SM_STATUS ! */
/**
 * GAP security manager status codes.
 */
enum BLE_CORE_GAP_SM_STATUS {
	BLE_CORE_GAP_SM_ST_START_PAIRING,	/*< Pairing has started */          //!< BLE_CORE_GAP_SM_ST_START_PAIRING
	BLE_CORE_GAP_SM_ST_BONDING_COMPLETE,	/*< Bonding has completed */     //!< BLE_CORE_GAP_SM_ST_BONDING_COMPLETE
	BLE_CORE_GAP_SM_ST_LINK_ENCRYPTED,	/*< Link is encrypted */           //!< BLE_CORE_GAP_SM_ST_LINK_ENCRYPTED
	BLE_CORE_GAP_SM_ST_SECURITY_UPDATE,	/*< Link keys have been updated *///!< BLE_CORE_GAP_SM_ST_SECURITY_UPDATE
};

/* Must be the same with BLE_GAP_RSSI_OPS ! */
/**
 * RSSI operation definition.
 */
enum BLE_CORE_GAP_RSSI_OPS {
	BLE_CORE_GAP_RSSI_DISABLE_REPORT = 0,
	BLE_CORE_GAP_RSSI_ENABLE_REPORT
};

/** Test Mode opcodes. The same with ble_service_gap_api.h */
enum BLE_CORE_TEST_OPCODE {
	BLE_CORE_TEST_INIT_DTM = 0x01,	/**< Put BLE controller in HCI UART DTM test mode */
	BLE_CORE_TEST_START_DTM_RX = 0x1d,	/**< LE receiver test HCI opcode */
	BLE_CORE_TEST_START_DTM_TX = 0x1e,	/**< LE transmitter test HCI opcode */
	BLE_CORE_TEST_END_DTM = 0x1f,	/**< End LE DTM TEST */
	/* vendor specific commands start at 0x80 */
	BLE_CORE_TEST_SET_TXPOWER = 0x80,	/**< Set Tx power. To be called before start of tx test */
	BLE_CORE_TEST_START_TX_CARRIER,	/**< Start Tx Carrier Test */
};

struct ble_core_response {
	int status;		/**< Status of the operation */
	void *user_data;
};

struct ble_gap_device_name {
	uint8_t sec_mode;	/**< Security mode for writing device name, @ref BLE_GAP_SEC_MODES */
	uint8_t authorization;	/**< 0: no authorization, 1: authorization required */
	uint8_t len;		/**< Device name length (0-248) */
	uint8_t name_array[20];	/**< Device */
};

struct ble_gap_connection_values {
	uint16_t interval;		/**< Connection interval (unit 1.25 ms) */
	uint16_t latency;		/**< Connection latency (unit interval) */
	uint16_t supervision_to;	/**< Connection supervision timeout (unit 10ms)*/
};


enum BLE_GAP_SVC_ATTR_TYPE {
	GAP_SVC_ATTR_NAME = 0,		/**< Device Name, UUID 0x2a00 */
	GAP_SVC_ATTR_APPEARANCE,	/**< Appearance, UUID 0x2a01 */
	GAP_SVC_ATTR_PPCP = 4,		/**< Peripheral Preferred Connection Parameters (PPCP), UUID 0x2a04 */
	GAP_SVC_ATTR_CAR = 0xa6,	/**< Central Address Resolution (CAR), UUID 0x2aa6, BT 4.2 */
};

/**
 * Connection requested parameters.
 */
struct ble_core_gap_connection_params {
	uint16_t interval_min;	/**< minimal connection interval: range 0x0006 to 0x0c80 (unit 1.25ms) */
	uint16_t interval_max;	/**< maximum connection interval: range 0x0006 to 0x0c80 must be bigger then min! */
	uint16_t slave_latency;	/**< maximum connection slave latency: 0x0000 to 0x01f3 */
	uint16_t link_sup_to;	/**< link supervision timeout: 0x000a to 0x0c80 (unit 10ms) */
};

/**
 * Connection scan requested parameters.
 */
struct ble_core_gap_scan_params {
	uint8_t     active;       /**< If 1, perform active scanning (scan requests). */
	uint8_t     selective;    /**< If 1, ignore unknown devices (non whitelisted). */
	uint16_t    interval;     /**< Scan interval between 0x0004 and 0x4000 in 0.625ms units (2.5ms to 10.24s). */
	uint16_t    window;       /**< Scan window between 0x0004 and 0x4000 in 0.625ms units (2.5ms to 10.24s). */
	uint16_t    timeout;      /**< Scan timeout between 0x0001 and 0xFFFF in seconds, 0x0000 disables timeout. */
};

struct ble_gap_service_write_params {
	uint16_t attr_type;		/**< GAP Characteristics attribute type  @ref BLE_GAP_SVC_ATTR_TYPE */
	union {
		struct ble_gap_device_name name;
		uint16_t appearance;				/**< Appearance UUID */
		struct ble_core_gap_connection_params conn_params;	/**< Preferred Peripheral Connection Parameters */
		uint8_t car;					/**< Central Address Resolution support 0: no, 1: yes */
	};
};

struct ble_service_read_bda_response {
	int status;            /**< Status of the operation */
	bt_addr_le_t bd;       /**< If @ref status ok */
	void * user_data;
};

struct ble_service_write_response {
	int status;            /**< Status of the operation */
	uint16_t attr_type;    /**< GAP Characteristics attribute type  @ref BLE_GAP_SVC_ATTR_TYPE */
	void * user_data;      /**< Pointer to the user data of the request */
};

struct ble_gap_service_read_params {
	uint16_t attr_type;                /**< Type of GAP data characteristic to read @ref BLE_GAP_SVC_ATTR_TYPE */
};

struct debug_params {
	uint32_t u0;  /** user parameter */
	uint32_t u1;  /** user parameter */
};

struct debug_response {
	int status;       /**< Status of the operation */
	uint32_t u0;      /** user parameter */
	uint32_t u1;      /** user parameter */
	void * user_data; /**< Pointer to the user data of the request */
};

struct ble_wr_config_params {
	bt_addr_le_t bda;
	uint8_t bda_present;
	int8_t tx_power;
	struct ble_core_gap_connection_params central_conn_params; /**< Central supported range */
};

/**
 * Advertisement parameters.
 */

/* Complete encoded eir data structure */
struct bt_eir_data {
	uint8_t len;
	uint8_t data[31];
};

struct ble_gap_adv_params {
	uint16_t timeout;
	uint16_t interval_min;	/**< min interval 0xffff: use default 0x0800 */
	uint16_t interval_max;	/**< max interval 0xffff: use default 0x0800 */
	uint8_t type;		/**< advertisement types @ref GAP_ADV_TYPES */
	uint8_t filter_policy;	/**< filter policy to apply with white list */
	bt_addr_le_t peer_bda;	/**< bd address of peer device in case of directed advertisement */
	struct bt_eir_data ad;	/**< Advertisement data, maybe 0 (length) */
	struct bt_eir_data sd;	/**< Scan response data, maybe 0 (length) */
};

struct ble_log_s {
	uint8_t param0;
	uint8_t param1;
	uint8_t param2;
	uint8_t param3;
};

void ble_log(const struct ble_log_s *p_param, char *p_buf, uint8_t buflen);

void ble_core_delete_conn_params_timer(void);

void on_nble_up(void);

/**
 * Write GAP Service Attribute Characteristics.
 *
 * The response to this request is received through @ref on_ble_gap_service_write_rsp
 *
 * @param p_params data of the characteristic to write
 * @param user_data User data
 */
void ble_gap_service_write_req(const struct ble_gap_service_write_params * p_params,
		void *user_data);

/**
 * Response to @ref ble_gap_read_bda_req.
 *
 * @param p_params Response
 */
void on_ble_gap_read_bda_rsp(const struct ble_service_read_bda_response * p_params);

/**
 * Response to @ref ble_gap_service_write_req.
 *
 * @param p_params Response
 */
void on_ble_gap_service_write_rsp(const struct ble_service_write_response * p_params);

/**
 * Send generic debug command
 *
 * The response to this request is received through @ref on_ble_gap_dbg_rsp
 *
 * @param p_params debug parameters
 * @param user_data User data
 */
void ble_gap_dbg_req(const struct debug_params * p_params, void *user_data);

/**
 * Response to @ref ble_gap_dbg_req.
 *
 * @param p_params Response
 */
void on_ble_gap_dbg_rsp(const struct debug_response * p_params);

/**
 * Set Enable configuration parameters (BD address, etc).
 *
 * The response to this request is received through @ref on_ble_set_enable_config_rsp
 *
 * This shall put the controller stack into a usable (enabled) state.
 * Hence this should be called first!
 *
 * @param p_config     BLE write configuration
 * @param user_data    User data
 *
 */
void ble_set_enable_config_req(const struct ble_wr_config_params * p_config,
		void *user_data);

/**
 * Start advertising.
 *
 * The response to this request is received through @ref on_ble_gap_start_advertise_rsp
 *
 * @param p_params advertisement
 * @param p_adv_data Pointer to advertisement and scan response data
 */
void ble_gap_start_advertise_req(struct ble_gap_adv_params * p_params);

/**
 * Response to @ref ble_gap_start_advertise_req.
 *
 * @param p_params Response
 */
void on_ble_gap_start_advertise_rsp(const struct ble_core_response * p_params);

/**
 * Request to stop advertisement
 *
 * @param user_data pointer to private data
 */
void ble_gap_stop_advertise_req(void *user_data);

/**
 * Response to @ref ble_gap_stop_advertise_req.
 *
 * @param p_params Response
 */
void on_ble_gap_stop_advertise_rsp(const struct ble_core_response * p_params);

/**
 * Read BD address from Controller.
 *
 * The response to this request is received through @ref on_ble_gap_read_bda_rsp
 *
 * @param p_priv Pointer to private data
 */
void ble_gap_read_bda_req(void * p_priv);

struct ble_gap_irk_info {
	uint8_t irk[BLE_GAP_SEC_MAX_KEY_LEN];	/**< Identity Resolving Key (IRK) */
};

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
void ble_gap_wr_white_list_req(bt_addr_le_t * bd_array, uint8_t bd_array_size,
		struct ble_gap_irk_info * irk_array, uint8_t irk_array_size, void * p_priv);

/**
 * Response to @ref ble_gap_wr_white_list_req.
 *
 * @param p_params Response
 */
void on_ble_gap_wr_white_list_rsp(const struct ble_core_response * p_params);
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
void on_ble_gap_clr_white_list_rsp(const struct ble_core_response * p_params);

struct ble_gap_connect_update_params {
	uint16_t conn_handle;
	struct ble_core_gap_connection_params params;
};

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
 * @param user_data User data
 */
void ble_gap_conn_update_req(const struct ble_gap_connect_update_params *p_params,
		void *user_data);

/**
 * Response to @ref ble_gap_conn_update_req.
 *
 * @param p_params Response
 */
void on_ble_gap_conn_update_rsp(const struct ble_core_response * p_params);

struct ble_gap_connect_req_params {
	bt_addr_le_t bda;                                  /**< BD address */
	struct ble_core_gap_connection_params conn_params; /**< Connection parameters */
	struct ble_core_gap_scan_params scan_params;       /**< Scan parameters */
};

struct ble_gap_disconnect_req_params {
	uint16_t conn_handle;   /**< Connection handle */
	uint8_t reason;         /**< Reason of the disconnect */
};

/**
 * Disconnect connection (peripheral or central role).
 *
 * The response to this request is received through @ref on_ble_gap_disconnect_rsp
 *
 * @param p_params Connection to terminate
 * @param user_data User data
 */
void ble_gap_disconnect_req(const struct ble_gap_disconnect_req_params *p_params,
		void *user_data);
/**
 * Response to @ref ble_gap_disconnect_req.
 *
 * @param p_params Response
 */
void on_ble_gap_disconnect_rsp(const struct ble_core_response *p_params);

/**
 * Read GAP Service Characteristics.
 *
 * The response to this request is received through @ref on_ble_gap_service_read_rsp
 *
* @param ble_gap_service_read GAP service characteristic to read
* @param user_data Pointer to private data
 */
void ble_gap_service_read_req(const struct ble_gap_service_read_params * ble_gap_service_read, void * user_data);

/**
 * Response to @ref ble_gap_service_read_req.
 *
 * @param p_params Response
 */
void on_ble_gap_service_read_rsp(const struct ble_core_response * p_params);

/**
 * Security manager configuration parameters.
 *
 * options and io_caps will define there will be a passkey request or not.
 * It is assumed that io_caps and options are compatible.
 */
struct ble_core_gap_sm_config_params {
	uint8_t options;	/**< Security options (@ref BLE_GAP_SM_OPTIONS) */
	uint8_t io_caps;	/**< I/O Capabilities to allow passkey exchange (@ref BLE_GAP_IO_CAPABILITIES) */
	uint8_t key_size;	/**< Maximum encryption key size (7-16) */
};

/**
 * Configuring the security manager.
 *
 * The response to this request is received through @ref on_ble_gap_sm_config_rsp
 *
 * @param p_params local authentication/bonding parameters
 * @param user_data User data
 */
void ble_gap_sm_config_req(const struct ble_core_gap_sm_config_params * p_params,
		void *user_data);

struct ble_gap_sm_config_rsp {
	void *user_data; /**< Pointer to user data structure */
	int status; /**< Result of security manager initialization */
	uint32_t state; /**< State of bond DB */
};

/**
 * Response to @ref ble_gap_sm_config_req.
 *
 * @param p_params Response
 */
void on_ble_gap_sm_config_rsp(struct ble_gap_sm_config_rsp *p_params);

/**
 * Security manager pairing parameters.
 */
struct ble_core_gap_sm_pairing_params {
	uint8_t auth_level;	/**< authentication level see @ref BLE_GAP_SM_OPTIONS */
};

struct ble_core_gap_sm_pairing_req_params {
	uint16_t conn_handle;                    /**< Connection on which bonding procedure is executed */
	struct ble_core_gap_sm_pairing_params params; /**< Local authentication/bonding parameters */
};

/**
 * Initiate the bonding procedure (central).
 *
 * The response to this request is received through @ref on_ble_gap_sm_pairing_rsp
 *
 * @param p_params Connection to initiate with its parameters
 * @param user_data Pointer to private data
 */
void ble_gap_sm_pairing_req(const struct ble_core_gap_sm_pairing_req_params *p_params, void *user_data);

/**
 * Response to @ref ble_gap_sm_pairing_req.
 *
 * @param p_params Response
 */
void on_ble_gap_sm_pairing_rsp(const struct ble_core_response * p_params);

/**
 * Security reply to incoming security request.
 */
struct ble_core_gap_sm_passkey {
	uint8_t type;			/**< Security data type in this reply @ref BLE_GAP_SM_PASSKEY_TYPE */
	union {
		uint8_t passkey[6];	/**< 6 digits (string) */
		uint8_t oob[16];	/**< 16 bytes of OOB security data */
	};
};

struct ble_gap_sm_key_reply_req_params {
	uint16_t conn_handle;                 /**< Connection on which bonding is going on */
	struct ble_core_gap_sm_passkey params;     /**< Bonding security reply */
};

/**
 * Reply to an incoming passkey request event.
 *
 * The response to this request is received through @ref on_ble_gap_sm_passkey_reply_rsp
 *
 * @param p_params connection on which bonding is going on and  bonding security reply
 * @param user_data User data
 */
void ble_gap_sm_passkey_reply_req(const struct ble_gap_sm_key_reply_req_params * p_params,
		void *user_data);

/**
 * Response to @ref ble_gap_sm_passkey_reply_req.
 *
 * @param p_params Response
 */
void on_ble_gap_sm_passkey_reply_rsp(const struct ble_core_response * p_params);

/**
 * Clear bonds
 *
 * The response to this request is received through @ref on_ble_gap_sm_clear_bonds_rsp
 *
 * @param user_data User data
 */
void ble_gap_sm_clear_bonds_req(void *user_data);

/**
 * Response to @ref ble_gap_sm_clear_bonds_req.
 *
 * @param p_params Response
 */
void on_ble_gap_sm_clear_bonds_rsp(const struct ble_core_response * p_params);

/**
 * RSSI report parameters
 */
struct ble_core_rssi_report_params {
	uint16_t conn_handle;   /**< Connection handle */
	uint8_t op;             /**< RSSI operation @ref BLE_GAP_RSSI_OPS */
	uint8_t delta_dBm;      /**< minimum RSSI dBm change to report a new RSSI value */
	uint8_t min_count;      /**< number of delta_dBm changes before sending a new RSSI report */
};

/**
 * Enable or disable the reporting of the RSSI value.
 *
 * The response to this request is received through @ref on_ble_gap_set_rssi_report_rsp
 *
 * @param params RSSI report parameters
 * @param user_data  pointer to user data
 */
void ble_gap_set_rssi_report_req(const struct ble_core_rssi_report_params * params,
			    void * user_data);

/**
 * Response to @ref ble_gap_set_rssi_report_req.
 *
 * @param p_params Response
 */
void on_ble_gap_set_rssi_report_rsp(const struct ble_core_response * p_params);

enum BLE_GAP_SCAN_OPTIONS {
	BLE_GAP_SCAN_DEFAULT = 0,		/**< no specific option */
	BLE_GAP_SCAN_ACTIVE = 0x01,		/**< do an active scan (request scan response */
	BLE_GAP_SCAN_WHITE_LISTED = 0x02	/**< Use white list and only report whitelisted devices */
};

struct ble_gap_scan_params {
	uint16_t timeout;		/**< scan timeout in s, 0 never */
	uint16_t interval;		/**< interval: 0x4 - 0x4000. (unit: 0.625ms), use default: 0xffff (0x0010) */
	uint16_t window;		/**< Window: 0x4 - 0x4000. (unit: 0.625ms), use default 0xffff (= 0x0010) */
	uint8_t options;		/**< scan options, ORed options from @ref BLE_GAP_SCAN_OPTIONS */
};

/**
 * Start scanning for BLE devices doing advertisement.
 *
 * The response to this request is received through @ref on_ble_gap_start_scan_rsp
 *
 * @param p_params scan parameters
 * @param user_data Pointer to user data
 */
void ble_gap_start_scan_req(const struct ble_gap_scan_params * p_params, void * p_priv);

/**
 * Response to @ref ble_gap_start_scan_req.
 *
 * @param p_params Response
 */
void on_ble_gap_start_scan_rsp(const struct ble_core_response * p_params);

/**
 * Stop scanning.
 *
 * The response to this request is received through @ref on_ble_gap_stop_scan_rsp
 *
 * @param user_data Pointer to user data
 */
void ble_gap_stop_scan_req(void * p_priv);

/**
 * Response to @ref ble_gap_stop_scan_req.
 *
 * @param p_params Response
 */
void on_ble_gap_stop_scan_rsp(const struct ble_core_response * p_params);

/**
 * Connect to a Remote Device.
 *
 * The response to this request is received through @ref on_ble_gap_connect_rsp
 *
 * @param p_params connection parameters @ref ble_gap_connect_req_params
 * @param p_priv Pointer to private data
 */
void ble_gap_connect_req(const struct ble_gap_connect_req_params * p_params, void * user_data);

/**
 * Response to @ref ble_gap_connect_req.
 *
 * @param p_params Response
 */
void on_ble_gap_connect_rsp(const struct ble_core_response * p_params);

/**
 * Cancel an ongoing connection attempt.
 *
 * The response to this request is received through @ref on_ble_gap_cancel_connect_rsp
 *
 * @param user_data Pointer to user data
 */
void ble_gap_cancel_connect_req(void * p_priv);

/**
 * Response to @ref ble_gap_cancel_connect_req.
 *
 * @param p_params Response
 */
void on_ble_gap_cancel_connect_rsp(const struct ble_core_response * p_params);

enum BLE_GAP_SET_OPTIONS {
	BLE_GAP_SET_CH_MAP = 0,		/**< Set channel map */
};

struct ble_gap_channel_map {
	uint16_t conn_handle;		/**< connection on which to change channel map */
	uint8_t map[5];			/**< 37 bits are used of the 40 bits (LSB) */
} ;


struct ble_gap_set_option_params {
	uint8_t op;                   /**< Option to set @ref BLE_GAP_SET_OPTIONS */
	union {
		struct ble_gap_channel_map ch_map;
	};
};

/**
 * Set a gap option (channel map etc) on a connection.
 *
 * The response to this request is received through @ref on_ble_gap_set_option_rsp
 *
 * @param p_params contains gap options parameters
 * @param user_data Pointer to user data
 */
void ble_gap_set_option_req(const struct ble_gap_set_option_params *p_params, void * user_data);

/**
 * Response to @ref ble_gap_set_option_req.
 *
 * @param p_params Response
 */
void on_ble_gap_set_option_rsp(const struct ble_core_response * p_params);

/** Generic request op codes.
 * This allows to access some non connection related commands like DTM.
 */
enum BLE_GAP_GEN_OPS {
	DUMMY_VALUE = 0,		/**< Not used now. */
};

/** Generic command parameters. */
struct ble_gap_gen_cmd_params {
	uint8_t op_code;	/**< @ref BLE_GAP_GEN_OPS */
};

/**
 * Generic command
 *
 * The response to this request is received through @ref on_ble_gap_generic_cmd_rsp
 *
 * @param p_params contains Generic command parameters.
 * @param user_data Pointer to user data
 */
void ble_gap_generic_cmd_req(const struct ble_gap_gen_cmd_params * p_params, void * p_priv);

/**
 * Response to @ref ble_gap_generic_cmd_req.
 *
 * @param p_params Response
 */
void on_ble_gap_generic_cmd_rsp(const struct ble_core_response *p_params);

/**
 * Get ble_core version.
 *
 * The response to this request is received through @ref on_ble_get_version_rsp
 *
 * @param rsp Pointer to response data structure
 */
void ble_get_version_req(void *user_data);

struct ble_version_response {
	struct version_header version;
	void *user_data; /**< Pointer to response data structure */
};

/**
 * Response to @ref ble_get_version_req.
 *
 * @param p_params Response
 */
void on_ble_get_version_rsp(const struct ble_version_response *p_params);

/**
 * Init DTM mode.
 *
 * The response to this request is received through @ref on_ble_gap_dtm_init_rsp
 *
 * @param user_data Pointer to response data structure
 */
void ble_gap_dtm_init_req(void *user_data);

/**
 * Response to @ref ble_gap_dtm_init_req.
 *
 * @param user_data Pointer to user data structure
 */
void on_ble_gap_dtm_init_rsp(void *user_data);

struct ble_gap_connect_evt {
	uint16_t conn_handle;				/**< Connection handle */
	struct ble_gap_connection_values conn_values;	/**< Connection values */
	uint8_t role_slave;				/**< 0 if connected as master, otherwise as slave */
	bt_addr_le_t peer_bda;				/**< Address of peer device */
};

/**
 * Function invoked by the BLE service when a new connection is established.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_connect_evt(const struct ble_gap_connect_evt * p_evt);

struct ble_gap_disconnect_evt {
	uint16_t conn_handle;	/**< Connection handle */
	uint8_t hci_reason;	/**< HCI disconnect reason */
};

/**
 * Function invoked by the BLE service when a connection is lost.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_disconnect_evt(const struct ble_gap_disconnect_evt * p_evt);


/**
 * Updated connection event.
 */
struct ble_gap_conn_update_evt {
	uint16_t conn_handle;				/**< Connection handle */
	struct ble_gap_connection_values conn_values;	/**< New connection values */
};

/**
 * Function invoked by the BLE service when a connection is updated.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_conn_update_evt(const struct ble_gap_conn_update_evt * p_evt);

struct ble_gap_rssi_evt {
	uint16_t conn_handle;	/**< Connection handle */
	int8_t rssi_lvl;	/**< RSSI level (compared to 0 dBm) */
};

/**
 * Function invoked by the BLE service upon RSSI event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_rssi_evt(const struct ble_gap_rssi_evt * p_evt);

struct ble_gap_timout_evt {
	uint16_t conn_handle;	/**< Connection handle */
	int reason;		/**< reason for timeout @ref BLE_SVC_GAP_TIMEOUT_REASON */
};

/**
 * Function invoked by the BLE service upon timeout event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_to_evt(const struct ble_gap_timout_evt * p_evt);

struct ble_gap_sm_passkey_req_evt {
	uint16_t conn_handle;	/**< Connection handle */
	uint8_t key_type;	/**< Passkey or OBB data see @ref BLE_GAP_SM_PASSKEY_TYPE */
};

/**
 * Function invoked by the BLE service upon security manager passkey
 * request event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_sm_passkey_req_evt(const struct ble_gap_sm_passkey_req_evt * p_evt);

struct ble_gap_sm_passkey_disp_evt {
	uint16_t conn_handle;			/**< Connection handle */
	uint8_t passkey[BLE_PASSKEY_LEN];	/**< Passkey to be displayed */
};

/**
 * Function invoked by the BLE service upon security manager display event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_sm_passkey_display_evt(const struct ble_gap_sm_passkey_disp_evt * p_evt);

struct ble_gap_sm_status_evt {
	uint16_t conn_handle;	/**< Connection handle */
	uint8_t status;		/**< Security manager status @ref BLE_GAP_SM_STATUS */
	uint8_t gap_status;	/**< Result of SM procedure, non-null indicates failure */
};

/**
 * Function invoked by the BLE service upon a security manager event.
 *
 * @param p_evt Pointer to the event structure.
 */
void on_ble_gap_sm_status_evt(const struct ble_gap_sm_status_evt * p_evt);

/**
 * Response to @ref ble_set_enable_config_req.
 *
 * @param p_params Response
 */
void on_ble_set_enable_config_rsp(const struct ble_core_response *p_params);

/**
 * Get the list of bonded devices
 *
 * @param user_data User Data
 */
void ble_get_bonded_device_list_req(void *user_data);

/**@brief Structure containing list of bonded devices. */
struct ble_core_bonded_devices
{
#ifdef CONFIG_TCMD_BLE_DEBUG
	bt_addr_le_t    addrs[8];      /**< Pointer to an array of device address pointers, pointing to addresses to be used in whitelist. NULL if none are given. */
#endif
	uint8_t       addr_count;      /**< Count of device addresses in array. */
};

struct ble_get_bonded_device_list_rsp {
	int status; /**< Status of the operation */
	struct ble_core_bonded_devices bonded_dev;
	struct cfw_message *p_priv;
};

void on_ble_get_bonded_device_list_rsp(const struct ble_get_bonded_device_list_rsp* p_params);

#endif /* GAP_INTERNAL_H_ */
