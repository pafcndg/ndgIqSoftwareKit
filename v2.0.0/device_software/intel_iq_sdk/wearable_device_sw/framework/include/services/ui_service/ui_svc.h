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

#ifndef _UI_SVC_H
#define _UI_SVC_H

#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "cfw/cfw_service.h"
#include "services/ui_service/ui_svc_utils.h"
#include "util/misc.h"
#include "drivers/led/led.h"
#include "drivers/haptic/haptic.h"
#ifdef CONFIG_HAS_TOUCH
#include "touch_api.h"
#endif

/**
 * @defgroup ui_service UI Service
 * Handles User Interface events and notifications.
 * @ingroup services
 */

 /**
 * @addtogroup ui_service_api
 * @{
 */

#define UISVC_MAIN_CLIENT_NAME "MAIN"

/* FIXME : uncomment line once all audio components will be complete   */
/*         and ready. As the UI is also listening to some audio        */
/*         messages and audio stream functions are not complete, we do */
/*         not want unexpected behavior at the moment.                 */
/* Enable audio interface of UI service. */
/*#define CONFIG_UI_AUDIO_ENABLE*/

/**
 * List of drivers identifiers.
 */
#define UI_DRV_ID_BASE           0x0300
#define UI_DRV_DEVICE_BIT_OFFSET 8
#define UI_DRV_DEVICE_MASK       ((1<<UI_DRV_DEVICE_BIT_OFFSET)-1)
#define UIDRV_LPAL_ID            (UI_DRV_ID_BASE + (1<<UI_DRV_DEVICE_BIT_OFFSET))
#define UIDRV_TOUCH_ID           (UI_DRV_ID_BASE + (2<<UI_DRV_DEVICE_BIT_OFFSET))
#define UIDRV_BTN_BASE           (UI_DRV_ID_BASE + (3<<UI_DRV_DEVICE_BIT_OFFSET))

/**
 * Indexes (bit position) for available events in the events masks.
 */
#define UI_EVT_IDX_BASE           0x0001
#define UI_LPAL_EVT_IDX           UI_EVT_IDX_BASE
#define UI_TOUCH_EVT_IDX          UI_EVT_IDX_BASE << 1
#define UI_TAP_EVT_IDX            UI_EVT_IDX_BASE << 2
#define UI_BTN_SINGLE_EVT_IDX     UI_EVT_IDX_BASE << 3
#define UI_BTN_DOUBLE_EVT_IDX     UI_EVT_IDX_BASE << 4
#define UI_BTN_MAX_EVT_IDX        UI_EVT_IDX_BASE << 5
#define UI_BTN_MULTIPLE_EVT_IDX   UI_EVT_IDX_BASE << 6


/**
 * Indexes for available notifications.
 */
#define UI_LED_NOTIF_IDX  0x0001
#define UI_VIBR_NOTIF_IDX 0x0002

/**
 * UI Service states.
 */
typedef enum {
    UI_SET_EVT_READY,
    UI_SET_EVT_BLOCKING
} ui_service_states_t;

/**
 * Structure for pairs of event identifier and associated service/driver.
 */
typedef struct {
    uint16_t event_id;
    uint16_t interface_id;
} ui_events_interfaces_list_t;

/**
 * Structure to store received event and response.
 */
typedef struct {
    struct cfw_message * msg_req;
    uint32_t events_req;
    uint32_t events_enable_req;
    uint32_t events_resp;
    uint32_t events_enable_resp;
} ui_events_req_t;

/**
 * Structure for tracking services opened by the UI Service.
 *
 * Filled when received event(s) request needs a service to be opened.
 */
typedef struct {
    uint16_t svc_id[MAX_SERVICES];
    uint8_t  svc_open[MAX_SERVICES];
    uint8_t  count;
} ui_svc_t;

/**
 * Messages for MSG_ID_UI_LPAL_EVT events.
 */
typedef struct ui_lpal_evt {
    struct cfw_message header;
    uint32_t ui_lpal_event_id;
} ui_lpal_evt_t;

/**
 * Message for MSG_ID_UI_LPAL_EVT events.
 */
typedef struct ui_lpal_evt_rsp {
    struct cfw_message header;
    uint32_t ui_lpal_event_rsp_id;
} ui_lpal_evt_rsp_t;

/**
 * Message for MSG_ID_UI_ASR_EVT events.
 */
typedef struct ui_asr_evt {
    struct cfw_message header;
    uint32_t ui_asr_event_id;
} ui_asr_evt_t;

/**
 * Message for MSG_ID_UI_ASR_EVT events.
 */
typedef struct ui_asr_evt_rsp {
    struct cfw_message header;
    uint32_t ui_asr_event_rsp_id;
} ui_asr_evt_rsp_t;

/**
 * Message for ui_get_available_features response.
 */
typedef struct ui_get_available_features_rsp {
    struct cfw_rsp_message header;
    uint32_t events;
    uint32_t notifications;
} ui_get_available_features_rsp_t;

/**
 * Message for ui_get_enabled_events_rsp response.
 */
typedef struct ui_get_events_rsp {
    struct cfw_rsp_message header;
    uint32_t events;
} ui_get_events_rsp_t;

/**
 * Message for ui_set_enabled_events_req request.
 */
typedef struct ui_set_events_req {
    struct cfw_message header;
    uint32_t mask;
    uint32_t enable;
} ui_set_events_req_t;

/**
 * Message for ui_play_led_pattern_req request.
 */
typedef struct ui_play_led_pattern_req {
    struct cfw_message header;
    enum led_type type;
    uint8_t led_id;
    led_s pattern;
} ui_play_led_pattern_req_t;

/**
 * Message for MSG_ID_UI_LED_RSP events.
 */
typedef struct ui_play_led_pattern_rsp {
    struct cfw_rsp_message header;
    uint8_t led_id;
} ui_play_led_pattern_rsp_t;

/**
 * Message for ui_play_vibr_pattern_req request.
 */
typedef struct ui_play_vibr_pattern_req {
    struct cfw_message header;
    vibration_type type;
    vibration_u pattern;
} ui_play_vibr_pattern_req_t;

/**
 * Retrieve all supported user events and user notifications.
 * @param  service_conn service connection pointer
 * @param  priv private data pointer that will be passed sending answer.
 * @return error code
 */
int8_t ui_get_available_features(cfw_service_conn_t *service_conn, void *priv);

/**
 * Retrieve all user events enabled.
 * @param  service_conn service connection pointer
 * @param  priv private data pointer that will be passed sending answer.
 * @return error code
 */
int8_t ui_get_enabled_events(cfw_service_conn_t *service_conn, void *priv);

/**
 * Enable one or several events.
 * @param  service_conn service connection pointer
 * @param  mask bit field to select event(s) to be changed
 * @param  enable bit field to enable=1/disable=0 selected event(s).
 * @param  priv private data pointer that will be passed sending answer.
 * @return error code
 */
int8_t ui_set_enabled_events(cfw_service_conn_t *service_conn,
            uint32_t mask, uint32_t enable, void *priv);

/**
 * Gets available events.
 *
 * @return mask with bit fields representing event(s).
 */
uint32_t ui_get_events_list(void);

/** @} */

#endif
