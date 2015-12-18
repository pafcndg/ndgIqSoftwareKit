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

#include "service_tests.h"
#include "services/gpio_service/gpio_service.h"
#include "scss_registers.h"

#if defined (CONFIG_BOARD_CURIE_APP)
// Pin definition for ss GPIO service test
#define TST_SS_OUTPUT_PIN 10
#define TST_SS_INPUT_PIN  11
#define MUX_SS_OUTPUT_PIN 63
#define MUX_SS_INPUT_PIN  64

// Pin definition for soc GPIO service test
#define TST_SOC_OUTPUT_PIN 5
#define TST_SOC_INPUT_PIN  6
#define MUX_SOC_OUTPUT_PIN 5
#define MUX_SOC_INPUT_PIN  6

// Pin definition for AON GPIO service test
#define TST_AON_OUTPUT_PIN 5
#define TST_AON_INPUT_PIN  4
#endif

#if defined (CONFIG_BOARD_CURIE_CTB)
// Pin definition for ss GPIO service test
#define TST_SS_OUTPUT_PIN 10
#define TST_SS_INPUT_PIN  11
#define MUX_SS_OUTPUT_PIN 63
#define MUX_SS_INPUT_PIN  64

// Pin definition for soc GPIO service test
#define TST_SOC_OUTPUT_PIN 13
#define TST_SOC_INPUT_PIN  12
#define MUX_SOC_OUTPUT_PIN 47
#define MUX_SOC_INPUT_PIN  46

// Pin definition for AON GPIO service test
#define TST_AON_OUTPUT_PIN 2
#define TST_AON_INPUT_PIN  1
#endif

#define TEST_GPIO(action, err, ...) \
    tst_gpio_ ## action = 0; \
    gpio_ ## action(__VA_ARGS__); \
    SRV_WAIT((tst_gpio_ ## action != 1), 100); \
    CU_ASSERT(err, (tst_gpio_ ## action) == 1); \
    if(tst_gpio_ ## action != 1) { \
        return DRV_RC_FAIL; \
    }

static int tst_gpio_configure = 0;
static int tst_gpio_set_state = 0;
static int tst_gpio_get_state = 0;
static int tst_gpio_get_value = 0;
static int tst_gpio_listen = 0;
static int tst_gpio_unlisten = 0;
static int tst_gpio_event = 0;

void generic_gpio_srv_test(uint8_t service_id, uint8_t inputPin, uint8_t outputPin, char* portLabel);

/**
 * \fn void gpio_tst_handle_msg(struct cfw_message * msg, void * data)
 *
 * \brief Callback function for gpio service test
 */
void gpio_tst_handle_msg(struct cfw_message * msg, void * data)
{
    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_CFW_OPEN_SERVICE_RSP:
        *((cfw_service_conn_t **)(msg->priv))  = (cfw_service_conn_t *)((cfw_open_conn_rsp_msg_t*)msg)->service_conn;
        cu_print("TST_GPIO open\n");
        break;
    case MSG_ID_GPIO_CONFIGURE_RSP:
        cu_print("TST_GPIO %s state=%d\n", msg->priv, ((gpio_configure_rsp_msg_t*)msg)->rsp_header.status);
        tst_gpio_configure = 1;
        break;
    case MSG_ID_GPIO_SET_RSP:
        tst_gpio_set_state = 1;
        cu_print("TST_GPIO %s: state=%d\n", msg->priv, ((gpio_set_rsp_msg_t*)msg)->rsp_header.status);
        break;
    case MSG_ID_GPIO_GET_RSP:
        tst_gpio_get_state = 1;
        tst_gpio_get_value = ((gpio_get_rsp_msg_t*)msg)->state;
        cu_print("TST_GPIO %s: state=%d, pin=0x%x\n", msg->priv,
                                            ((gpio_set_rsp_msg_t*)msg)->rsp_header.status,
                                            ((gpio_get_rsp_msg_t*)msg)->state);
        break;
    case MSG_ID_GPIO_LISTEN_RSP:
        tst_gpio_listen = 1;
        cu_print("TST_GPIO %s: state=%d, pin=%d\n", msg->priv,((gpio_listen_rsp_msg_t*)msg)->rsp_header.status,
                ((gpio_listen_rsp_msg_t*)msg)->index);
        break;
    case MSG_ID_GPIO_UNLISTEN_RSP:
        tst_gpio_unlisten = 1;
        cu_print("TST_GPIO %s: state=%d, pin=%d\n", msg->priv,((gpio_unlisten_rsp_msg_t*)msg)->rsp_header.status,
                ((gpio_listen_rsp_msg_t*)msg)->index);
        break;
    case MSG_ID_GPIO_EVT:
        tst_gpio_event = 1;
        cu_print("TST_GPIO EVT %s: state=%d, pin=%d\n", msg->priv,((gpio_listen_evt_msg_t*)msg)->pin_state,
                ((gpio_listen_evt_msg_t*)msg)->index);
        break;
    default:
        cu_print("default cfw handler\n");
        break;
    }
    cfw_msg_free(msg);
}

DRIVER_API_RC tst_gpio_srv_test_double_edge(cfw_client_t *gpio_client, unsigned int service_id, unsigned int input_pin, unsigned int output_pin)
{
    cfw_service_conn_t *gpio_service_conn = NULL;

    if (!cfw_service_registered(service_id)) {
        cu_print("soc gpio register failed\n");
        return DRV_RC_FAIL;
    }

    cfw_open_service_conn(gpio_client, service_id, &gpio_service_conn);
    SRV_WAIT((!gpio_service_conn), 1000);
    CU_ASSERT("TST_GPIO open service failed", (gpio_service_conn));
    if(gpio_service_conn == NULL) {
        return DRV_RC_FAIL;
    }

    TEST_GPIO(configure, "TST_GPIO config failed", gpio_service_conn, output_pin, 1, "cfg_output double edge");

    TEST_GPIO(set_state, "TST_GPIO set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1 de");
    TEST_GPIO(listen, "TST_GPIO listen failed (re)", gpio_service_conn, input_pin, BOTH_EDGE, DEB_OFF, "listen de");

    tst_gpio_event = 0;
    // ********************************
    // *      Test falling edge       *
    // ********************************
    TEST_GPIO(set_state, "TST_GPIO set 2 failed (de)", gpio_service_conn, output_pin, 0, "set_output 0 de");
    // Check event
    CU_ASSERT("TST_GPIO DE irq not detected", (tst_gpio_event == 1));
    if(tst_gpio_event != 1) {
        return DRV_RC_FAIL;
    }
    // Check GPIO port value
    TEST_GPIO(get_state, "TST_GPIO get 1 failed (de)", gpio_service_conn, "get_output 1 de");
    CU_ASSERT("TST_GPIO DE wrong port state detected (should be LOW)", (!(tst_gpio_get_value & (1<<input_pin))));
    if(tst_gpio_get_value & (1<<input_pin)) {
        return DRV_RC_FAIL;
    }

    tst_gpio_event = 0;
    // ********************************
    // *      Test rising edge        *
    // ********************************
    TEST_GPIO(set_state, "TST_GPIO set 3 failed (de)", gpio_service_conn, output_pin, 1, "set_output 1 de");
    // Check event
    CU_ASSERT("TST_GPIO DE irq not detected", (tst_gpio_event == 1));
    if(tst_gpio_event != 1) {
        return DRV_RC_FAIL;
    }
    // Check GPIO port value
    TEST_GPIO(get_state, "TST_GPIO get 2 failed (de)", gpio_service_conn, "get_output 2 de");
    CU_ASSERT("TST_GPIO DE wrong port state detected (should be HIGH)", (tst_gpio_get_value & (1<<input_pin)));
    if(!(tst_gpio_get_value & (1<<input_pin))) {
        return DRV_RC_FAIL;
    }

    TEST_GPIO(unlisten, "TST_GPIO unlisten failed (de)", gpio_service_conn, input_pin, "unlisten de");

    // Deconfigure output pin
    TEST_GPIO(set_state, "TST_GPIO set deconfig failed", gpio_service_conn, output_pin, 0, "set_output 0");
    TEST_GPIO(configure, "TST_GPIO end config failed", gpio_service_conn, output_pin, 0, "cfg_input double edge");

    cu_print("GPIO double edge test done\n");
    return DRV_RC_OK;
}

DRIVER_API_RC tst_gpio_srv_test_single_edge(cfw_client_t *gpio_client, unsigned int service_id, unsigned int input_pin, unsigned int output_pin)
{
    cfw_service_conn_t *gpio_service_conn = NULL;

    if (!cfw_service_registered(service_id)) {
        cu_print("soc gpio register failed\n");
        return DRV_RC_FAIL;
    }

    cfw_open_service_conn(gpio_client, service_id, &gpio_service_conn);
    SRV_WAIT((!gpio_service_conn), 1000);
    CU_ASSERT("TST_GPIO open service failed", (gpio_service_conn));
    if(gpio_service_conn == NULL) {
        return DRV_RC_FAIL;
    }

    TEST_GPIO(configure, "TST_GPIO config failed", gpio_service_conn, output_pin, 1, "cfg_output single edge");

    // ********************************
    // *      Test rising edge        *
    // ********************************
    TEST_GPIO(set_state, "TST_GPIO set 1 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1 re");
    TEST_GPIO(listen, "TST_GPIO listen failed (re)", gpio_service_conn, input_pin, RISING_EDGE, DEB_OFF, "listen re");

    tst_gpio_event = 0;
    TEST_GPIO(set_state, "TST_GPIO set 2 failed (re)", gpio_service_conn, output_pin, 0, "set_output 0 re");
    // Check no event
    CU_ASSERT("TST_GPIO RE irq wrong detected", (tst_gpio_event == 0));
    if(tst_gpio_event) {
        return DRV_RC_FAIL;
    }
    // Check GPIO port value
    TEST_GPIO(get_state, "TST_GPIO get 1 failed (re)", gpio_service_conn, "get_output 1 re");
    CU_ASSERT("TST_GPIO RE wrong port state detected (should be LOW)", (!(tst_gpio_get_value & (1<<input_pin))));
    if(tst_gpio_get_value & (1<<input_pin)) {
        return DRV_RC_FAIL;
    }

    TEST_GPIO(set_state, "TST_GPIO set 3 failed (re)", gpio_service_conn, output_pin, 1, "set_output 1 re");
    // Check event
    CU_ASSERT("TST_GPIO RE irq RE not detected", (tst_gpio_event == 1));
    if(tst_gpio_event != 1) {
        return DRV_RC_FAIL;
    }
    // Check GPIO port value
    TEST_GPIO(get_state, "TST_GPIO get 2 failed (re)", gpio_service_conn, "get_output 2 re");
    CU_ASSERT("TST_GPIO RE wrong port state detected (should be HIGH)", (tst_gpio_get_value & (1<<input_pin)));
    if(!(tst_gpio_get_value & (1<<input_pin))) {
        return DRV_RC_FAIL;
    }

    TEST_GPIO(unlisten, "TST_GPIO unlisten failed (re)", gpio_service_conn, input_pin, "unlisten re");

    // ********************************
    // *      Test falling edge       *
    // ********************************
    TEST_GPIO(set_state, "TST_GPIO set 1 failed (fe)", gpio_service_conn, output_pin, 0, "set_output 1 fe");
    TEST_GPIO(listen, "TST_GPIO listen failed (fe)", gpio_service_conn, input_pin, FALLING_EDGE, DEB_OFF, "listen fe");
    tst_gpio_event = 0;
    // Check no event
    TEST_GPIO(set_state, "TST_GPIO set 2 failed (fe)", gpio_service_conn, output_pin, 1, "set_output 0 fe");
    CU_ASSERT("TST_GPIO FE irq wrong detected", (tst_gpio_event == 0));
    if(tst_gpio_event) {
        return DRV_RC_FAIL;
    }
    // Check GPIO port value
    TEST_GPIO(get_state, "TST_GPIO get 1 failed (fe)", gpio_service_conn, "get_output 1 fe");
    CU_ASSERT("TST_GPIO FE wrong port state detected (should be HIGH)", (tst_gpio_get_value & (1<<input_pin)));
    if(!(tst_gpio_get_value & (1<<input_pin))) {
        return DRV_RC_FAIL;
    }

    // Check event
    TEST_GPIO(set_state, "TST_GPIO set 3 failed (fe)", gpio_service_conn, output_pin, 0, "set_output 1 fe");
    CU_ASSERT("TST_GPIO FE irq not detected", (tst_gpio_event == 1));
    if(tst_gpio_event != 1) {
        return DRV_RC_FAIL;
    }
    // Check GPIO port value
    TEST_GPIO(get_state, "TST_GPIO get 2 failed (fe)", gpio_service_conn, "get_output 2 fe");
    CU_ASSERT("TST_GPIO FE wrong port state detected (should be LOW)", (!(tst_gpio_get_value & (1<<input_pin))));
    if(tst_gpio_get_value & (1<<input_pin)) {
        return DRV_RC_FAIL;
    }

    TEST_GPIO(unlisten, "TST_GPIO unlisten failed (fe)", gpio_service_conn, input_pin, "unlisten fe");

    // Deconfigure output pin
    TEST_GPIO(set_state, "TST_GPIO set deconfig failed", gpio_service_conn, output_pin, 0, "set_output 0");
    TEST_GPIO(configure, "TST_GPIO end config failed", gpio_service_conn, output_pin, 0, "cfg_input single edge");

    cu_print("GPIO single edge test done\n");
    return DRV_RC_OK;
}

void soc_gpio_srv_test(void)
{
    uint8_t init_tst_input_pin_mode  = GET_PIN_MODE(MUX_SOC_INPUT_PIN);
    uint8_t init_tst_output_pin_mode = GET_PIN_MODE(MUX_SOC_OUTPUT_PIN);

    SET_PIN_MODE(MUX_SOC_INPUT_PIN, QRK_PMUX_SEL_MODEA);
    SET_PIN_MODE(MUX_SOC_OUTPUT_PIN, QRK_PMUX_SEL_MODEA);

    generic_gpio_srv_test(SOC_GPIO_SERVICE_ID, TST_SOC_INPUT_PIN, TST_SOC_OUTPUT_PIN, "SOC");

        /* Restore default configuration for SOC pins */
    SET_PIN_MODE(MUX_SOC_INPUT_PIN, init_tst_input_pin_mode);
    SET_PIN_MODE(MUX_SOC_OUTPUT_PIN, init_tst_output_pin_mode);
}

void ss_gpio_srv_test(void)
{
    uint8_t init_tst_input_pin_mode  = GET_PIN_MODE(MUX_SS_INPUT_PIN);
    uint8_t init_tst_output_pin_mode = GET_PIN_MODE(MUX_SS_OUTPUT_PIN);

    SET_PIN_MODE(MUX_SS_INPUT_PIN, QRK_PMUX_SEL_MODEA);
    SET_PIN_MODE(MUX_SS_OUTPUT_PIN, QRK_PMUX_SEL_MODEA);

    generic_gpio_srv_test(SS_GPIO_SERVICE_ID, TST_SS_INPUT_PIN, TST_SS_OUTPUT_PIN, "SS");

        /* Restore default configuration for SS pins */
    SET_PIN_MODE(MUX_SS_INPUT_PIN, init_tst_input_pin_mode);
    SET_PIN_MODE(MUX_SS_OUTPUT_PIN, init_tst_output_pin_mode);
}

void aon_gpio_srv_test(void)
{
    generic_gpio_srv_test(AON_GPIO_SERVICE_ID, TST_AON_INPUT_PIN, TST_AON_OUTPUT_PIN, "AON");
}

void generic_gpio_srv_test(uint8_t service_id, uint8_t inputPin, uint8_t outputPin, char* portLabel)
{
    DRIVER_API_RC ret;
    char* outputString = balloc(55, NULL) ;
    char* border = "##################################################\n";
    cfw_client_t * gpio_client = cfw_client_init(get_test_queue(), gpio_tst_handle_msg, NULL);

    ret = tst_gpio_srv_test_single_edge(gpio_client, service_id, inputPin, outputPin);
    sprintf(outputString, "Test for %s gpio single edge", portLabel);
    CU_ASSERT(outputString , ret == DRV_RC_OK);
    if(ret == DRV_RC_OK)
    {
        cu_print("%s", border);
        cu_print("#       Test GPIO %s gpio single edge OK        #\n", portLabel);
        cu_print("%s", border);
    }

    ret = tst_gpio_srv_test_double_edge(gpio_client, service_id, inputPin, outputPin);
    sprintf(outputString, "Test for %s gpio double edge", portLabel);
    CU_ASSERT(outputString, ret == DRV_RC_OK);
    if(ret == DRV_RC_OK)
    {
        cu_print("%s", border);
        cu_print("#       Test GPIO %s gpio double edge OK         #\n", portLabel);
        cu_print("%s", border);
    }
}
