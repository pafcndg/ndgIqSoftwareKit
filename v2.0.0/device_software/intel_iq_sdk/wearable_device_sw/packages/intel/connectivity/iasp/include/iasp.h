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

#ifndef IASP_H_
#define IASP_H_

/**
 * @defgroup iasp IASP
 * @ingroup services
 *
 * Intel Accessory Service Protocol.
 *
 * @{
 */

#include <stdint.h>
#include "util/compiler.h"

//#define ISPP_TEST /* ruggedize ispp test */

/* Define the number of channels, can not be lower than the number of official channels */
#ifndef IASP_MAX_CHANNELS
#ifndef ISPP_TEST
#define IASP_MAX_CHANNELS 3
#else
#define IASP_MAX_CHANNELS 4
#endif
#endif

/* Official channels */
#define IASP_LOGGING_CHANNEL    0x06
#define IASP_ITM_CHANNEL        0x1E
#define IASP_LOOPBACK_CHANNEL   0x1F
#define IASP_TEST_CHANNEL       0x10
#define IASP_CHANNEL_MASK       0x1F

enum iasp_events {
	IASP_OPEN,
	IASP_CLOSE,
	IASP_RX_COMPLETE,
	IASP_TX_COMPLETE,
};

struct iasp_open {
	uint16_t mtu;
};

struct iasp_rx {
	uint16_t len;
	uint8_t *buf;
};

struct iasp_tx {
	int status;
	void *context;
	uint8_t *buf;
	uint16_t len;
};

struct iasp_event {
	enum iasp_events event;
	union {
		struct iasp_open open;
		struct iasp_rx rx;
		struct iasp_tx tx;
	} evt;

};

typedef void (*iasp_evt_handler_t)(const struct iasp_event *p_iasp_evt);

/**
 * Function for handling an IASP event on the official channel.
 *
 * This function can be NOT defined if it is not required for
 * the application.
 *
 * @param p_iasp_evt Pointer to the IASP event.
 */
void iasp_logging_channel_handler(const struct iasp_event *p_iasp_evt)
	__weak;

/**
 * Function for handling an IASP event on the official channel.
 *
 * This function can be NOT defined if it is not required for
 * the application.
 *
 * @param p_iasp_evt Pointer to the IASP event.
 */
void iasp_itm_channel_handler(const struct iasp_event *p_iasp_evt)
	__weak;

/**
 * Function for writing a message to an IASP channel.
 *
 * @param context Pointer to the user private data (returned on tx complete event).
 * @param channel The channel on which the data shall be sent.
 * @param p_extra_prefix An optional additional prefix to add before the data.
 * @param extra_prefix_len The length of the optional prefix.
 * @param p_data The Message to send.
 * @param data_len Length of the message.
 *
 * @return 0 upon success, otherwise a negative error code.
 * @retval E_OS_ERR_BUSY Transmission queue is full.
 * @retval E_OS_ERR If Transmission queue is not ready or invalid packet length.
 * @retval E_OS_ERR_NOT_SUPPORTED Channel is not registered
 */
int iasp_write(void *context, uint8_t channel, const uint8_t *p_extra_prefix,
		uint16_t extra_prefix_len, const uint8_t *p_data,
		uint16_t data_len);

/**
 * Function for registering a non-official channel.
 *
 * @param channel The new non-official channel identifier.
 * @param handler Handler invoked on IASP events on this channel.
 *
 * @return 0 upon success, otherwise an error code.
 * @retval E_OS_ERR_NOT_SUPPORTED Channel is already registered (official or non-official)
 * @retval E_OS_ERR_NOT_ALLOWED The parameters are not supported (channel not in range or handler undefined)
 * @retval E_OS_ERR_OVERFLOW There is no more space to allocate a new channel
 */
int iasp_register(uint8_t channel, iasp_evt_handler_t handler);

/**
 * Function for un-registering a non-official channel.
 *
 *@param[in] channel The non-official channel to unregister.
 *
 * @return 0 upon success, otherwise an error code.
 * @retval E_OS_ERR_NOT_SUPPORTED Channel is not registered
 */
int iasp_unregister(uint8_t channel);

/** @} */

#endif /* IASP_H_ */

