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

#ifndef _UI_SVC_API_H
#define _UI_SVC_API_H

#include <stdint.h>
#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "cfw/cfw_service.h"
#include "services/services_ids.h"
#include "drivers/led/led.h"
#include "drivers/haptic/haptic.h"
#include "lib/button/button.h"

/**
 * @defgroup ui_service_api UI Service API
 * Define the interface of User Interface service.
 * @ingroup ui_service
 * @{
 */

/* Uncomment when testing the UI Service: */
#define TEST_UI_SVC 1

#ifdef TEST_UI_SVC
#define TEST_SVC_ID (MAX_SERVICES)
#define UI_TEST_EVT_IDX UI_EVT_IDX_BASE << 15
#endif

#define UISVC_NAME "UIService"


/**
 * User Interaction service external message IDs.
 */
#define MSG_ID_UI_BASE                 (MSG_ID_UI_SERVICE_BASE + 0x00)
#define MSG_ID_UI_RSP_BASE             (MSG_ID_UI_SERVICE_BASE + 0x40)
#define MSG_ID_UI_EVT_BASE             (MSG_ID_UI_SERVICE_BASE + 0x80)

#define MSG_ID_UI_GET_FEAT_REQ         (MSG_ID_UI_BASE + 0x01)
#define MSG_ID_UI_GET_EVT_REQ          (MSG_ID_UI_BASE + 0x02)
#define MSG_ID_UI_SET_EVT_REQ          (MSG_ID_UI_BASE + 0x03)
#define MSG_ID_UI_LED_REQ              (MSG_ID_UI_BASE + 0x04)
#define MSG_ID_UI_LPAL_REQ             (MSG_ID_UI_BASE + 0x05)
#define MSG_ID_UI_ASR_REQ              (MSG_ID_UI_BASE + 0x06)
#define MSG_ID_UI_VIBR_REQ             (MSG_ID_UI_BASE + 0x07)
#define MSG_ID_UI_BTN_SET_TIMING_REQ   (MSG_ID_UI_BASE + 0x08)

#define MSG_ID_UI_GET_FEAT_RSP         (MSG_ID_UI_GET_FEAT_REQ | MSG_ID_UI_RSP_BASE)
#define MSG_ID_UI_GET_EVT_RSP          (MSG_ID_UI_GET_EVT_REQ | MSG_ID_UI_RSP_BASE)
#define MSG_ID_UI_SET_EVT_RSP          (MSG_ID_UI_SET_EVT_REQ | MSG_ID_UI_RSP_BASE)
#define MSG_ID_UI_LED_RSP              (MSG_ID_UI_LED_REQ | MSG_ID_UI_RSP_BASE)
#define MSG_ID_UI_VIBR_RSP             (MSG_ID_UI_VIBR_REQ | MSG_ID_UI_RSP_BASE)
#define MSG_ID_UI_LPAL_RSP             (MSG_ID_UI_LPAL_REQ | MSG_ID_UI_RSP_BASE)
#define MSG_ID_UI_ASR_RSP              (MSG_ID_UI_ASR_REQ | MSG_ID_UI_RSP_BASE)


#define MSG_ID_UI_LPAL_EVT             (MSG_ID_UI_EVT_BASE + 0x01)
#define MSG_ID_UI_ASR_EVT              (MSG_ID_UI_EVT_BASE + 0x02)
#define MSG_ID_UI_TOUCH_EVT            (MSG_ID_UI_EVT_BASE + 0x03)
#define MSG_ID_UI_TAP_EVT              (MSG_ID_UI_EVT_BASE + 0x04)
#define MSG_ID_UI_BTN_SINGLE_EVT       (MSG_ID_UI_EVT_BASE + 0x05)
#define MSG_ID_UI_BTN_DOUBLE_EVT       (MSG_ID_UI_EVT_BASE + 0x06)
#define MSG_ID_UI_BTN_MAX_EVT          (MSG_ID_UI_EVT_BASE + 0x07)
#define MSG_ID_UI_BTN_MULTIPLE_EVT     (MSG_ID_UI_EVT_BASE + 0x08)

/**
 * User Interaction service internal message IDs.
 */
#define MSG_ID_UI_LOCAL_TOUCH_EVT       (MSG_ID_UI_EVT_BASE + 0x07)
#define MSG_ID_UI_LOCAL_TAP_EVT         (MSG_ID_UI_EVT_BASE + 0x08)
#define MSG_ID_UI_LOCAL_LED_EVT         (MSG_ID_UI_EVT_BASE + 0x09)

/**
 * List of status and error codes.
 */
#define UI_SUCCESS        0
#define UI_ERROR         -1
#define UI_BUSY          -2
#define UI_NOTFOUND      -3
#define UI_OPENSVC_FAIL  -4
#define UI_EVTREQ_FAIL   -5
#define UI_INVALID_PARAM -6
#define UI_INVALID_REQ   -7
#define UI_NO_DRV        -8
#define UI_ABORT         -9

/**
 * UI service configuration structure
 */
struct ui_config {
    uint8_t btn_count;   /*!< Number of buttons available */
    struct button *btns; /*!< Button list used by UI service */
    uint8_t led_count;   /*!< Number of LEDs available */
    struct led *leds;    /*!< Led list used by UI service */
};

/**
 * Message for MSG_ID_UI_TOUCH_EVT events.
 */
typedef struct ui_touch_evt {
    struct cfw_message header;
    uint32_t gesture_id;
} ui_touch_evt_t;

/**
 * Message for MSG_ID_UI_TAP_EVT events.
 */
typedef struct ui_tap_evt {
    struct cfw_message header;
    uint32_t type;
} ui_tap_evt_t;

/**
 * Message for MSG_ID_UI_BTN_EVT events.
 */
typedef struct ui_button_evt {
    struct cfw_message header;
    uint8_t btn;
    uint32_t param;
} ui_button_evt_t;

/**
 * Drivers events union.
 */
union ui_drv_evt {
    ui_touch_evt_t touch_evt;
    ui_tap_evt_t tap_evt;
    ui_button_evt_t btn_evt;
};

/**
 * Play a led pattern on LED1 or LED2.
 * @param  service_conn service connection pointer
 * @param  led_id LED1 or LED2
 * @param  type LED pattern type
 * @param  pattern pointer to LED pattern data
 * @param  priv private data pointer that will be passed sending answer.
 * @return error code
 */
int8_t ui_play_led_pattern(cfw_service_conn_t *service_conn, uint8_t led_id,
            enum led_type type, led_s * pattern, void *priv);

/**
 * Play a vibration pattern.
 * @param  service_conn service connection pointer
 * @param  type pattern type to be played by the DRV2605 haptic driver
 * @param  pattern pointer to pattern data
 * @param  priv private data pointer that will be passed sending answer.
 * @return error code
 */
int8_t ui_play_vibr_pattern(cfw_service_conn_t *service_conn,
            vibration_type type, vibration_u * pattern, void *priv);

/**
 * API function called when a audio tone or sentence has to be played.
 * @param  service_conn Service connection.
 * @param  audio_resp_id Audio data to be played.
 * @param  priv Additional data. NULL if not used.
 * @return UI_SUCCESS on success, else error code.
 */
int8_t ui_audio_response(cfw_service_conn_t * service_conn,
            uint32_t audio_resp_id, void * priv);

/**
 * Initialize and register the UI Service.
 * @param ui_svc_queue UI Service queue for messages.
 * @param config UI Service configuration structure.
 * @return none
 */
void ui_service_init(void * ui_svc_queue, struct ui_config *config);

/**
 * Stop the UI Service.
 *
 * @return error code
 */
int8_t ui_service_stop(void);

/** @} */

#endif /* ifndef _UI_SVC_API_H */
