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

#ifndef TOPIC_APPLICATION_H_
#define TOPIC_APPLICATION_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup topic_application_grp Topic application
 * @{
 * @ingroup itm
 */

// Forward declarations
struct topic_connection;

/**
 * @defgroup message_type Message type
 * @{
 */
#define MSG_LIST_REQUEST 0
#define MSG_REQUEST      1
#define MSG_RESPONSE     2
#define MSG_SUBSCRIBE    3
#define MSG_UNSUBSCRIBE  4
#define MSG_PUBLISH      5

/** @} */

/**
 * Parameters of transmission complete event.
 */
struct on_tx_complete_params {
	int status; /**< Status of the transmission */
	uint8_t *p_data; /**< Data transmitted */
	uint16_t len; /**< Length of the data transmitted */
	uint8_t msg_type; /**< Type of the message transmitted @ref message_type */
	union {
		struct topic_application *app; /**< For publish messages (@ref msg_type): pointer to the application */
		uint8_t req_id;  /**< For other messages (@ref msg_type): request ID */
	};
};

/**
 * Describe a topic application.
 */
struct topic_application
{
	/**
	 * Array of topics offered by the application (@ref topics).
	 */
	uint8_t const * const * topic_array;

	/**
	 * Called when there is a new connection established.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 */
	void (*on_connect)(uint8_t con_id);

	/**
	 * Called when there is a connection dropped.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 *
	 */
	void (*on_disconnect)(uint8_t con_id);

	/**
	 * Called when a list response was received on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param req_id identifier of the request
	 * @param status status of the request (@ref topic_status)
	 * @param topic_ext_list topic extended list (@ref topics)
	 *
	 * @note This is a response to an application list request. The returned
	 * extended topic list was built by running the following procedure
	 * on all the topics of the remote topic manager:
	 * - if the topic does not match the pattern, do not go further
	 * - if the topic depth is greater than the requested maximum depth,
	 *   the topic will be truncated to the maximum depth and a multilevel
	 *   wildcard will be appended to indicate that there are sub-topics
	 *   to be discovered.
	 * - If there are several sub-topics to be discovered, only one
	 *   topic with an appended multilevel wildcard will be present in the reply.
	 * The supported status codes are:
	 * - @ref TOPIC_STATUS_INVALID if the request was malformed
	 * - @ref TOPIC_STATUS_NOSPACE if the root pattern is too long
	 * - @ref TOPIC_STATUS_TOOLONG if the reply PDU size is not sufficient to hold all the topics
	 *        matching the root pattern
	 * - @ref TOPIC_STATUS_FAIL if there was another failure
	 */
	void (*on_list_rsp)(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * topic_ext_list);

	/**
	 * Called when a subscribe request was received on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param topic_ix index of the topic in the application list of topics
	 *
	 * @retval true if the subscription request is accepted
	 * @retval false if the subscription request is rejected
	 *
	 * @note if this callback is not implemented (NULL), all subscriptions will
	 * be automatically accepted.
	 */
	bool (*on_subscribe_req)(uint8_t con_id, uint8_t topic_ix);

	/**
	 * Called when a subscribe response was received on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param req_id identifier of the request
	 * @param status status of the request @ref topic_status
	 * @param topic_list list of topics that accepted the request (@ref topics)
	 *
	 * @note The supported status codes are:
	 * - @ref TOPIC_STATUS_OK if everything was okay and all subscriptions were accepted
	 * - @ref TOPIC_STATUS_PARTIAL_OK if everything was okay but some subscriptions were rejected (see accepted list)
	 * - @ref TOPIC_STATUS_INVALID if the request is malformed
	 */
	void (*on_subscribe_rsp)(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * topic_list);

	/**
	 * Called when a subscribe is accepted.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param topic_ix index of the topic in the application list of topics
	 */
	void (*on_subscribed)(uint8_t con_id, uint8_t topic_ix);

	/**
	 * Called when a unsubscribe request was received on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param topic_ix index of the topic in the application list of topics
	 */
	void (*on_unsubscribe_req)(uint8_t con_id, uint8_t topic_ix);

	/**
	 * Called when an unsubscribe response was received on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param req_id identifier of the request
	 * @param status status of the request @ref topic_status
	 *
	 * @note The supported status codes are:
	 * - @ref TOPIC_STATUS_OK if everything was okay
	 * - @ref TOPIC_STATUS_FAIL if all topics in list were not subscribed
	 * - @ref TOPIC_STATUS_PARTIAL_OK if everything was okay but some unsubscriptions were rejected (see accepted list)
	 * - @ref TOPIC_STATUS_INVALID if the request is malformed
	 */
	void (*on_unsubscribe_rsp)(uint8_t con_id, uint8_t req_id, uint8_t status);

	/**
	 * Called when a publish message was received on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param topic published topic (@ref topics)
	 * @param data payload of the topic
	 * @param len length of the payload
	 */
	void (*on_publish)(uint8_t con_id, const uint8_t * topic, const uint8_t * data, uint16_t len);

	/**
	 * Called when a message was sent on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param params parameters of the event
	 *
	 */
	void (*on_tx_complete_cb)(uint8_t con_id, struct on_tx_complete_params * params);

	/**
	 * Called when a request message was received on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param req_id identifier of the request which will need to be passed to the response
	 * @param topic_ix index of the topic in the application list of topics
	 * @param data payload of the topic request
	 * @param len length of the payload
	 */
	void (*on_request)(uint8_t con_id, uint8_t req_id, uint8_t topic_ix, const uint8_t * data, uint16_t len);

	/**
	 * Called when a response message on a connection.
	 *
	 * This callback can be set to NULL.
	 *
	 * @param con_id identifier of the connection in the topic manager
	 * @param req_id identifier of the request
	 * @param status status of the request (@ref topic_status)
	 * @param data payload of the topic response
	 * @param len length of the payload
	 */
	void (*on_response)(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * data, uint16_t len);
};

/** @} */

#endif /* TOPIC_APPLICATION_H_ */
