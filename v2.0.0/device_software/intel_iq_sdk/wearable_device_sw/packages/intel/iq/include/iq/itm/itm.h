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

/**
 * @defgroup itm ITM
 * @{
 * @ingroup services
 *
 * Intel Topic Management.
 *
 * This service is made of a single manager to which several
 * [applications](@ref topic_application) can register to offer for subscription
 * a list [topics](@ref topics).  These topics can then be subscribed by remotely
 * connected [applications](@ref topic_application).
 *
 * Example of an application registration:
 * @msc
 * main, app1, app2, topic_manager, connection1, connection2;
 * main box connection2 [label="... activity ..."];
 * connection1 box connection1 [label="topic connection established"];
 * topic_manager<=connection1 [label="topic_connection_on_connect_cb()", URL="@ref topic_connection_on_connect_cb()"];
 * main box connection2 [label="... activity ..."];
 * main=>app1 [label="start"];
 * app1 box app1 [label="fill the structure with the topic list and callbacks"];
 * app1=>topic_manager [label="itm_add_application()", URL="@ref itm_add_application()"];
 * app1=>topic_manager [label="itm_start_application()", URL="@ref itm_start_application()"];
 * app1<=topic_manager [label="on_connect(connection1)", URL="@ref topic_application::on_connect()"];
 * main box connection2 [label="... activity ..."];
 * connection2 box connection2 [label="topic connection established"];
 * topic_manager<=connection2 [label="topic_connection_on_connect_cb()", URL="@ref topic_connection_on_connect_cb()"];
 * app1<=topic_manager [label="on_connect(connection2)", URL="@ref topic_application::on_connect()"];
 * main box connection2 [label="... activity ..."];
 * main=>app2 [label="start"];
 * app2 box app2 [label="fill the structure with the topic list and callbacks"];
 * app2=>topic_manager [label="itm_add_application()", URL="@ref itm_add_application()"];
 * app2=>topic_manager [label="itm_start_application()", URL="@ref itm_start_application()"];
 * app2<=topic_manager [label="on_connect(connection1)", URL="@ref topic_application::on_connect()"];
 * app2<=topic_manager [label="on_connect(connection2)", URL="@ref topic_application::on_connect()"];
 * @endmsc
 *
 * Example of a list request:
 * @msc
 * app1, topic_manager1, local, remote, topic_manager2, app2;
 * app1 box app2 [label="applications, topic_managers and connections are initialized and ready for use"];
 * app1=>topic_manager1 [label="itm_list_req(local, pattern)", URL="@ref itm_list_req()"];
 * topic_manager1=>local [label="list_req(req_id, pattern)", URL="@ref topic_connection::list_req()"];
 * local box remote [label="transport"];
 * remote=>topic_manager2 [label="topic_connection_on_list_req_cb(req_id, pattern)", URL="@ref topic_connection_on_list_req_cb()"];
 * remote<=topic_manager2 [label="list_rsp(req_id, ext_list)", URL="@ref topic_connection::list_rsp()"];
 * local box remote [label="transport"];
 * topic_manager1<=local [label="topic_connection_on_list_rsp_cb(req_id, ext_list)", URL="@ref topic_connection_on_list_rsp_cb()"];
 * app1<=topic_manager1 [label="on_list_rsp(local, ext_list)", URL="@ref topic_application::on_list_rsp()"];
 * @endmsc
 *
 * Example of a subscribe request:
 * @msc
 * app1, topic_manager1, local, remote, topic_manager2, app2;
 * app1 box app2 [label="applications, topic_managers and connections are initialized and ready for use"];
 * app1 box app2 [label="app1 has discovered remote topics or knows about it (could have received a subscription)"];
 * app1=>topic_manager1 [label="itm_subscribe_req(local, pattern)", URL="@ref itm_subscribe_req()"];
 * topic_manager1=>local [label="subscribe_req(req_id, pattern)", URL="@ref topic_connection::subscribe_req()"];
 * local box remote [label="transport"];
 * remote=>topic_manager2 [label="topic_connection_on_subscribe_req_cb(req_id, pattern)", URL="@ref topic_connection_on_subscribe_req_cb()"];
 * topic_manager2=>app2 [label="on_subscribe_req(remote, topic)", URL="@ref topic_application::on_subscribe_req()"];
 * topic_manager2 box topic_manager2 [label="if pattern matches several topics, loop on above"];
 * remote<=topic_manager2 [label="subscribe_rsp(req_id, list)", URL="@ref topic_connection::subscribe_rsp()"];
 * local box remote [label="transport"];
 * topic_manager1<=local [label="topic_connection_on_subscribe_rsp_cb(req_id, list)", URL="@ref topic_connection_on_subscribe_rsp_cb()"];
 * app1<=topic_manager1 [label="on_subscribe_rsp(local, list)", URL="@ref topic_application::on_subscribe_rsp()"];
 * @endmsc
 *
 * Example of a publish and request:
 * @msc
 * app1, topic_manager1, local, remote, topic_manager2, app2;
 * app1 box app2 [label="app1 has subscribed to a topic offered by app2"];
 * topic_manager2<=app2 [label="itm_publish(remote, topic, data)", URL="@ref itm_publish()"];
 * remote<=topic_manager2 [label="publish(topic, data)", URL="@ref topic_connection::publish()"];
 * local box remote [label="transport"];
 * topic_manager1<=local [label="topic_connection_on_publish_cb(topic, data)", URL="@ref topic_connection_on_publish_cb()"];
 * app1<=topic_manager1 [label="on_publish(local, topic, data)", URL="@ref topic_application::on_publish()"];
 * topic_manager1 box topic_manager1 [label="the topic is broadcast to all applications because the subscriptions are not saved in topic manager"];
 * app1 box app1 [label="the application needs more information on this topic publish"];
 * app1=>topic_manager1 [label="itm_request(local, topic)", URL="@ref itm_request()"];
 * topic_manager1=>local [label="request(req_id, topic)", URL="@ref topic_connection::request()"];
 * local box remote [label="transport"];
 * remote=>topic_manager2 [label="topic_connection_on_request_cb(req_id, topic)", URL="@ref topic_connection_on_request_cb()"];
 * topic_manager2=>app2 [label="on_request(remote, req_id, topic)", URL="@ref topic_application::on_request()"];
 * topic_manager2<=app2 [label="itm_response(req_id, data)", URL="@ref itm_response()"];
 * remote<=topic_manager2 [label="response(req_id, data)", URL="@ref topic_connection::response()"];
 * local box remote [label="transport"];
 * topic_manager1<=local [label="topic_connection_on_response_cb(req_id, data)", URL="@ref topic_connection_on_response_cb()"];
 * app1<=topic_manager1 [label="on_response(local, data)", URL="@ref topic_application::on_response()"];
 * @endmsc
 *
 * Example of the simplest application offering only a topic:
 * @msc
 * main, app, topic_manager, connection;


 * main box connection [label="... activity ..."];
 * main=>app [label="start"];
 * app box app [label="fill the structure with the only topic in the topic list and no callbacks (eventually, only on_request)"];
 * app=>topic_manager [label="itm_add_application()", URL="@ref itm_add_application()"];
 * app=>topic_manager [label="itm_start_application()", URL="@ref itm_start_application()"];
 * main box connection [label="... activity ..."];
 * app=>topic_manager [label="itm_publish(topic, data)", URL="@ref itm_publish()"];
 * topic_manager -x connection [label="publish lost because there is not connection"];
 * main box connection [label="... activity ..."];
 * connection box connection [label="topic connection established"];
 * topic_manager<=connection [label="topic_connection_on_connect_cb()", URL="@ref topic_connection_on_connect_cb()"];
 * app x- topic_manager [label="on_connect lost because there is no callback"];
 * main box connection [label="... activity ..."];
 * app=>topic_manager [label="itm_publish(topic, data)", URL="@ref itm_publish()"];
 * topic_manager=>connection [label="publish(topic, data)", URL="@ref topic_connection::publish()"];
 * main box connection [label="... from this point on, all publish are forwarded to the connection ..."];
 * @endmsc
 *
 */

#ifndef ITM_H_
#define ITM_H_

#include <stdint.h>
#include <stdbool.h>
#include "util/compiler.h"

// Forward declarations
struct topic_application;

/**
 * @defgroup topics Topics
 *
 * # Description of the topics
 *
 * A topic is an element offered for subscription.  When subscribed, it can be
 * published at anytime by the publisher - when published it may carry some
 * data attached to the topic publish action.
 *
 * Each topic must be identified uniquely by a sequence of bytes terminating with
 * a NULL byte.  The depth of a topic is the number of bytes in the topic.  The depth
 * of the topic and the size of the data attached to the topic is application
 * specific as shown below.
 *
 * Examples of topics:
 * topic               | data when published
 * --------------------| ------------------------
 * NOTIFICATION/EMAIL  | email title (1 byte title length + title, limited to 32 chars)
 * NOTIFICATION/CALL   | phone call, string of chars, ending with 0, maximum 20 chars
 * TEST/COMMANDS/CMD   | test command to execute (32 bytes fixed)
 * TEST/COMMANDS/RSP   | test command status (1 byte fixed)
 *
 * NOTIFICATION/EMAIL is a topic of depth 2 and the string of bytes *could* be {0x10, 0x11, 0x00}.
 *
 * Nevertheless, some topic bytes are reserved for special meanings and the application can
 * not use them for other purposes.
 *
 * # Representation of the topics
 * A topic is represented by a NULL terminated string of bytes.
 *
 * Most topic bytes are unassigned and it is left to the applications
 * to verify that their topics are unique.
 *
 * Here are several examples of the same topic definitions:
@code
const uint8_t * topic1 = "234";
const uint8_t * topic2 = "\x32\x33\x34";
const uint8_t * topic3 = {0x32, 0x33, 0x34, 0};
const uint8_t   topic4[4] = {50, 51, 52, 0};
const uint8_t empty_topic[1] = {0};
@endcode
 *
 * A topic pattern is a topic which can contain some wildcard elements @ref topic_wildcards.  If
 * a topic pattern is totally empty (only NULL char), it will match all elements like a
 * @ref MULTI_LEVEL_WILDCARD_BYTE.
 *
 * Here are several examples of topic patterns:
@code
const uint8_t * pattern1 = "2*";
const uint8_t * pattern2 = "\x32*";
const uint8_t * pattern3 = {0x32, '*', 0};
const uint8_t * pattern4 = "\x32+";
@endcode
 *
 * A topic list is a single buffer containing concatenated topics.  The list is
 * terminated by an empty topic.  This means that an empty list
 * is a simple NULL char and a non empty-list ends with two NULL chars (the
 * last of the last topic and the empty topic indicating the end of the list).
 *
 * Here are several examples of topic lists:
@code
const uint8_t * list1 = "234\0235\0236\0";
const uint8_t * list2 = {0x32, 0x33, 0x34, 0, 0x32, 0x33, 0x35, 0, 0x32, 0x33, 0x36, 0, 0};
const uint8_t * empty_list = {0};
@endcode
 *
 * Here are several examples of topic arrays:
@code
uint8_t const * const * array1 = {"234", "235", "236", 0};
uint8_t const * const * array2 = {"\x32\x33\x34", "\x32\x33\x35", "\x32\x33\x36", 0};
uint8_t const * const * array3 = {{0x32, 0x33, 0x34, 0}, {0x19, 0x23, 0x34, 0}, 0};
@endcode
 *
 * A topic extended list is a topic list that can contain topics that end with a wildcard, this
 * simply indicates that there was no space to include all the topic but there are some
 * sub-topics present.
 *
 * Here are several examples of topic arrays:
@code
const uint8_t * ext_list = "2*";
@endcode
 * @{
 *
 * @defgroup topic_wildcards Topic Wildcards
 * @{
 */

#define TERMINATING_BYTE          0     /**< last byte of a topic */
#define ONE_LEVEL_WILDCARD_BYTE   0x2b  /**< '+', one level wildcard */
#define MULTI_LEVEL_WILDCARD_BYTE 0x2a  /**< '*', multi level wildcard */
#define TOPIC_SIZE_MAX            8     /**< Maximum supported size for the topics and
                                             the topic patterns (including terminating byte) */
#define TOPIC_LIST_MAX            64    /**< Maximum supported size for the topic lists and
                                             the extended topic lists (including terminating topic) */
/** @} */

/**
 * @defgroup topic_status Topic Status
 * @{
 */
#define TOPIC_STATUS_OK         0  /**< Everything is OK and all the subscriptions were accepted */
#define TOPIC_STATUS_PARTIAL_OK 1  /**< Everything is OK but some subscriptions were rejected */
#define TOPIC_STATUS_FAIL       2  /**< If there was a failure (and not the following ones)*/
#define TOPIC_STATUS_INVALID    3  /**< If the request was malformed */
#define TOPIC_STATUS_NOSPACE    4  /**< If the root pattern is too long */
#define TOPIC_STATUS_TOOLONG    5  /**< If the reply PDU size is not sufficient to hold all the topics
	                                     matching the root pattern */
/** @} */

/**
 * Compute the index of a topic in a topic array
 *
 * @param topic_array The topic array in which the topic is located
 * @param topic The pointer to the topic located inside the array
 *
 * @return The topic index in the array
 */
uint8_t topic_index(uint8_t const * const * topic_array, uint8_t const * const * topic) __attribute_const;

/**
 * Compute the size of a topic in bytes
 *
 * @param topic The topic to compute the size of
 *
 * @return The topic size in bytes, including the terminating 0
 */
uint8_t topic_size(const uint8_t * topic) __attribute__ ((pure));

/**
 * Compute a topic depth
 *
 * @param topic The topic to compute the depth of
 *
 * @return The topic hierarchical depth
 */
uint8_t topic_depth(const uint8_t * topic) __attribute__ ((pure));

/**
 * Compute the size of a topic list in bytes
 *
 * @param list The topic list to compute the size of
 *
 * @return The topic list size in bytes, including the terminating 0
 */
uint8_t topic_list_size(const uint8_t * list) __attribute__ ((pure));

/**
 * Find a topic in a NULL terminated array of topics
 *
 * @param topic_array The array of topics to search in
 * @param topic The topic to search for
 *
 * @return The index of the topic in the topic array, 0xFF if topic was not found
 */
uint8_t topic_find(uint8_t const * const * topic_array, const uint8_t * topic) __attribute__ ((pure));

/**
 * Check if a topic matches a pattern
 *
 * @param topic Topic to check against the pattern
 * @param pattern Filter to pass the topic in
 * @return true if the topic matches the pattern, otherwise false
 */
bool topic_match(const uint8_t * topic, const uint8_t * pattern) __attribute__ ((pure));

/**
 * Convert from a topic array to a topic list
 *
 * @param topic_list Topic list buffer for building the list
 * @param size Topic list buffer size
 * @param topic_array Topic array to convert
 * @return The status of the conversion
 * @retval 0 Success
 * @retval E_OS_ERR_NO_MEMORY The topic list buffer size was too small
 */
int topic_array_2_list(uint8_t * topic_list, uint8_t size, uint8_t const * const * topic_array);

/** @} */

/**
 * Add an application to the topic manager.
 *
 * @param app pointer to the instance of the application
 *
 * @return A positive value is the 8-bit identifier of the
 * application in the topic manager.  A negative value is
 * an error.
 * @retval E_OS_ERR_NO_MEMORY No more entries in the topic manager
 * @retval E_OS_ERR_NOT_ALLOWED A registered application has an identical topic
 *
 */
int itm_add_application(const struct topic_application * app);

/**
 * Start the application in the topic manager.
 *
 * Until this function is called, the topic manager will not
 * include the application topics nor will it call the application
 * callbacks on events.
 *
 * @param app_id identifier of the application in the topic manager
 *
 * @return 0 in case of success, a negative value is an error.
 * @retval E_OS_ERR app_id did not match an application
 *
 * @note This function allows adding the application while the
 * rest of the system is not ready to accept incoming events.
 */
int itm_start_application(uint8_t app_id);

/**
 * List topics offered by the remote topic manager on a connection.
 *
 * @param app_id identifier of the application in the topic manager
 * @param con_id identifier of the connection in the topic manager
 * @param pattern topic pattern (@ref topics)
 * @param depth maximum depth of the topics in the returned list
 *
 * @return A positive value is the 8-bit identifier of the request
 * passed to the response callback when response is received.
 * A negative value is an error.
 * @retval E_OS_ERR Application or connection identifier invalid
 * @retval E_OS_ERR_NOT_ALLOWED Application was not started
 *
 * @note This request will receive a response in the application
 * callback @ref topic_application::on_list_rsp.
 */
int itm_list_req(uint8_t app_id, uint8_t con_id, const uint8_t * pattern, uint8_t depth);

/**
 * Subscribe to topics coming from this connection.
 *
 * @param app_id identifier of the application in the topic manager
 * @param con_id identifier of the connection in the topic manager
 * @param topic_array array of topics to subscribe to (@ref topics)
 *
 * @return A positive value is the 8-bit identifier of the request
 * passed to the response callback when response is received.
 * A negative value is an error.
 * @retval E_OS_ERR Application or connection identifier invalid
 * @retval E_OS_ERR_NOT_ALLOWED Application was not started
 *
 * @note This request will receive a response in the application
 * callback @ref topic_application::on_subscribe_rsp.
 */
int itm_subscribe_req(uint8_t app_id, uint8_t con_id, uint8_t const * const * topic_array);

/**
 * Unsubscribe from topics coming from this connection.
 *
 * @param app_id identifier of the application in the topic manager
 * @param con_id identifier of the connection in the topic manager
 * @param topic_array array of topic to unsubscribe from (@ref topics)
 *
 * @return A positive value is the 8-bit identifier of the request
 * passed to the response callback when response is received.
 * A negative value is an error.
 * @retval E_OS_ERR Application or connection identifier invalid
 * @retval E_OS_ERR_NOT_ALLOWED Application was not started
 *
 * @note This request will receive a response in the application
 * callback @ref topic_application::on_unsubscribe_rsp.
 */
int itm_unsubscribe_req(uint8_t app_id, uint8_t con_id, uint8_t const * const * topic_array);

/**
 * Publish a new value on a topic.
 *
 * This procedure can be unicast (if the connection is specified) or
 * broadcast (if the connection is not specified).  In case of broadcast
 * it will be forwarded to all connections on which an accepted
 * subscription response was issued.
 *
 * @param app_id identifier of the application in the topic manager
 * @param con_id identifier of the connection in the topic manager (or 0xFF for broadcast)
 * @param topic_ix index of the topic in the application list of topics
 * @param data updated data of the topic
 * @param len length of the updated data
 *
 * @return The number of published messages queued for transmission, a negative value
 * is an error.  In case of unicast the return value is either 1 or a negative error code.
 * In case of broadcast, the return value is the number of successfully queued messages,
 * no error code is returned.
 * @retval E_OS_ERR Application or connection identifier invalid
 * @retval E_OS_ERR_NOT_ALLOWED Application was not started
 */
int itm_publish(uint8_t app_id, uint8_t con_id, uint8_t topic_ix, const uint8_t * data, uint16_t len);

/**
 * Request more data from topic on a remote topic_manager.
 *
 * This request should be issued as a result of a received
 * publish on the same topic.  An unexpected request
 * could lead to an error from the remote topic publisher.
 *
 * @param app_id identifier of the application in the topic manager
 * @param con_id identifier of the connection in the topic manager
 * @param topic topic on which custom request shall be sent
 * @param data data associated with this custom request
 * @param len length of the data associated with the custom request
 *
 * @return A positive value is the 8-bit identifier of the request
 * passed to the response callback when response is received.
 * A negative value is an error.
 * @retval E_OS_ERR Application or connection identifier invalid
 * @retval E_OS_ERR_NOT_ALLOWED Application was not started
 *
 * @note This request will receive a response in the application
 * callback @ref topic_application::on_response.
 */
int itm_request(uint8_t app_id, uint8_t con_id, const uint8_t * topic, const uint8_t * data, uint16_t len);

/**
 * Send the response to a topic request
 *
 * @param con_id identifier of the connection in the topic manager on which response should be sent
 * @param req_id identifier of the request received in the @ref topic_application::on_request
 * @param status status of the request (@ref topic_status)
 * @param data data to send in the response
 * @param len length of the data
 *
 * @return 0 in case of success, a negative value is an error.
 * @retval E_OS_ERR Connection identifier invalid
 */
int itm_response(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * data, uint16_t len);

#endif /* ITM_H_ */

/** @} */
