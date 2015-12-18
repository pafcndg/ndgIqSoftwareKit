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

#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"

#include "infra/log.h"
#include "scss_registers.h"

#include "pwrbtn_service_test.h"
#include "service_tests.h"
#include "services/gpio_service/gpio_service.h"
#include "services/pwrbtn.h"

#define TEST_PWRBTN(action, err, ...) \
    tst_pwrbtn_ ## action = 0; \
    gpio_ ## action(__VA_ARGS__); \
    SRV_WAIT((tst_pwrbtn_ ## action != 1), 2000); \
    CU_ASSERT(err, (tst_pwrbtn_ ## action) == 1); \
    if(tst_pwrbtn_ ## action != 1) { \
        return DRV_RC_FAIL; \
    }

#define VERY_SH_PRESS      1
#define SH_PRESS           2
#define SINGLE_PRESS       3
#define L_PRESS            4
#define VERY_L_PRESS       5
#define DBLE_PRESS         6
#define DBLE_PRESS_FAIL_1  7
#define DBLE_PRESS_FAIL_2  8
#define TOO_SHORT_PRESS    9

/*
 * Define the pin where the button is connected.
 */
#define GPIO_PIN     0


static int tst_step = 0;
static int tst_pwrbtn_configure = 0;
static int tst_pwrbtn_set_state = 0;
static int tst_pwrbtn_get_state = 0;
static int tst_pwrbtn_get_value = 0;
static int tst_pwrbtn_listen = 0;
static int tst_pwrbtn_unlisten = 0;
static int tst_pwrbtn_event = 0;


static int tst_very_short = 0;
static int tst_short = 0;
static int tst_single = 0;
static int tst_long = 0;
static int tst_very_long = 0;
static int tst_double = 0;
static int tst_double_fail_1 = 0;
static int tst_double_fail_2 = 0;
static int tst_too_short = 0;

void pwrbtn_notify_callback(btn_press_pattern event, int duration);

#ifndef CONFIG_UI_SERVICE_IMPL
struct pwr_btn pwrbtn_aon_0_alone = {
    .pin = GPIO_PIN,
    .gpio_service_id = AON_GPIO_SERVICE_ID,
    .pwrbtn_cb = pwrbtn_notify_callback,
};
#endif

/**
 * Callback function for gpio service response
 */
void pwrbtn_tst_handle_msg(struct cfw_message * msg, void * data)
{
    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_CFW_OPEN_SERVICE_RSP:
        *((cfw_service_conn_t **)(msg->priv))  = (cfw_service_conn_t *)((cfw_open_conn_rsp_msg_t*)msg)->service_conn;
        cu_print("TST_PWRBTN service opened \n");
        break;
    case MSG_ID_GPIO_CONFIGURE_RSP:
        cu_print("TST_PWRBTN %s state=%d\n", msg->priv, ((gpio_configure_rsp_msg_t*)msg)->rsp_header.status);
        tst_pwrbtn_configure = 1;
        break;
    case MSG_ID_GPIO_SET_RSP:
        tst_pwrbtn_set_state = 1;
        cu_print("TST_PWRBTN %s: state=%d\n", msg->priv, ((gpio_set_rsp_msg_t*)msg)->rsp_header.status);
        break;
    case MSG_ID_GPIO_GET_RSP:
        tst_pwrbtn_get_state = 1;
        tst_pwrbtn_get_value = ((gpio_get_rsp_msg_t*)msg)->state;
        cu_print("TST_PWRBTN %s: state=%d, pin=0x%x\n", msg->priv,
                                            ((gpio_set_rsp_msg_t*)msg)->rsp_header.status,
                                            ((gpio_get_rsp_msg_t*)msg)->state);
        break;
    case MSG_ID_GPIO_LISTEN_RSP:
        tst_pwrbtn_listen = 1;
        cu_print("TST_PWRBTN %s: state=%d, pin=%d\n", msg->priv,((gpio_listen_rsp_msg_t*)msg)->rsp_header.status,
                ((gpio_listen_rsp_msg_t*)msg)->index);
        break;
    case MSG_ID_GPIO_UNLISTEN_RSP:
        tst_pwrbtn_unlisten = 1;
        cu_print("TST_PWRBTN %s: state=%d, pin=%d\n", msg->priv,((gpio_unlisten_rsp_msg_t*)msg)->rsp_header.status,
                ((gpio_listen_rsp_msg_t*)msg)->index);
        break;
    case MSG_ID_GPIO_EVT:
        tst_pwrbtn_event = 1;
        cu_print("TST_PWRBTN EVT %s: state=%d, pin=%d\n", msg->priv,((gpio_listen_evt_msg_t*)msg)->pin_state,
                ((gpio_listen_evt_msg_t*)msg)->index);
        break;
    default:
        cu_print("default cfw handler\n");
        break;
    }
    cfw_msg_free(msg);
}


int press_simulation(unsigned int service_id, unsigned int input_pin, unsigned int output_pin)
{
    cfw_service_conn_t *gpio_service_conn = NULL;

    cfw_client_t * gpio_client = cfw_client_init(get_test_queue(), pwrbtn_tst_handle_msg, NULL);
    if (!cfw_service_registered(service_id)) {
        cu_print("TST_PWRBTN: ss gpio service not registered\n");
        return DRV_RC_FAIL;
    }

    cfw_open_service_conn(gpio_client, service_id, &gpio_service_conn);
    SRV_WAIT((!gpio_service_conn), 2000);
    CU_ASSERT("TST_PWRBTN: can not open ss gpio service", (gpio_service_conn));
    if(gpio_service_conn == NULL) {
        return DRV_RC_FAIL;
    }

    /* The configuration put the pin to 0 => the pin have to be set to 1
    * before first press simulation */
    TEST_PWRBTN(configure, "TST_PWRBTN: gpio configuration failed", gpio_service_conn, output_pin, 1, "SS pin 2 output");
    SRV_WAIT((!tst_pwrbtn_configure), 2000);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");
    /* wait state response */
    SRV_WAIT((!tst_pwrbtn_set_state), 2000);

    /* wait 3s */
    local_task_sleep_ms(3000);

    tst_step = VERY_SH_PRESS;
    cu_print("Start pattern with press_duration = 700ms\n");
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(700);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait pattern detection */
    SRV_WAIT((!tst_very_short), 2000);
    /* wait 3s */
    local_task_sleep_ms(3000);

    cu_print("Start pattern with press_duration = 2500ms\n");
    tst_step = SH_PRESS;
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(2500);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait pattern detection */
    SRV_WAIT((!tst_short), 2000);
    /* wait 3s */
    local_task_sleep_ms(3000);

    cu_print("Start pattern with press_duration = 4000ms\n");
    tst_step = SINGLE_PRESS;
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(4000);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait pattern detection */
    SRV_WAIT((!tst_single), 2000);
    /* wait 3s */
    local_task_sleep_ms(3000);

    cu_print("Start pattern with press_duration = 7000ms\n");
    tst_step = L_PRESS;
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(7000);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait pattern detection */
    SRV_WAIT((!tst_long), 2000);
    /* wait 3s */
    local_task_sleep_ms(3000);

    cu_print("Start pattern with press_duration = 12000ms\n");
    tst_step = VERY_L_PRESS;
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(12000);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait 3s */
    local_task_sleep_ms(3000);

    /* Start failed double press pattern : very_short_press + 550 ms delay max + short_press */
    cu_print("Start double pattern press failed...\n");
    tst_step = DBLE_PRESS_FAIL_1;
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(700);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait pattern detection */
    SRV_WAIT((!tst_double_fail_1), 2000);
    /* wait between the very short press */
    local_task_sleep_ms(550);

    tst_step = DBLE_PRESS_FAIL_2;
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(2700);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait pattern detection */
    SRV_WAIT((!tst_double_fail_2), 2000);
    /* wait 3s */
    local_task_sleep_ms(3000);

    cu_print("Start too short pattern with duration = 300ms\n");
    tst_step = TOO_SHORT_PRESS;
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(300);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait pattern detection */
    SRV_WAIT((!tst_double_fail_2), 2000);
    /* wait 3s */
    local_task_sleep_ms(3000);

    /* Start double press pattern : very_short_press + 350 ms delay max + very_short_press */
    cu_print("Start double pattern press...\n");
    tst_step = DBLE_PRESS;
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(700);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait between the very short press */
    local_task_sleep_ms(350);

    TEST_PWRBTN(set_state, "TST_PWRBTN: set 0 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0");
    local_task_sleep_ms(900);
    TEST_PWRBTN(set_state, "TST_PWRBTN: set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1");

    /* wait pattern detection */
    SRV_WAIT((!tst_double), 2000);
    /* wait 2s */
    local_task_sleep_ms(2000);

    cfw_close_service_conn(gpio_service_conn, NULL);

    return DRV_RC_OK;
}


void pwrbtn_notify_callback(btn_press_pattern event, int duration)
{
    switch (tst_step){
    case VERY_SH_PRESS:
        CU_ASSERT("Bad event identification received", event == ONCE_PRESSED);
        CU_ASSERT("Duration is wrong", ((duration < 800) && (duration > 700)));
        tst_very_short = 1;
        cu_print("PATTERN SINGLE PRESS RECEIVED\n");
        break;
    case SH_PRESS:
        CU_ASSERT("Bad event identification received", event == ONCE_PRESSED);
        CU_ASSERT("Duration for SHORT PRESS is wrong", ((duration < 2600) && (duration > 2500)));
        tst_short = 1;
        cu_print("PATTERN SINGLE PRESS RECEIVED\n");
        break;
    case SINGLE_PRESS:
        CU_ASSERT("Bad event identification received", event == ONCE_PRESSED);
        CU_ASSERT("Duration is wrong", ((duration < 4100) && (duration > 4000)));
        tst_single = 1;
        cu_print("PATTERN SINGLE PRESS RECEIVED\n");
        break;
    case L_PRESS:
        CU_ASSERT("Bad event identification received", event == ONCE_PRESSED);
        CU_ASSERT("Duration is wrong", ((duration < 7100) && (duration > 7000)));
        tst_long = 1;
        cu_print("PATTERN SINGLE PRESS RECEIVED\n");
        break;
    case VERY_L_PRESS:
        /* Should not receive pattern with duration = 12000ms */
        tst_very_long = 1;
        break;
    case DBLE_PRESS:
        CU_ASSERT("Bad event identification received", event == TWICE_PRESSED);
        CU_ASSERT("Duration  is wrong", ((duration < 800) && (duration > 700)));
        tst_double = 1;
        cu_print("PATTERN DOUBLE PRESS RECEIVED\n");
        break;
    case DBLE_PRESS_FAIL_1:
        CU_ASSERT("Bad event identification received", event == ONCE_PRESSED);
        CU_ASSERT("Duration is wrong", ((duration < 800) && (duration > 700)));
        tst_double_fail_1 = 1;
        cu_print("PATTERN SINGLE PRESS RECEIVED\n");
        break;
    case DBLE_PRESS_FAIL_2:
        CU_ASSERT("Bad event identification received", event == ONCE_PRESSED);
        CU_ASSERT("Duration is wrong", ((duration < 2800) && (duration > 2700)));
        tst_double_fail_2 = 1;
        cu_print("PATTERN SINGLE PRESS RECEIVED\n");
        break;
    case TOO_SHORT_PRESS:
        /* Should not receive pattern with duration = 300ms */
        tst_too_short = 1;
        break;
    default:
        CU_ASSERT("Test step invalid", tst_step > 9);
    }
}


void pwrbtn_srv_test(void)
{
    DRIVER_API_RC ret;

#ifndef CONFIG_UI_SERVICE_IMPL
    pwrbtn_init(&pwrbtn_aon_0_alone, get_test_queue());
    /* Wait pwrbtn unitialisation */
    local_task_sleep_ms(1500);
#endif

    cu_print("##################################################\n");
    cu_print("#                                                #\n");
    cu_print("# !!! Pins AON %d and SS %d must be connected !  #\n", PWRBTN_TST_AON_INPUT_PIN, PWRBTN_TST_SS_OUTPUT_PIN);
    cu_print("#                                                #\n");
    cu_print("#  Purpose of service : Validate power button    #\n");
    cu_print("##################################################\n");

    ret = press_simulation(SS_GPIO_SERVICE_ID, PWRBTN_TST_AON_INPUT_PIN, PWRBTN_TST_SS_OUTPUT_PIN);

    CU_ASSERT("Test power button module failed", ret == DRV_RC_OK);
    CU_ASSERT("Press 700ms not notified", tst_very_short == 1);
    CU_ASSERT("Press 2500ms not notified", tst_short == 1);
    CU_ASSERT("Press 4000ms not notified", tst_single == 1);
    CU_ASSERT("Press 7000ms not notified", tst_long == 1);
    CU_ASSERT("Press 12000ms notified", tst_very_long == 0);
    CU_ASSERT("Double press not notified", tst_double == 1);
    CU_ASSERT("Press 700ms not notified", tst_double_fail_1 == 1);
    CU_ASSERT("Press 2700ms not notified", tst_double_fail_2 == 1);
    CU_ASSERT("Press 300ms notified", tst_too_short == 0);

    cu_print("##################################################\n");
    cu_print("#          Test module power button done           #\n");
    cu_print("##################################################\n");
}

