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

#include "util/workqueue.h"
#include "drivers/ss_gpio_iface.h"
#include "drivers/soc_gpio.h"
#include "drivers/sensor/bmi160_bus.h"
#include "drivers/sensor/bmi160_support.h"
#include "drivers/sensor/bmi160_gpio.h"

#ifdef CONFIG_BMI160_PM
static uint8_t gyro_powermode;
#if BMI160_ENABLE_MAG
static uint8_t mag_powermode;
#endif
#endif

static uint8_t fifo_full_en;

#if BMI160_ENABLE_DOUBLE_TAPPING
uint32_t bmi160_any_motion_timestamp = 0;
uint8_t bmi160_wait_double_tap = 0;
#else
uint8_t bmi160_any_motion_detected = 0;
#endif

uint8_t bmi160_need_update_wm_interrupt = 0;

static void handle_sensor_reg(struct bmi160_rt_t *bmi160_rt, uint8_t sensor_type)
{
    uint32_t buffer[3]; /** gyro/mag need 12 bytes, accle need 6 bytes */
    uint8_t sensor_int_en_mask = (1<<sensor_type);
    uint8_t data_size = BMI160_MAG_FRAME_SIZE;

    if (sensor_type >= BMI160_SENSOR_COUNT) { /** current sensor count is 2 */
        pr_debug(LOG_MODULE_BMI160, "Invalid sensor type: %u", sensor_type);
        return;
    }
    if(sensor_type==BMI160_SENSOR_ACCEL)
        data_size = BMI160_ACCEL_FRAME_SIZE;
    else if(sensor_type==BMI160_SENSOR_GYRO)
        data_size = BMI160_GYRO_FRAME_SIZE;

    if ((bmi160_rt->int_en[BMI160_OFFSET_DRDY] & sensor_int_en_mask))
        bmi160_rt->reg_data_read_funs[sensor_type]((uint8_t*)buffer, data_size);
    if (_Usually(bmi160_rt->read_callbacks[sensor_type])) {
            bmi160_rt->read_callbacks[sensor_type]
                    ((uint8_t *) buffer, data_size,
                     bmi160_rt->cb_priv_data[sensor_type]);
    }
}

static void handle_reg_data(struct bmi160_rt_t *bmi160_rt)
{
    uint8_t user_status = 0;

    if(!bmi160_rt->int_en[BMI160_OFFSET_DRDY])  /* check if drdy is enabled */
        return;

    if(bmi160_rt->motion_callback && !bmi160_rt->motion_state)
        return;

    if(bmi160_read_reg(BMI160_USER_STAT_ADDR, &user_status))
        return;

    if(!bmi160_rt->fifo_en[BMI160_SENSOR_ACCEL] &&
        BMI160_GET_BITSLICE(user_status, BMI160_USER_STAT_DATA_RDY_ACCEL))
        handle_sensor_reg(bmi160_rt, BMI160_SENSOR_ACCEL);

    if(!bmi160_rt->fifo_en[BMI160_SENSOR_GYRO] &&
        BMI160_GET_BITSLICE(user_status, BMI160_USER_STAT_DATA_RDY_GYRO))
        handle_sensor_reg(bmi160_rt, BMI160_SENSOR_GYRO);

#if BMI160_ENABLE_MAG
    if(!bmi160_rt->fifo_en[BMI160_SENSOR_MAG] &&
        BMI160_GET_BITSLICE(user_status, BMI160_USER_STAT_DATA_RDY_MAG))
        handle_sensor_reg(bmi160_rt, BMI160_SENSOR_MAG);
#endif
}

#if BMI160_SUPPORT_FIFO_INT_DATA_REPORT
static void report_fifo_data(uint8_t sensor_type, uint8_t *buffer, uint16_t frame_cnt, uint8_t frame_size)
{
    uint8_t sensor_int_en_mask = (1<<sensor_type);
    struct bmi160_rt_t *bmi160_rt = bmi160_get_ptr();

    if (bmi160_rt->int_en[BMI160_OFFSET_FIFO_WM] & sensor_int_en_mask) {
        if (_Usually(bmi160_rt->read_callbacks[sensor_type])) {
            bmi160_rt->read_callbacks[sensor_type]
                    ((uint8_t *)buffer, frame_cnt * frame_size,
                     bmi160_rt->cb_priv_data[sensor_type]);

        }
    }
}
#endif

static void handle_fifo(struct bmi160_rt_t *bmi160_rt, uint8_t status)
{
    uint8_t fifo_en =
#if BMI160_ENABLE_MAG
                      bmi160_rt->fifo_en[BMI160_SENSOR_MAG]   |
#endif
                      bmi160_rt->fifo_en[BMI160_SENSOR_ACCEL] |
                      bmi160_rt->fifo_en[BMI160_SENSOR_GYRO];

    if(!fifo_en) return;

    uint8_t fifo_wm = 0;
    if (bmi160_rt->int_en[BMI160_OFFSET_FIFO_WM])
        fifo_wm = BMI160_GET_BITSLICE(status, BMI160_USER_INTR_STAT_1_FIFO_WM_INTR);

    if (fifo_wm) {
#if BMI160_SUPPORT_FIFO_INT_DATA_REPORT
        bmi160_read_fifo_header_data(FIFO_FRAME);
        report_fifo_data(BMI160_SENSOR_ACCEL, (uint8_t *)bmi160_accel_fifo,
                         bmi160_accel_index, sizeof(phy_accel_data_t));
        report_fifo_data(BMI160_SENSOR_GYRO, (uint8_t *)bmi160_gyro_fifo,
                         bmi160_gyro_index, sizeof(phy_gyro_data_t));
#if BMI160_ENABLE_MAG
        report_fifo_data(BMI160_SENSOR_MAG, (uint8_t *)bmi160_mag_fifo,
                         bmi160_mag_index, sizeof(phy_mag_data_t));
#endif
#else
        /*Just report the watermark event*/
        for(int i=0; i<BMI160_SENSOR_COUNT; i++){
            if(bmi160_rt->wm_callback[i] && bmi160_rt->fifo_en[i]){
                phy_sensor_event_t wm_event;
                wm_event.event_type = PHY_SENSOR_EVENT_WM;
                wm_event.data = NULL;
                bmi160_rt->wm_callback[i](&wm_event, bmi160_rt->wm_cb_priv_data[i]);
                break;
            }
        }
        bmi160_int_disable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_WM, 0);
        bmi160_need_update_wm_interrupt = 1;
#endif
    }

    if (bmi160_rt->int_en[BMI160_OFFSET_FIFO_FULL] &&
        BMI160_GET_BITSLICE(status, BMI160_USER_INTR_STAT_1_FIFO_FULL_INTR)){
            bmi160_int_disable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_FULL, 0);
            if(bmi160_rt->motion_state)
                pr_warning(LOG_MODULE_BMI160, "FIFO full");
    }
}

#ifdef CONFIG_BMI160_PM
static void handle_motion_detection(struct bmi160_rt_t *bmi160_rt, uint8_t status0, uint8_t status1)
{
    uint8_t any_motion = 0;
    uint8_t no_motion = 0;
    uint8_t event = 0;
    static uint32_t no_motion_timestamp = 0;
    static uint8_t need_init = 1;

    if(_Rarely(need_init)){
        need_init = 0;
        fifo_full_en = bmi160_rt->int_en[BMI160_OFFSET_FIFO_FULL];
        gyro_powermode = bmi160_rt->power_status[BMI160_SENSOR_GYRO];
#if BMI160_ENABLE_MAG
        mag_powermode = bmi160_rt->power_status[BMI160_SENSOR_MAG];
#endif
    }

#if BMI160_ENABLE_DOUBLE_TAPPING
    uint8_t double_tap = 0;
    static uint32_t double_tap_timestamp = 0;
    if(bmi160_rt->int_en[BMI160_OFFSET_DOUBLE_TAP]){
        double_tap = BMI160_GET_BITSLICE(status0, BMI160_USER_INTR_STAT_0_DOUBLE_TAP_INTR);
        if(double_tap){
            double_tap_timestamp = get_uptime_ms();
            bmi160_int_disable(BMI160_INT_SET_0, BMI160_OFFSET_DOUBLE_TAP, 0);
            if(double_tap_timestamp-bmi160_any_motion_timestamp<=600){
                event = PHY_SENSOR_EVENT_DOUBLE_TAP;
                pr_debug(LOG_MODULE_BMI160, "Double tap");
                bmi160_rt->motion_callback(&event, 1, bmi160_rt->motion_cb_data);
            }
#if DEBUG_BMI160
            else {
                pr_debug(LOG_MODULE_BMI160, "Ignore double tap");
            }
#endif
        }
    }
#endif

    if(bmi160_rt->int_en[BMI160_OFFSET_ANYMOTION]){
        any_motion = BMI160_GET_BITSLICE(status0, BMI160_USER_INTR_STAT_0_ANY_MOTION);
        if(any_motion){
#if BMI160_ENABLE_DOUBLE_TAPPING
            bmi160_any_motion_timestamp = get_uptime_ms();
            bmi160_wait_double_tap = 1;

            /* Flush fifo to avoid unneccessary fifo full interrupt
             * when any_motion detected
             */
            bmi160_flush_fifo();
            bmi160_int_enable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_FULL, fifo_full_en);
#else
            bmi160_any_motion_detected = 1;
            bmi16_set_user_sensor_config(BMI160_SENSOR_ACCEL, BMI160_POWER_LOWPOWER, 1, bmi160_rt->accel_config);
#endif
            bmi160_rt->motion_state = 1;
            bmi160_int_disable(BMI160_INT_SET_0, BMI160_OFFSET_ANYMOTION, 0);
            bmi160_int_enable(BMI160_INT_SET_2, BMI160_OFFSET_NOMOTION, (1<<BMI160_SENSOR_MOTION));

            /* restore gyro power mode */
            bmi160_change_sensor_powermode(BMI160_SENSOR_GYRO, gyro_powermode);
#if BMI160_ENABLE_MAG
            if(bmi160_rt->change_mag_powermode)
                bmi160_rt->change_mag_powermode(mag_powermode);
#endif
            event = PHY_SENSOR_EVENT_ANY_MOTION;
            bmi160_rt->motion_callback(&event, 1, bmi160_rt->motion_cb_data);
        }
    }

    if(bmi160_rt->int_en[BMI160_OFFSET_NOMOTION]){
        no_motion = BMI160_GET_BITSLICE(status1, BMI160_USER_INTR_STAT_1_NOMOTION_INTR);
        if(no_motion){
            no_motion_timestamp = get_uptime_ms();
            bmi160_int_disable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_FULL, 0);
            bmi160_int_disable(BMI160_INT_SET_2, BMI160_OFFSET_NOMOTION, 0);
            bmi160_int_enable(BMI160_INT_SET_0, BMI160_OFFSET_ANYMOTION, (1<<BMI160_SENSOR_MOTION));
#if BMI160_ENABLE_DOUBLE_TAPPING
            bmi160_int_enable(BMI160_INT_SET_0, BMI160_OFFSET_DOUBLE_TAP, (1<<BMI160_SENSOR_MOTION));
#endif

            bmi160_rt->motion_state = 0;

            event = PHY_SENSOR_EVENT_NO_MOTION;
            bmi160_rt->motion_callback(&event, 1, bmi160_rt->motion_cb_data);

            /* 100HZ, AVG=1 */
            uint8_t accel_config_nomotion = BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ;
            accel_config_nomotion |= ((CONFIG_BMI160_LOWPOWER_AVG_NOMOTION)<<4);
            bmi16_set_user_sensor_config(BMI160_SENSOR_ACCEL, BMI160_POWER_LOWPOWER, 1, accel_config_nomotion);

            /* suspend gyro */
            gyro_powermode = bmi160_rt->power_status[BMI160_SENSOR_GYRO];
            bmi160_change_sensor_powermode(BMI160_SENSOR_GYRO, BMI160_POWER_SUSPEND);
#if BMI160_ENABLE_MAG
            /* suspend gyro */
            mag_powermode = bmi160_rt->power_status[BMI160_SENSOR_MAG];
            if(bmi160_rt->change_mag_powermode)
                bmi160_rt->change_mag_powermode(BMI160_POWER_SUSPEND);
#endif
        }
    }
}
#endif

static void bmi160_gpio_generic_callback(struct bmi160_rt_t *bmi160_rt, uint8_t pin)
{
    uint8_t status0 = 0;
    uint8_t status1 = 0;
    uint8_t status0_new = 0;
    uint8_t status1_new = 0;
    uint32_t timestamp;
    uint8_t retry;
    int timeout_ms = 0;

    timestamp = get_uptime_ms();

    handle_reg_data(bmi160_rt);

    if(bmi160_rt->motion_callback && !bmi160_rt->motion_state)
        timeout_ms = 10;

    do{
        if(bmi160_read_reg(BMI160_USER_INTR_STAT_0_ADDR, &status0))
            goto OUT;
        if(bmi160_read_reg(BMI160_USER_INTR_STAT_1_ADDR, &status1))
            goto OUT;
    } while(get_uptime_ms()-timestamp < timeout_ms && !status0 && !status1);

RETRY:

#ifdef CONFIG_BMI160_PM
    if(bmi160_rt->motion_callback)
        handle_motion_detection(bmi160_rt, status0, status1);
#endif

    handle_fifo(bmi160_rt, status1);

    retry = 0;
    bmi160_read_reg(BMI160_USER_INTR_STAT_0_ADDR, &status0_new);
    bmi160_read_reg(BMI160_USER_INTR_STAT_1_ADDR, &status1_new);
    if(status0_new && status0_new!=status0)
        retry = 1;

    if(status1_new && status1_new!=status1)
        retry = 1;

    status0 = status0_new;
    status1 = status1_new;

    handle_reg_data(bmi160_rt);

    if(retry)
        goto RETRY;

OUT:
    bmi160_unmask_int_pin(pin);
}

#if BMI160_USE_INT_PIN1
static void bmi160_gpio_pin1_callback(void *arg)
{
    bmi160_gpio_generic_callback((struct bmi160_rt_t *)arg, BMI160_INT_PIN1);
}
/* BMI160 INT1 ISR callback */
static void bmi160_pin1_isr(bool state, void *arg)
{
    /* Mask this interrup */
    bmi160_mask_int_pin(BMI160_INT_PIN1);
    workqueue_queue_work(bmi160_gpio_pin1_callback, arg);
}
#endif

#if BMI160_USE_INT_PIN2
static void bmi160_gpio_pin2_callback(void *arg)
{
    bmi160_gpio_generic_callback((struct bmi160_rt_t *)arg, BMI160_INT_PIN2);
}
/* BMI160 INT2 ISR callback */
static void bmi160_pin2_isr(bool state, void *arg)
{
    /* Mask this interrup */
    bmi160_mask_int_pin(BMI160_INT_PIN2);
    workqueue_queue_work(bmi160_gpio_pin2_callback, arg);
}
#endif

void bmi160_mask_int_pin(uint8_t pin)
{
    if (pin == BMI160_INT_PIN1){
        soc_gpio_mask_interrupt(&pf_device_soc_gpio_aon, BMI160_GPIN_AON_PIN);
    }
#if BMI160_USE_INT_PIN2
    else if (pin == BMI160_INT_PIN2)
        ss_gpio_mask_interrupt(&pf_device_ss_gpio_8b0, BMI160_GPIO_SS_PIN);
#endif
}

void bmi160_unmask_int_pin(uint8_t pin)
{
    if (pin == BMI160_INT_PIN1) {
        soc_gpio_unmask_interrupt(&pf_device_soc_gpio_aon, BMI160_GPIN_AON_PIN);
    }
#if BMI160_USE_INT_PIN2
    else if (pin == BMI160_INT_PIN2)
        ss_gpio_unmask_interrupt(&pf_device_ss_gpio_8b0, BMI160_GPIO_SS_PIN);
#endif
}

DRIVER_API_RC bmi160_config_gpio(void)
{
    struct device *dev;

    gpio_cfg_data_t cfg = { 0 };

    cfg.gpio_type = GPIO_INTERRUPT;
    cfg.int_type = EDGE;
    cfg.int_polarity = ACTIVE_HIGH;
    cfg.int_debounce = DEBOUNCE_ON;
    cfg.gpio_cb_arg = (void*)bmi160_get_ptr();

#if BMI160_USE_INT_PIN1
    cfg.gpio_cb = bmi160_pin1_isr;
    dev = &pf_device_soc_gpio_aon;
    if (soc_gpio_set_config(dev, BMI160_GPIN_AON_PIN, &cfg)) {
        pr_debug(LOG_MODULE_BMI160, "Config GPIO[%d-%d] failed",
                 SOC_GPIO_AON_ID, BMI160_GPIN_AON_PIN);
        return DRV_RC_FAIL;
    }
#endif

#if BMI160_USE_INT_PIN2
    /* GPIO_SS4 connected to interrupt PIN2 of BMI160 */
    cfg.gpio_cb = bmi160_pin2_isr;
    dev = &pf_device_ss_gpio_8b0;
    if (ss_gpio_set_config(dev, BMI160_GPIO_SS_PIN, &cfg)) {
        pr_debug(LOG_MODULE_BMI160, "Config GPIO[%d-%d] failed",
                 SOC_GPIO_AON_ID, BMI160_GPIN_AON_PIN);
        return DRV_RC_FAIL;
    }
#endif
    pr_debug(LOG_MODULE_BMI160, "GPIO config done");
    return DRV_RC_OK;
}
