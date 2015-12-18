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

#ifndef TOPIC_IASP_H_
#define TOPIC_IASP_H_

/**
 * @defgroup topic_connection_iasp Topic connection over IASP
 * @{
 * @ingroup topic_connection_grp
 *
 * This module implements the topic connection over IASP, given the nature of the
 * device, it will act as a server and therefore, it is not able to initiate the connection
 * establishment.
 *
 * In the below sequence charts, the payload of the IASP message is represented in a byte array format,
 * when the bytes represent a parameter to or from the interface, they are represented using the first letter
 * of that parameter.  Here is an example:
 *
 * @code
 *     list_rsp(req_id, status, ext_list)
 * maps to IASP message:
 *     LIST_RSP([02 RR SS LL....])
 * @endcode
 *
 * In this example, the LIST_RSP message show as:
 * - 02 : message identifier
 * - RR : the list_rsp req_id parameter
 * - SS : the list_rsp status parameter
 * - LL... : the list_rsp variable length ext_list parameter
 *
 *
 * Example of connection establishment:
 * @msc
 * topic_manager, server, client;
 * server <= client [label="PROBE_REQ([FF])"];
 * server => client [label="PROBE_RSP([FE])"];
 * topic_manager <= server [label="topic_connection_on_connect_cb()", URL="@ref topic_connection_on_connect_cb()"];
 * @endmsc
 *
 * Example of disconnection:
 * @msc
 * topic_manager, server, client;
 * server <- client [label="link lost"];
 * topic_manager <= server [label="topic_connection_on_disconnect_cb()", URL="@ref topic_connection_on_disconnect_cb()"];
 * @endmsc
 *
 * Example of receiving a list request (to retrieve the list of topics offered by the local ITM manager):
 * @msc
 * topic_manager, server, client;
 * server <= client [label="LIST_REQ([01 RR DD PP....])"];
 * topic_manager <= server [label="topic_connection_on_list_req_cb(req_id, pattern, depth)", URL="@ref topic_connection_on_list_req_cb()"];
 * topic_manager => server [label="list_rsp(req_id, status, ext_list)", URL="@ref topic_connection::list_rsp()"];
 * server => client [label="LIST_RSP([02 RR SS LL....])"];
 * @endmsc
 *
 * Example of receiving a subscribe request (remote ITM manager wants to receive publications from the local ITM manager):
 * @msc
 * topic_manager, server, client;
 * server <= client [label="SUBSCRIBE_REQ([03 RR LL....])"];
 * topic_manager <= server [label="topic_connection_on_subscribe_req_cb(req_id, list)", URL="@ref topic_connection_on_subscribe_req_cb()"];
 * topic_manager => server [label="subscribe_rsp(req_id, status, ext_list)", URL="@ref topic_connection::subscribe_rsp()"];
 * server => client [label="SUBSCRIBE_RSP([04 RR SS LL....])"];
 * @endmsc
 *
 * Example of unsubscribe request (remote ITM manager does not want to receive publications from the local ITM manager anymore):
 * @msc
 * topic_manager, server, client;
 * server <= client [label="UNSUBSCRIBE_REQ([05 RR LL....])"];
 * topic_manager <= server [label="topic_connection_on_unsubscribe_req_cb(req_id, list)", URL="@ref topic_connection_on_unsubscribe_req_cb()"];
 * topic_manager => server [label="unsubscribe_rsp(req_id, status)", URL="@ref topic_connection::unsubscribe_rsp()"];
 * server => client [label="UNSUBSCRIBE_RSP([06 RR SS])"];
 * @endmsc
 *
 * Example of publish (remote ITM manager issued a publication to the local ITM manager):
 * @msc
 * topic_manager, server, client;
 * server <= client [label="PUBLISH([07 TT.... DD....])"];
 * topic_manager <= server [label="topic_connection_on_publish_cb(topic, data)", URL="@ref topic_connection_on_publish_cb()"];
 * @endmsc
 *
 * Example of topic request (remote ITM manager topic issued a specific topic request to the local ITM manager):
 * @msc
 * topic_manager, server, client;
 * server <= client [label="TOPIC_REQ([08 RR TT.... DD....])"];
 * topic_manager <= server [label="topic_connection_on_request_cb(req_id, topic, data)", URL="@ref topic_connection_on_request_cb()"];
 * topic_manager => server [label="response(req_id, status, data)", URL="@ref topic_connection::unsubscribe_rsp()"];
 * server => client [label="SUBSCRIBE_RSP([09 RR SS DD....])"];
 * @endmsc
 *
 */

/** @} */

#endif /* TOPIC_IASP_H_ */

