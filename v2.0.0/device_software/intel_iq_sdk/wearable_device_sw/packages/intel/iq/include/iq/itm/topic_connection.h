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
 */

#ifndef TOPIC_CONNECTION_H_
#define TOPIC_CONNECTION_H_

/**
 * @defgroup topic_connection_grp Topic connection
 * @{
 * @ingroup itm
 */

#include <stdint.h>

// Forward declarations
struct on_tx_complete_params;
struct topic_application;

/**
 * Interface to a topic manager connection.
 *
 * The interface contains the API that is invoked by the topic manager when
 * an action shall be performed on this connection.
 *
 * Likewise, when a topic connection implementation needs to send an event to the
 * topic manager, it shall invoke the [topic connection callbacks](@ref topic_connection_cb).
 *
 * Typically, a topic connection will be instantiated for each new remote topic_manager
 * connecting and it will be populated with the appropriate primitives depending
 * on the underlying protocol.
 *
 * Most implementations will "inherit" from this generic struct by declaring an
 * struct as member e.g.
 *
 * @code
 * struct ispp_topic_connection {
 *     struct topic_connection base;
 *     struct bt_conn *conn;
 * } my_ispp_conn;
 *
 * my_ispp_conn.base.list_req = ispp_list_req;
 * my_ispp_conn.base....
 *
 * @endcode
 *
 * And the ISPP primitives will retrieve the required information by using
 * a container_of macro.
 *
 * When a new ISPP connection is established, the application shall allocate
 * a new struct ispp_topic_connection and populate it before passing it to the
 * @ref topic_connection_on_connect_cb.
 *
 */

struct topic_connection {
	/**
	 * Send a list request on this connection.
	 *
	 * @param con pointer to this instance of the connection
	 * @param req_id identifier of the request
	 * @param pattern topic pattern (@ref topics)
	 * @param depth maximum depth of the topics in the returned list
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*list_req)(struct topic_connection * con, uint8_t req_id, const uint8_t * pattern, uint8_t depth);

	/**
	 * Send a list response on this connection.
	 *
	 * This function should be called in response to a received list request, the command_id
	 * that was received in the request should be sent back to this function as a parameter.
	 *
	 * @param con pointer to this instance of the connection
	 * @param req_id identifier of the request
	 * @param status status of the request (@ref topic_status)
	 * @param topic_ext_list topic extended list (@ref topics)
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*list_rsp)(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * topic_ext_list);

	/**
	 * Send a subscribe request on this connection.
	 *
	 * @param con pointer to this instance of the connection
	 * @param req_id identifier of the request
	 * @param topic_list list of topics to subscribe to (@ref topics)
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*subscribe_req)(struct topic_connection * con, uint8_t req_id, const uint8_t * topic_list);

	/**
	 * Send a subscribe response on this connection.
	 *
	 * This function should be called in response to a received subscribe request, the command_id
	 * that was received in the request should be sent back to this function as a parameter.
	 *
	 * @param con pointer to this instance of the connection
	 * @param req_id identifier of the request
	 * @param status status of the request @ref topic_status
	 * @param topic_list list of topics that accepted the request (@ref topics)
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*subscribe_rsp)(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * topic_list);

	/**
	 * Send an unsubscribe request on this connection.
	 *
	 * @param con pointer to this instance of the connection
	 * @param req_id identifier of the request
	 * @param topic_list list of topics to unsubscribe from (@ref topics)
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*unsubscribe_req)(struct topic_connection * con, uint8_t req_id, const uint8_t * topic_list);

	/**
	 * Send an unsubscribe response on this connection.
	 *
	 * This function should be called in response to a received unsubscribe request, the command_id
	 * that was received in the request should be sent back to this function as a parameter.
	 *
	 * @param con pointer to this instance of the connection
	 * @param req_id identifier of the request
	 * @param status status of the request @ref topic_status
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*unsubscribe_rsp)(struct topic_connection * con, uint8_t req_id, uint8_t status);

	/**
	 * Send a publish message on this connection.
	 *
	 * @param app pointer to the instance of the application
	 * @param con pointer to this instance of the connection
	 * @param topic topic to publish (@ref topics)
	 * @param data payload of the topic
	 * @param len length of the payload
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*publish)(const struct topic_application * app, struct topic_connection * con, const uint8_t * topic, const uint8_t * data, uint16_t len);

	/**
	 * Send a request message on this connection.
	 *
	 * @param con pointer to this instance of the connection
	 * @param req_id identifier of the request
	 * @param topic topic to request (@ref topics)
	 * @param data payload of the topic request
	 * @param len length of the payload
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*request)(struct topic_connection * con, uint8_t req_id, const uint8_t * topic, const uint8_t * data, uint16_t len);

	/**
	 * Send a response message on this connection.
	 *
	 * This function should be called in response to a received request, the command_id
	 * that was received in the request should be sent back to this function as a parameter.
	 *
	 * @param con pointer to this instance of the connection
	 * @param req_id identifier of the request
	 * @param status status of the request (@ref topic_status)
	 * @param data payload of the topic response (if NULL means there is no data)
	 * @param len length of the payload
	 *
	 * @return 0 in case of success, a negative value is an error.
	 *
	 * @note This function MUST be defined by the specific implementation of the @ref topic_connection
	 */
	int (*response)(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * data, uint16_t len);
};

/** @anchor topic_connection_cb */

/**
 * Called when a new connection was established and should be added to the
 * topic manager.
 *
 * @param con pointer to this instance of the connection
 */
void topic_connection_on_connect_cb(struct topic_connection * con);

/**
 * Called when a connection was dropped and should be removed from the
 * topic manager.
 *
 * @param con pointer to this instance of the connection
 */
void topic_connection_on_disconnect_cb(struct topic_connection * con);

/**
 * Called when a list request was received on the connection.
 *
 * @param con pointer to this instance of the connection
 * @param req_id identifier of the request
 * @param pattern topic match pattern (@ref topics)
 * @param depth maximum depth of the topics in the returned list
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_list_req_cb(struct topic_connection * con, uint8_t req_id, const uint8_t * pattern, uint8_t depth);

/**
 * Called when a list response was received on the connection.
 *
 * @param con pointer to this instance of the connection
 * @param req_id identifier of the request
 * @param status status of the request (@ref topic_status)
 * @param topic_ext_list topic extended list (@ref topics)
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_list_rsp_cb(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * topic_ext_list);

/**
 * Called when a subscribe request was received on this connection.
 *
 * @param con pointer to this instance of the connection
 * @param req_id identifier of the request
 * @param topic_list list of topics to subscribe to (@ref topics)
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_subscribe_req_cb(struct topic_connection * con, uint8_t req_id, const uint8_t * topic_list);

/**
 * Called when a subscribe response was received on this connection.
 *
 * @param con pointer to this instance of the connection
 * @param req_id identifier of the request
 * @param status status of the request (@ref topic_status)
 * @param topic_list list of topics that accepted the request (@ref topics)
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_subscribe_rsp_cb(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * topic_list);

/**
 * Called when a unsubscribe request was received on this connection.
 *
 * @param con pointer to this instance of the connection
 * @param req_id identifier of the request
 * @param pattern_list list of topic pattern (@ref topics)
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_unsubscribe_req_cb(struct topic_connection * con, uint8_t req_id, const uint8_t * pattern_list);

/**
 * Called when a unsubscribe response was received on this connection.
 *
 * @param con pointer to this instance of the connection
 * @param req_id identifier of the request
 * @param status status of the request (@ref topic_status)
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_unsubscribe_rsp_cb(struct topic_connection * con, uint8_t req_id, uint8_t status);

/**
 * Called when a publish message was received on this connection.
 *
 * @param con pointer to this instance of the connection
 * @param topic topic to publish (@ref topics)
 * @param data payload of the topic
 * @param len length of the payload
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_publish_cb(struct topic_connection * con, const uint8_t * topic, const uint8_t * data, uint16_t len);

/**
 * Called when a frame was sent on this connection.
 *
 * @param con pointer to this instance of the connection
 * @param params parameters of the event
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_tx_complete_cb(struct topic_connection * con, struct on_tx_complete_params * params);

/**
 * Called when a request message was received on this connection.
 *
 * @param con pointer to this instance of the connection
 * @param req_id identifier of the request
 * @param topic topic to request (@ref topics)
 * @param data payload of the topic request
 * @param len length of the payload
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_request_cb(struct topic_connection * con, uint8_t req_id, const uint8_t * topic, const uint8_t * data, uint16_t len);

/**
 * Called when a response message was received on this connection.
 *
 * @param con pointer to this instance of the connection
 * @param req_id identifier of the request
 * @param status status of the request (@ref topic_status)
 * @param data payload of the topic response
 * @param len length of the payload
 *
 * @note This function MUST be defined by the topic manager
 */
void topic_connection_on_response_cb(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * data, uint16_t len);

/** @} */

#endif /* TOPIC_CONNECTION_H_ */


