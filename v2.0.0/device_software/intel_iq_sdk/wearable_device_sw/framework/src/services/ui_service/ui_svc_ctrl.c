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

#include "services/ui_service/ui_svc.h"
#include "services/ui_service/ui_svc_api.h"
#include "cfw/cfw_messages.h"
#include "infra/log.h"

#define PANIC_FAIL_1 1
#define PANIC_FAIL_2 2

extern ui_events_req_t ui_events_req;
extern struct ui_config *ui_config;
extern void *ui_svc_queue_msg;
extern ui_events_interfaces_list_t ui_events_interfaces_list[MAX_SERVICES];

extern void ui_handle_events_callback(uint32_t item, uint32_t param);

static uint8_t ui_svc_pending_open;
static int reg_events[20];
static int8_t reg_count;
static ui_svc_t ui_svc_table;

/**@brief Function to check if service has already been opened by the
 *        ui service.
 *
 * @details It parses the table of service already opened by the ui service
 *          since boot time. If found, current status is returned (open/closed).
 * @param[in]  svc Service id.
 * @return   Service status, UI_NOTFOUND if not found.
 */
int8_t ui_is_client_service_opened(uint16_t svc)
{
    uint8_t idx;

    /* Check if the service has already been managed by the UI service, */
    /* and is already open.                                             */
    for (idx = 0; idx < ui_svc_table.count; idx++) {
        if (ui_svc_table.svc_id[idx] == svc)
            return (ui_svc_table.svc_open[idx]);
    }

    return UI_NOTFOUND;
}

/**@brief Function to update state of the service being opened/closed by the
 *        ui service.
 *
 * @details If service present in the table of service already opened by the
 *          ui service, its state is upaded (open = 1, close = 0). If not
 *          found it is added to the table.
 *          Service state is upaded when the ui service has completed a request
 *          to set events (request making necessary to open/close a service).
 * @param[in]  svc  Service id.
 * @param[in]  open Final state of the service.
 * @return   none.
 */
void ui_update_client_service_state(uint16_t svc, uint8_t open)
{
    uint8_t idx;

    for (idx = 0; idx < ui_svc_table.count; idx++) {
        /* Service listed. Just update its state for tracking. */
        if (ui_svc_table.svc_id[idx] == svc) {
            ui_svc_table.svc_open[idx] = open;
            return;
        }
    }

    /* Add the list for tracking managed services. */
    ui_svc_table.svc_id[ui_svc_table.count] = svc;
    ui_svc_table.svc_open[ui_svc_table.count] = open;
    ui_svc_table.count++;
}

/**@brief Function to initialize table of services that will be opened/closed
 *        by the ui service.
 *
 * @details This table and its content is only used when managing a request
 *          to set events (request making necessary to open/close a service).
 * @return   none.
 */
void ui_init_client_service_table(void)
{
    ui_svc_table.count = 0;
}

/**@brief Function to registers events for a service being opened by the
 *        ui service.
 *
 * @details Gets the event mask received earlier by the ui service, verifies
 *          this is a known one, and registers them to the framework.
 *          Function executed only when receiving MSG_ID_CFW_OPEN_SERVICE_REQ
 *          form the framework, after the ui service has requested the
 *          framework to open the new service.
 * @param[in]  msg Message from framework.
 * @return   none.
 */
void ui_register_events_for_service(struct cfw_message * msg)
{
    uint8_t idx;
    cfw_service_conn_t *service_conn = NULL;
    cfw_open_conn_rsp_msg_t *cnf;

    pr_debug(LOG_MODULE_UI_SVC,
        "UI service: register evts for svc:%d : evt_mask: 0x%X set_mask:0x%X",
        ui_svc_pending_open,
        ui_events_req.events_req,
        ui_events_req.events_enable_req);

    cnf = (cfw_open_conn_rsp_msg_t *) msg;
    if (cnf == NULL) panic(PANIC_FAIL_1);

    service_conn = cnf->service_conn;

    pr_debug(LOG_MODULE_UI_SVC,
        "UI service: conn:0x%X service_conn:0x%X for svc:%d",
        cnf,
        service_conn,
        ui_svc_pending_open);

    reg_count = 0;

    /* Build list of events to be registered.                          */
    /* For each event to be set, and contained in the request received */
    /* by the ui service, it is verified if it is known and associated */
    /* with the right service waiting to be opened.                    */
    for (idx = 0; idx < ARRAY_SIZE(ui_events_interfaces_list); idx++) {
        if (ui_events_interfaces_list[idx].interface_id == ui_svc_pending_open) {
            if (TEST_EVT (ui_events_interfaces_list[idx].event_id,
                    ui_events_req.events_req)) {
                reg_events[reg_count] = ui_events_interfaces_list[idx].event_id;
                reg_count++;
            }
        }
    }

    cfw_register_events(service_conn, reg_events, reg_count, msg->priv);
}

/**@brief Function to handle result of events registration to the framework.
 *
 * @details Function executed only when receiving MSG_ID_CFW_REGISTER_EVT
 *          form the framework, after the ui service has sent a registration
 *          request to the for a new service.
 *          For each registered event, the result is passed to a callback
 *          which in turn builds the response for a request received earlier
 *          by the ui service.
 * @param[in]  msg Message from framework.
 * @return   none.
 */
void ui_handle_events_registration(struct cfw_message * msg)
{
    int idx;

    pr_debug(LOG_MODULE_UI_SVC,
        "UI service: events registered for svc:%d\n", ui_svc_pending_open);

    /* Success. Update table used to track the opened/closed services */
    /* by the UI service.                                             */
    ui_update_client_service_state(ui_svc_pending_open, 1);

    for (idx = 0; idx < reg_count; idx++)
        ui_handle_events_callback(reg_events[idx],
            ui_events_req.events_enable_req);
}

/**@brief Function to close a service previously opened by the ui service.
 *
 * @details Updates the table used to track services manages by the ui service.
 * @param[in]  msg Message from framework.
 * @return   none.
 */
void ui_handle_close_service(struct cfw_message * msg)
{
    conn_handle_t * conn = (conn_handle_t*)msg->conn;
    if (conn == NULL) panic(PANIC_FAIL_1);

    ui_update_client_service_state(conn->svc->service_id, 0);
}

/**@brief Function to handle messages received from the framework.
 *
 * @details Used only for managing services opened/closed by the ui service
 *          upon reception of event(s) requests.
 * @param[in]  msg   Pointer to message.
 * @param[in]  param Additional data. NULL if not used.
 * @return   none.
 */
static void ui_handle_service_message(struct cfw_message * msg, void *param)
{
    pr_debug(LOG_MODULE_UI_SVC,
        "UI service ctrl: handle msg for service %d: msg: %d src: %d dst: %d\n",
        ui_svc_pending_open,
        CFW_MESSAGE_ID(msg),
        CFW_MESSAGE_SRC(msg),
        CFW_MESSAGE_DST(msg));

    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_CFW_OPEN_SERVICE_RSP:
        pr_debug(LOG_MODULE_UI_SVC, "UI service ctrl msg open: %d", MSG_ID_CFW_OPEN_SERVICE_RSP);
        ui_register_events_for_service(msg);
        break;
    case MSG_ID_CFW_CLOSE_SERVICE_RSP:
        pr_debug(LOG_MODULE_UI_SVC, "UI service ctrl msg close: %d", MSG_ID_CFW_CLOSE_SERVICE_RSP);
        ui_handle_close_service(msg);
        break;
    case MSG_ID_CFW_REGISTER_EVT_RSP:
        pr_debug(LOG_MODULE_UI_SVC, "UI service ctrl msg register: %d", MSG_ID_CFW_REGISTER_EVT_RSP);
        ui_handle_events_registration(msg);
        break;
    default:
        pr_debug(LOG_MODULE_UI_SVC, "UI service ctrl msg default");
        break;
    }
    cfw_msg_free(msg);
}

/**@brief Function to handle event(s) requests concerning other services.
 *
 * @details The function verifies if the other service has already been
 *          registered to the framework, and then executes the process for
 *          opening it.
 * @param[in]  svc_id   Concerned service.
 * @return   UI_SUCCESS on success, else error code.
 */
int8_t ui_handle_evt_req_for_service(uint8_t svc_id)
{
    int8_t ret = UI_SUCCESS;
    cfw_client_t *c;

    /* Here we assume that the service to be opened has already */
    /* been registered.                                         */
    pr_debug(LOG_MODULE_UI_SVC, "register new handle for service test %d", svc_id);
    /* Init cfw. */
    c = cfw_client_init(ui_svc_queue_msg, ui_handle_service_message, NULL);
    if (c == NULL) panic(PANIC_FAIL_1);

    /* Check if service was already opened by the UI service. */
    if (ui_is_client_service_opened(svc_id) <= 0) {
        /* Keep concerned service id for next steps. */
        ui_svc_pending_open = svc_id;
        pr_debug(LOG_MODULE_UI_SVC, "open req for service test %d", svc_id);
        if (cfw_open_service_conn(c,
                      svc_id,
                      ui_events_req.msg_req->priv)) {
            pr_debug(LOG_MODULE_UI_SVC,
                    "Error: cfw open service id %d failed",
                    svc_id);
            ret = UI_OPENSVC_FAIL;
        }
    } else {
        pr_debug(LOG_MODULE_UI_SVC,
                "Error: service id %d already opened and configured",
                svc_id);
        ret = UI_ERROR;
    }

    return ret;
}


/**@brief Function to handle event(s) requests concerning a driver.
 *
 * @details The function transmits to the driver the event identifier and
 *          the action to be done for this event.
 * @param[in] drv_id Identifier of concerned driver.
 * @param[in]  evt     Event identifier.
 * @param[in]  enabled Indicates if corresponding feature must be enabled
 *                     or disabled by the driver.
 * @return   UI_SUCCESS on success, else error code.
 */
int8_t ui_handle_evt_req_for_driver(uint16_t drv_id, uint32_t evt, uint32_t enabled)
{
    int8_t ret = UI_SUCCESS;

    pr_debug(LOG_MODULE_UI_SVC,
        "UI service: evt_id:0x%x set:0x%x for drv:0x%x",
        evt,
        enabled,
        drv_id);

    switch (drv_id & ~(UI_DRV_DEVICE_MASK)) {
#ifdef CONFIG_HAS_TOUCH
    case UIDRV_TOUCH_ID:
        /* Current touch driver always return single status for passed masks. */
        /* So if success, then callback is called with same passed values.    */
        /* To be modified in the future if differentiation is made betwwen    */
        /* passed events indexes in masks.                                    */
        if (evt == UI_TOUCH_EVT_IDX)
            ret = td_set_touch_events(MOTION_GEST_REGISTERED, enabled);
        else if (evt == UI_TAP_EVT_IDX)
            ret = td_set_touch_events(TAP_GEST_REGISTERED, enabled);

        if (ret == SUCCESS) {
            ui_handle_events_callback(evt, enabled);
            ret = UI_SUCCESS;
        } else {
            pr_debug(LOG_MODULE_UI_SVC,
                    "ERROR: could not set events for TOUCH DRV");
            ret = UI_EVTREQ_FAIL;
        }
        break;
#endif
#ifdef CONFIG_UI_SERVICE_IMPL_BUTTON
    case UIDRV_BTN_BASE: {
        int btn_id = drv_id & UI_DRV_DEVICE_MASK;
        uint8_t press_mask = 0;

        if (btn_id >= ui_config->btn_count) {
            pr_debug(LOG_MODULE_UI_SVC,
                    "ERROR: could not set events for BTN DRV");
            ret = UI_EVTREQ_FAIL;
            break;
        }
        if (evt & UI_BTN_SINGLE_EVT_IDX)
            press_mask |= SINGLE_PRESS;
        if (evt & UI_BTN_DOUBLE_EVT_IDX)
            press_mask |= DOUBLE_PRESS;
        if (evt & UI_BTN_MAX_EVT_IDX)
            press_mask |= MAX_PRESS;
#ifdef CONFIG_BUTTON_MULTIPLE_PRESS
        if (evt & UI_BTN_MULTIPLE_EVT_IDX)
            press_mask |= MULTIPLE_PRESS;
#endif

        button_set_press_mask(btn_id, press_mask);
        ui_handle_events_callback(evt, enabled);
        ret = UI_SUCCESS;
        break;
        }
#endif
    default:
#ifdef TEST_UI_SVC
        /* Return result directly. Just for testing path.     */
        /* This line will removed once interface with drivers */
        /* will be more complete.                             */
        ui_handle_events_callback(evt, enabled);
#endif
        break;
    }

    return ret;
}
