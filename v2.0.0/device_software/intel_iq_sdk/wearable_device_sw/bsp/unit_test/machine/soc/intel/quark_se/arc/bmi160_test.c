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

#include "drivers/eiaextensions.h"
#include "util/cunit_test.h"
#include "infra/time.h"

#include "drivers/sensor/bmi160_support.h"
#include "drivers/sensor/bmi160_gpio.h"
#include "drivers/sensor/bmi160_bus.h"

#define FIFO_BUF_LEN    512

#define TEST_MOTION_DETECT 0
#define TEST_SLEEP_WAKEUP 0
#define TEST_ACCEL_RANGE_SET 0
#define TEST_GYRO_RANGE_SET 0

#define TICKS_1S    1000
#define TICKS_2S    2000
#define TICKS_3S   3000

#define TEST_DATA_ODR_HZ 50
#define TEST_GYRO_DATA_ODR_HZ 50

#define TEST_REPORT_MODE_MASK     (PHY_SENSOR_REPORT_MODE_POLL_REG_MASK  |  \
                                   PHY_SENSOR_REPORT_MODE_INT_REG_MASK   |  \
                                   PHY_SENSOR_REPORT_MODE_POLL_FIFO_MASK |  \
                                   PHY_SENSOR_REPORT_MODE_INT_FIFO_MASK)

#define ARC_IRQ_PRRORITY 0x206
#define ARC_IRQ_SELECT 0x40b
#define  read_auxreg( reg )     \
    ({                          \
        unsigned int __ret;     \
        __asm__ __volatile__ (  \
        " lr    %0, [%1] "      \
        : "=r"(__ret)           \
        : "i" (reg));           \
        __ret;                  \
    })

#define  write_auxreg( value, reg ) \
    ({                              \
        __asm__ __volatile__ (      \
        " sr    %0, [%1] "          \
        :                           \
        : "ir"(value), "i" (reg));  \
    })

static sensor_t p_bmi160_accel;
static uint32_t accel_frame_cnt = 0;

static sensor_t p_bmi160_gyro;
static uint32_t gyro_frame_cnt = 0;

#if BMI160_ENABLE_MAG
static sensor_t p_bmi160_mag;
static uint32_t mag_frame_cnt = 0;
#endif

#if TEST_MOTION_DETECT
static sensor_t p_bmi160_motion;
#endif

static uint32_t tick_test_duration;
static uint8_t print_sensor_data = 0;

static sensor_id_t sensor_id;

#define PRINT_INTERVAL 20

static void wait_ticks(uint32_t ticks)
{
    uint32_t start = get_uptime_32k();

    while((get_uptime_32k()-start) < ticks) ;
}

/* Only for test usage, msecs*33 should not overflow for uint32_t */
static void bmi160_delay_ms_test(uint32_t msecs)
{
    wait_ticks(((uint64_t)(msecs * 32768 ))/ 1000);
}

int read_accel_cb(struct sensor_data *sensor_data, void *priv_data)
{
    phy_accel_data_t *accel_data;

    accel_frame_cnt++;
    accel_data = (phy_accel_data_t *) sensor_data->data;

    if(_Rarely(sensor_data->sensor.sensor_type!=SENSOR_ACCELEROMETER))
        cu_print("[Error]%s: type mismatch!!\n", __func__);

    if (print_sensor_data && (accel_frame_cnt%PRINT_INTERVAL==0))
        cu_print("\tAccel[id:%d type:%d len:%d time:%x] (%d, %d, %d)\n",
               sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
               sensor_data->data_length,
               sensor_data->timestamp,
               accel_data->x, accel_data->y, accel_data->z);

    return 0;
}

int read_gyro_cb(struct sensor_data *sensor_data, void *priv_data)
{
    phy_gyro_data_t *gryo_data;

    gyro_frame_cnt++;
    gryo_data = (phy_gyro_data_t *) sensor_data->data;

    if(_Rarely(sensor_data->sensor.sensor_type!=SENSOR_GYROSCOPE))
        cu_print("[Error]%s: type mismatch!!\n", __func__);

    if (print_sensor_data && (gyro_frame_cnt%PRINT_INTERVAL==0))
        cu_print("\tGyro [id:%d type:%d len:%d time:%x] (%d, %d, %d)\n",
               sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
               sensor_data->data_length,
               sensor_data->timestamp,
               gryo_data->x, gryo_data->y, gryo_data->z);

    return 0;
}

#if BMI160_ENABLE_MAG
int read_mag_cb(struct sensor_data *sensor_data, void *priv_data)
{
    phy_mag_data_t *mag_data;

    mag_frame_cnt++;
    mag_data = (phy_mag_data_t *) sensor_data->data;

    if(_Rarely(sensor_data->sensor.sensor_type!=SENSOR_MAGNETOMETER))
        cu_print("[Error]%s: type mismatch!!\n", __func__);

    if (print_sensor_data && (mag_frame_cnt%PRINT_INTERVAL==0))
        cu_print("\tMag  [id:%d type:%d len:%d time:%x] (%d, %d, %d)\n",
               sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
               sensor_data->data_length,
               sensor_data->timestamp,
               mag_data->x, mag_data->y, mag_data->z);

    return 0;
}
#endif

void test_polling_data_read(uint32_t polling_rate_hz)
{
    phy_accel_data_t *reg_accel;
    phy_gyro_data_t *reg_gyro;
    uint8_t sensor_data_bytes[20];
    struct sensor_data *sensor_data = (struct sensor_data *)sensor_data_bytes;

    int time_wait = 1000 / polling_rate_hz;
    uint32_t time_start, time_eslapse;
    uint32_t cnt_accel =0, cnt_gyro=0;

#if BMI160_ENABLE_MAG
    phy_mag_data_t *reg_mag;
    uint32_t cnt_mag=0;
#endif

    cu_print("<Accel Register Data Read (polling), %d s %s print>\n",
           tick_test_duration/TICKS_1S, print_sensor_data ? "with" : "without");

    sensor_data->data_length = BMI160_ACCEL_FRAME_SIZE;
    time_start = get_time_ms();
    while (1) {
        phy_sensor_data_read(p_bmi160_accel, sensor_data);
        cnt_accel++;
        reg_accel = (phy_accel_data_t *) sensor_data->data;
        if (print_sensor_data && (cnt_accel%PRINT_INTERVAL==0))
            cu_print("\tAccel[id:%d type:%d len:%d time:%x] (%d, %d, %d)\n",
                   sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
                   sensor_data->data_length,
                   sensor_data->timestamp,
                   reg_accel->x, reg_accel->y, reg_accel->z);
        bmi160_delay_ms_test(time_wait);
        if ((time_eslapse =
             get_time_ms() - time_start) >= tick_test_duration)
            break;
    }
    cu_print("\t%d Accel Sample Read in %d ms\n", cnt_accel, time_eslapse);

    cu_print("<Gyro Register Data Read (polling), %d s %s print>\n",
           tick_test_duration/TICKS_1S, print_sensor_data ? "with" : "without");

    sensor_data->data_length = BMI160_GYRO_FRAME_SIZE;

    time_start = get_time_ms();
    while (1) {
        phy_sensor_data_read(p_bmi160_gyro, sensor_data);
        cnt_gyro++;
        reg_gyro = (phy_gyro_data_t *) sensor_data->data;
        if (print_sensor_data && (cnt_gyro%PRINT_INTERVAL==0))
            cu_print("\tGyro [id:%d type:%d len:%d time:%x] (%d, %d, %d)\n",
                   sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
                   sensor_data->data_length,
                   sensor_data->timestamp,
                   reg_gyro->x, reg_gyro->y, reg_gyro->z);
        bmi160_delay_ms_test(time_wait);
        if ((time_eslapse =
             get_time_ms() - time_start) >= tick_test_duration)
            break;
    }
    cu_print("\t%d Gyro Sample Read in %d ms\n", cnt_gyro, time_eslapse);

#if BMI160_ENABLE_MAG
    cu_print("<Mag Register Data Read (polling), %d s %s print>\n",
           tick_test_duration/TICKS_1S, print_sensor_data ? "with" : "without");
    sensor_data->data_length = BMI160_MAG_FRAME_SIZE;

    time_start = get_time_ms();
    while (1) {
        phy_sensor_data_read(p_bmi160_mag, sensor_data);
        cnt_mag++;
        reg_mag = (phy_mag_data_t *) sensor_data->data;
        if (print_sensor_data && (cnt_mag%PRINT_INTERVAL==0))
            cu_print("\tMag  [id:%d type:%d len:%d time:%x] (%d, %d, %d)\n",
                   sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
                   sensor_data->data_length,
                   sensor_data->timestamp,
                   reg_mag->x, reg_mag->y, reg_mag->z);
        bmi160_delay_ms_test(time_wait);
        if ((time_eslapse =
             get_time_ms() - time_start) >= tick_test_duration)
            break;
    }
    cu_print("\t%d Mag Sample Read in %d ms\n", cnt_mag, time_eslapse);
#endif
}

uint8_t accel_fifo_buf[FIFO_BUF_LEN];
uint8_t gyro_fifo_buf[FIFO_BUF_LEN];
uint8_t mag_fifo_buf[FIFO_BUF_LEN];
void bmi160_fifo_polling_read_test(void)
{
    uint32_t time_start, time_eslapse;
    uint16_t actual_len_accel = 0;
    uint16_t actual_len_gyro = 0;
    uint16_t frame_cnt_accel = 0;
    uint16_t frame_cnt_gyro = 0;
    int print_cnt_accel = 0;
    int print_cnt_gyro = 0;
    phy_accel_data_t *accel_data;
    phy_gyro_data_t *gyro_data;
    int i;

    phy_sensor_enable_hwfifo_with_buffer(p_bmi160_accel, 1, accel_fifo_buf, FIFO_BUF_LEN);
    phy_sensor_enable_hwfifo_with_buffer(p_bmi160_gyro, 1, gyro_fifo_buf, FIFO_BUF_LEN);

#if BMI160_ENABLE_MAG
    uint16_t actual_len_mag = 0;
    uint16_t frame_cnt_mag = 0;
    int print_cnt_mag = 0;
    phy_mag_data_t *mag_data;
    phy_sensor_enable_hwfifo_with_buffer(p_bmi160_mag, 1, mag_fifo_buf, FIFO_BUF_LEN);
    cu_print("<Accel/Gyro/Mag FIFO Read (polling), %d s %s print>\n",
             tick_test_duration / TICKS_1S,
             print_sensor_data ? "with" : "without");
#else
    cu_print("<Accel/Gyro FIFO Read (polling), %d s %s print>\n",
             tick_test_duration / TICKS_1S,
             print_sensor_data ? "with" : "without");

#endif


    time_start = get_time_ms();
    bmi160_flush_fifo();

    while (1) {
        bmi160_delay_ms_test(500);
        actual_len_accel = phy_sensor_fifo_read(p_bmi160_accel, NULL, 0);
        actual_len_gyro = phy_sensor_fifo_read(p_bmi160_gyro, NULL, 0);
#if BMI160_ENABLE_MAG
        actual_len_mag = phy_sensor_fifo_read(p_bmi160_mag, NULL, 0);
#endif
        frame_cnt_accel += actual_len_accel/BMI160_ACCEL_FRAME_SIZE;
        frame_cnt_gyro += actual_len_gyro/BMI160_GYRO_FRAME_SIZE;

        for(i=0; i<actual_len_accel; i+=BMI160_ACCEL_FRAME_SIZE){
            accel_data = (phy_accel_data_t *)(&accel_fifo_buf[i]);

            if(print_cnt_accel%PRINT_INTERVAL==0)
                cu_print("\tAccel (%d, %d, %d)\n", accel_data->x, accel_data->y, accel_data->z);

            print_cnt_accel++;
        }
        for(i=0; i<actual_len_gyro; i+=BMI160_GYRO_FRAME_SIZE){
            gyro_data = (phy_gyro_data_t *)(&gyro_fifo_buf[i]);

            if(print_cnt_gyro%PRINT_INTERVAL==0)
                cu_print("\tGyro  (%d, %d, %d)\n", gyro_data->x, gyro_data->y, gyro_data->z);

            print_cnt_gyro++;
        }

#if BMI160_ENABLE_MAG
        frame_cnt_mag += actual_len_mag/BMI160_MAG_FRAME_SIZE;

        for(i=0; i<actual_len_mag && i<FIFO_BUF_LEN; i+=BMI160_MAG_FRAME_SIZE){
            mag_data = (phy_mag_data_t *)(&mag_fifo_buf[i]);

            if(print_cnt_mag%PRINT_INTERVAL==0)
                cu_print("\tMag   (%d, %d, %d)\n", mag_data->x, mag_data->y, mag_data->z);

            print_cnt_mag++;
        }
#endif

        if ((time_eslapse =
             get_time_ms() - time_start) >= tick_test_duration)
            break;
    }

    cu_print("\t%d Accel Sample Read in %d ms\n", frame_cnt_accel, time_eslapse);
    cu_print("\t%d Gyro Sample Read in %d ms\n", frame_cnt_gyro, time_eslapse);
#if BMI160_ENABLE_MAG
    cu_print("\t%d Mag Sample Read in %d ms\n", frame_cnt_mag, time_eslapse);
#endif

}

void bmi160_accel_fifo_data_read_test(void)
{
    uint32_t time_start, time_eslapse;

    phy_sensor_enable_hwfifo(p_bmi160_accel, 1, 0);
    phy_sensor_enable_hwfifo(p_bmi160_gyro, 0, 0);
#if BMI160_ENABLE_MAG
    phy_sensor_enable_hwfifo(p_bmi160_mag, 0, 0);
#endif

    cu_print("<Accel FIFO Read, %d s %s print>\n",
           tick_test_duration / TICKS_1S,
           print_sensor_data ? "with" : "without");

    time_start = get_time_ms();
    bmi160_flush_fifo();
    phy_sensor_data_register_callback(p_bmi160_accel, read_accel_cb, NULL, TEST_DATA_ODR_HZ*10);
    bmi160_delay_ms_test(tick_test_duration);
    phy_sensor_data_unregister_callback(p_bmi160_accel);
    time_eslapse = get_time_ms() - time_start;
    cu_print("\t%d Accel Sample Read in %d ms\n", accel_frame_cnt, time_eslapse);
    accel_frame_cnt = 0;
}

void bmi160_gyro_fifo_data_read_test(void)
{
    uint32_t time_start, time_eslapse;

    phy_sensor_enable_hwfifo(p_bmi160_accel, 0, 0);
    phy_sensor_enable_hwfifo(p_bmi160_gyro, 1, 0);
#if BMI160_ENABLE_MAG
    phy_sensor_enable_hwfifo(p_bmi160_mag, 0, 0);
#endif

    cu_print("<Gyro FIFO Read, %d s %s print>\n",
           tick_test_duration / TICKS_1S,
           print_sensor_data ? "with" : "without");

    time_start = get_time_ms();
    bmi160_flush_fifo();
    phy_sensor_data_register_callback(p_bmi160_gyro, read_gyro_cb, NULL, TEST_GYRO_DATA_ODR_HZ*10);
    bmi160_delay_ms_test(tick_test_duration);
    phy_sensor_data_unregister_callback(p_bmi160_gyro);
    time_eslapse = get_time_ms() - time_start;
    cu_print("\t%d Gyro Sample Read in %d ms\n", gyro_frame_cnt, time_eslapse);
    gyro_frame_cnt = 0;
}

#if BMI160_ENABLE_MAG
void bmi160_mag_fifo_data_read_test(void)
{
    uint32_t time_start, time_eslapse;

    phy_sensor_enable_hwfifo(p_bmi160_accel, 0, 0);
    phy_sensor_enable_hwfifo(p_bmi160_gyro, 0, 0);
    phy_sensor_enable_hwfifo(p_bmi160_mag, 1, 0);

    cu_print("<Mag FIFO Read, %d seconds %s print>\n",
           tick_test_duration / TICKS_1S,
           print_sensor_data ? "with" : "without");

    time_start = get_time_ms();
    bmi160_flush_fifo();
    phy_sensor_data_register_callback(p_bmi160_mag, read_mag_cb, NULL, TEST_DATA_ODR_HZ * 10);
    bmi160_delay_ms_test(tick_test_duration);
    phy_sensor_data_unregister_callback(p_bmi160_mag);
    time_eslapse = get_time_ms() - time_start;
    cu_print("\t%d Mag Sample Read in %d ms\n", mag_frame_cnt, time_eslapse);
    mag_frame_cnt = 0;
}
#endif

void bmi160_all_fifo_data_read_test(void)
{
    uint32_t time_start, time_eslapse;

    phy_sensor_enable_hwfifo(p_bmi160_accel, 1, 0);
    phy_sensor_enable_hwfifo(p_bmi160_gyro, 1, 0);
#if BMI160_ENABLE_MAG
    phy_sensor_enable_hwfifo(p_bmi160_mag, 1, 0);

    cu_print("<Accel/Gyro/Mag FIFO Read, %d s %s print>\n",
           tick_test_duration / TICKS_1S,
           print_sensor_data ? "with" : "without");
#else
    cu_print("<Accel/Gyro FIFO Read, %d s %s print>\n",
           tick_test_duration / TICKS_1S,
           print_sensor_data ? "with" : "without");
#endif

    time_start = get_time_ms();
    bmi160_flush_fifo();
    phy_sensor_data_register_callback(p_bmi160_accel, read_accel_cb, NULL, TEST_DATA_ODR_HZ*10);
    phy_sensor_data_register_callback(p_bmi160_gyro, read_gyro_cb, NULL, TEST_GYRO_DATA_ODR_HZ*10);
#if BMI160_ENABLE_MAG
    phy_sensor_data_register_callback(p_bmi160_mag, read_mag_cb, NULL, TEST_DATA_ODR_HZ*10);
#endif
    bmi160_delay_ms_test(tick_test_duration);
    phy_sensor_data_unregister_callback(p_bmi160_accel);
    phy_sensor_data_unregister_callback(p_bmi160_gyro);
 #if BMI160_ENABLE_MAG
    phy_sensor_data_unregister_callback(p_bmi160_mag);
#endif
    time_eslapse = get_time_ms() - time_start;

    cu_print("\t%d Accel Sample Read in %d ms\n", accel_frame_cnt, time_eslapse);
    cu_print("\t%d Gyro Sample Read in %d ms\n", gyro_frame_cnt, time_eslapse);
    accel_frame_cnt = 0;
    gyro_frame_cnt = 0;
 #if BMI160_ENABLE_MAG
    cu_print("\t%d Mag Sample Read in %d ms\n", mag_frame_cnt, time_eslapse);
    mag_frame_cnt = 0;
#endif
}

void test_interrupt_data_read()
{
    uint32_t time_start, time_eslapse;
    cu_print("<Accel Register Data Read (interrupt), %d s %s print>\n",
           tick_test_duration / TICKS_1S,
           print_sensor_data ? "with" : "without");

    time_start = get_time_ms();
    phy_sensor_data_register_callback(p_bmi160_accel, read_accel_cb, NULL, TEST_DATA_ODR_HZ*10);
    bmi160_delay_ms_test(tick_test_duration);
    phy_sensor_data_unregister_callback(p_bmi160_accel);
    time_eslapse = get_time_ms() - time_start;
    cu_print("\t%d Accel Sample Read in %d ms\n", accel_frame_cnt, time_eslapse);
    accel_frame_cnt = 0;

    cu_print("<Gyro Register Data Read (interrupt), %d s %s print>\n",
           tick_test_duration / TICKS_1S,
           print_sensor_data ? "with" : "without");
    time_start = get_time_ms();
    phy_sensor_data_register_callback(p_bmi160_gyro, read_gyro_cb, NULL, TEST_GYRO_DATA_ODR_HZ*10);
    bmi160_delay_ms_test(tick_test_duration);
    phy_sensor_data_unregister_callback(p_bmi160_gyro);
    time_eslapse = get_time_ms() - time_start;
    cu_print("\t%d Gyro Sample Read in %d ms\n", gyro_frame_cnt, time_eslapse);
    gyro_frame_cnt = 0;

 #if BMI160_ENABLE_MAG
    cu_print("<Mag Register Data Read (interrupt), %d seconds %s print>\n",
            tick_test_duration / TICKS_1S,
            print_sensor_data ? "with" : "without");
    time_start = get_time_ms();
    phy_sensor_data_register_callback(p_bmi160_mag, read_mag_cb, NULL, TEST_DATA_ODR_HZ * 10);
    bmi160_delay_ms_test(tick_test_duration);
    phy_sensor_data_unregister_callback(p_bmi160_mag);
    time_eslapse = get_time_ms() - time_start;
    cu_print("\t%d Mag Sample Read in %d ms\n", mag_frame_cnt, time_eslapse);
    mag_frame_cnt = 0;
#endif
}

void bmi160_reg_data_read_test(void)
{
    /* 2 seconds for each test case
     * Change the value here if you like.
     */
    tick_test_duration = TICKS_2S;
    print_sensor_data = 1;
    test_polling_data_read(TEST_DATA_ODR_HZ);
    test_interrupt_data_read();
}


void bmi160_fifo_data_read_test(void)
{
    tick_test_duration = TICKS_2S;
    print_sensor_data = 1;
    bmi160_accel_fifo_data_read_test();
    bmi160_gyro_fifo_data_read_test();
#if BMI160_ENABLE_MAG
    bmi160_mag_fifo_data_read_test();
#endif
    bmi160_all_fifo_data_read_test();
    bmi160_fifo_polling_read_test();
}

#if TEST_SLEEP_WAKEUP
static uint32_t wakeup_time_start, wakeup_time_eslapse;

void bmi160_accel_fifo_data_read_wake_test(void)
{
    phy_sensor_enable_hwfifo(p_bmi160_accel, 1, 0);
    phy_sensor_enable_hwfifo(p_bmi160_gyro, 0, 0);
 #if BMI160_ENABLE_MAG
    phy_sensor_enable_hwfifo(p_bmi160_mag, 0, 0);
#endif

    wakeup_time_start = get_time_ms();
    bmi160_flush_fifo();
    phy_sensor_data_register_callback(p_bmi160_accel, read_accel_cb, NULL, 500);
}

void bmi160_arc_sleep_wake_test()
{
    /* enter SS2
     * only interrupt with priority 0 can wake up the processor
     */
    uint8_t op = 0xF0;
    uint32_t time0, time1;
    unsigned int priority;

    cu_print("<Test ARC SLEEP/WAKEUP>\n");

    phy_sensor_set_odr_value(p_bmi160_accel, 500);

    tick_test_duration = TICKS_2S;
    print_sensor_data = 0;

    bmi160_accel_fifo_data_read_wake_test();

    bmi160_delay_ms_test(TICKS_1S);
    cu_print("\tSLEEP, Waiting for interrupt from BMI160\n");

    /* change the priority of interrupt to 0 (highest) */
#if BMI160_USE_INT_PIN2
    write_auxreg(IO_GPIO_8B0_INT_INTR_FLAG, ARC_IRQ_SELECT);
#else
    write_auxreg(SOC_GPIO_AON_INTERRUPT, ARC_IRQ_SELECT);
#endif
    priority = read_auxreg(ARC_IRQ_PRRORITY);
    write_auxreg(0, ARC_IRQ_PRRORITY);

    /* The CPU can be woken up by interrupt of priority 0 only */
    time0 = get_uptime_32k();
    __asm__ volatile ("SLEEP %0"::"r" (op));
    time1 = get_uptime_32k();

    cu_print("\tWakeup, sleep for %d ms\n", (time1 - time0) / 32);

    bmi160_delay_ms_test(tick_test_duration-TICKS_1S);
    phy_sensor_data_unregister_callback(p_bmi160_accel);
    wakeup_time_eslapse = get_time_ms() - wakeup_time_start;
    accel_frame_cnt = 0;

    /*restore the priority changed before */
#if BMI160_USE_INT_PIN2
    write_auxreg(IO_GPIO_8B0_INT_INTR_FLAG, ARC_IRQ_SELECT);
#else
    write_auxreg(SOC_GPIO_AON_INTERRUPT, ARC_IRQ_SELECT);
#endif
    write_auxreg(priority, ARC_IRQ_PRRORITY);
}
#endif

#if TEST_MOTION_DETECT
static volatile uint8_t do_fifo_read = 1;
int motion_detect_test_callback (struct sensor_data * sensor_data, void *priv_data)
{
    uint8_t *event = sensor_data->data;
    char *str_event;
    if(*event==PHY_SENSOR_EVENT_DOUBLE_TAP){
        str_event = "double tap";
        do_fifo_read = 1;
    } else if(*event==PHY_SENSOR_EVENT_NO_MOTION) {
        str_event = "no motion";
        do_fifo_read = 0;
    } else if(*event==PHY_SENSOR_EVENT_ANY_MOTION){
        str_event = "any motion";
        do_fifo_read = 1;
    }else
        str_event = "unkown";

    cu_print("\tMotion [id:%d type:%d len:%d time:%x] (event: %s)\n",
                   sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
                   sensor_data->data_length,
                   sensor_data->timestamp,
                   str_event);
    return 0;
}

void bmi160_motion_detection_test(void)
{
    phy_sensor_type_bitmap bitmap;
    bitmap = 1 << SENSOR_MOTION_DETECTOR;
    get_sensor_list(bitmap, &sensor_id, 1);
    cu_print("type=%d, id=%d\n", sensor_id.sensor_type, sensor_id.dev_id);

    p_bmi160_motion = phy_sensor_open(sensor_id.sensor_type, sensor_id.dev_id);
    CU_ASSERT("Failed to open motion detector\n", p_bmi160_motion !=NULL);
    phy_sensor_enable(p_bmi160_motion, 1);
    phy_sensor_data_register_callback(p_bmi160_motion, motion_detect_test_callback, NULL, 0);
}
#endif

void bmi160_unit_test(void)
{
    cu_print("###################################################################\n");
    cu_print("# bmi160 is connected via i2c or spi in the system                #\n");
    cu_print("#                                                                 #\n");
    cu_print("# Purpose of bmi160 unit tests :                                  #\n");
    cu_print("#            Test integration of bmi160 driver and phy_sensor_api #\n");
    cu_print("#            Read sensor data from sensor data register           #\n");
    cu_print("#            in both polling mode and interrupt mode              #\n");
    cu_print("#            Read sensor data from hw fifo in interrupt mode      #\n");
    cu_print("#            Test ARC wakeup by interrupt of bmi160               #\n");
    cu_print("###################################################################\n");

    phy_sensor_type_bitmap bitmap;
    uint8_t raw_data_len;
    uint8_t report_mode_mask;
    phy_sensor_range_property_t sensing_range;
    phy_sensor_fifo_share_property_t fifo_share;

    bitmap = 1 << SENSOR_ACCELEROMETER;
    get_sensor_list(bitmap, &sensor_id, 1);
    p_bmi160_accel = phy_sensor_open(sensor_id.sensor_type, sensor_id.dev_id);
    CU_ASSERT("Failed to open accel\n", p_bmi160_accel !=NULL);
    cu_print("BMI160 accel dev id = %d\n", sensor_id.dev_id);

    phy_sensor_get_property(p_bmi160_accel, SENSOR_PROP_SENSING_RANGE, &sensing_range);
    cu_print("BMI160 accelerometer sensing range: %d ~ %d g\n", sensing_range.low, sensing_range.high);

    phy_sensor_get_raw_data_len(p_bmi160_accel, &raw_data_len);
    CU_ASSERT("raw data length mismatch\n", raw_data_len==sizeof(phy_accel_data_t));
    phy_sensor_get_report_mode_mask(p_bmi160_accel, &report_mode_mask);
    CU_ASSERT("report mode mask mismatch\n", report_mode_mask==TEST_REPORT_MODE_MASK);

    bitmap = 1 << SENSOR_GYROSCOPE;
    get_sensor_list(bitmap, &sensor_id, 1);
    p_bmi160_gyro = phy_sensor_open(sensor_id.sensor_type, sensor_id.dev_id);
    CU_ASSERT("Failed to open gyro\n", p_bmi160_gyro !=NULL);
    cu_print("BMI160 gyro dev id = %d\n", sensor_id.dev_id);

    phy_sensor_get_property(p_bmi160_gyro, SENSOR_PROP_SENSING_RANGE, &sensing_range);
    cu_print("BMI160 gyroscope sensing range: %d ~ %d deg/s\n", sensing_range.low, sensing_range.high);

    phy_sensor_get_raw_data_len(p_bmi160_gyro, &raw_data_len);
    CU_ASSERT("raw data length mismatch\n", raw_data_len==sizeof(phy_gyro_data_t));
    phy_sensor_get_report_mode_mask(p_bmi160_gyro, &report_mode_mask);
    CU_ASSERT("report mode mask mismatch\n", report_mode_mask==TEST_REPORT_MODE_MASK);

#if BMI160_ENABLE_MAG
    bitmap = 1 << SENSOR_MAGNETOMETER;
    get_sensor_list(bitmap, &sensor_id, 1);
    p_bmi160_mag = phy_sensor_open(sensor_id.sensor_type, sensor_id.dev_id);
    CU_ASSERT("Failed to open mag\n", p_bmi160_mag !=NULL);
    cu_print("BMI160 mag dev id = %d\n", sensor_id.dev_id);

    phy_sensor_get_property(p_bmi160_mag, SENSOR_PROP_SENSING_RANGE, &sensing_range);
    cu_print("BMI160 magnetic sensing range: %d ~ %d uT\n", sensing_range.low, sensing_range.high);

    phy_sensor_get_raw_data_len(p_bmi160_mag, &raw_data_len);
    CU_ASSERT("raw data length mismatch\n", raw_data_len==sizeof(phy_mag_data_t));
    phy_sensor_get_report_mode_mask(p_bmi160_mag, &report_mode_mask);
    CU_ASSERT("report mode mask mismatch\n", report_mode_mask==TEST_REPORT_MODE_MASK);
#endif


    phy_sensor_get_property(p_bmi160_accel, SENSOR_PROP_FIFO_SHARE_BITMAP, &fifo_share);
    cu_print("BMI160 accel: fifo share bitmap=0x%x\n", fifo_share.bitmap);


    phy_sensor_get_property(p_bmi160_gyro, SENSOR_PROP_FIFO_SHARE_BITMAP, &fifo_share);
    cu_print("BMI160 gyro: fifo share bitmap=0x%x\n", fifo_share.bitmap);

#if BMI160_ENABLE_MAG
    phy_sensor_get_property(p_bmi160_mag, SENSOR_PROP_FIFO_SHARE_BITMAP, &fifo_share);
    cu_print("BMI160 mag: fifo share bitmap=0x%x\n", fifo_share.bitmap);
#endif

    phy_sensor_set_odr_value(p_bmi160_accel, TEST_DATA_ODR_HZ*10);
    phy_sensor_enable(p_bmi160_accel, 1);

    phy_sensor_set_odr_value(p_bmi160_gyro, TEST_GYRO_DATA_ODR_HZ*10);
    phy_sensor_enable(p_bmi160_gyro, 1);

#if BMI160_ENABLE_MAG
    phy_sensor_set_odr_value(p_bmi160_mag, TEST_DATA_ODR_HZ*10);
    phy_sensor_enable(p_bmi160_mag, 1);
#endif

    bmi160_reg_data_read_test();
    bmi160_fifo_data_read_test();

#if TEST_SLEEP_WAKEUP
    bmi160_arc_sleep_wake_test();
#endif

#if TEST_MOTION_DETECT
    phy_sensor_set_odr_value(p_bmi160_accel, TEST_DATA_ODR_HZ*20);
    phy_sensor_set_odr_value(p_bmi160_gyro, TEST_DATA_ODR_HZ*20);
    phy_sensor_set_odr_value(p_bmi160_mag, TEST_DATA_ODR_HZ*20);
    phy_sensor_enable_hwfifo_with_buffer(p_bmi160_accel, 1, accel_fifo_buf, FIFO_BUF_LEN);
    phy_sensor_enable_hwfifo_with_buffer(p_bmi160_gyro, 1, gyro_fifo_buf, FIFO_BUF_LEN);
    phy_sensor_enable_hwfifo_with_buffer(p_bmi160_mag, 1, mag_fifo_buf, FIFO_BUF_LEN);
    bmi160_motion_detection_test();
    uint16_t actual_len = 0;
    uint16_t actual_len1 = 0;
    uint16_t actual_len2 = 0;
    int print_cnt = 0;
    int print_cnt1 = 0;
    int print_cnt2 = 0;
    phy_accel_data_t *accel_data;
    phy_gyro_data_t *gyro_data;
    phy_mag_data_t *mag_data;

    bmi160_flush_fifo();

    while(1) {
        if(do_fifo_read){
            bmi160_delay_ms_test(300);

            actual_len = phy_sensor_fifo_read(p_bmi160_accel, NULL, 0);
            actual_len1 = phy_sensor_fifo_read(p_bmi160_gyro, NULL, 0);
            actual_len2 = phy_sensor_fifo_read(p_bmi160_mag, NULL, 0);


            if(actual_len == 0)
                cu_print("(A) no data read\n");

            for(int i=0; i<actual_len; i+=BMI160_ACCEL_FRAME_SIZE){
                accel_data = (phy_accel_data_t *)(&accel_fifo_buf[i]);

                if(print_cnt%100==0 && do_fifo_read)
                    cu_print("\tAccel (%d, %d, %d)\n", accel_data->x, accel_data->y, accel_data->z);

                print_cnt++;
            }

            if(actual_len1 == 0)
                cu_print("(G) no data read\n");

            for(int i=0; i<actual_len1; i+=BMI160_GYRO_FRAME_SIZE){
                gyro_data = (phy_gyro_data_t *)(&gyro_fifo_buf[i]);

                if(print_cnt1%100==0 && do_fifo_read)
                    cu_print("\tGyro (%d, %d, %d)\n", gyro_data->x, gyro_data->y, gyro_data->z);

                print_cnt1++;
            }

            if(actual_len2 == 0)
                cu_print("(M) no data read\n");

            for(int i=0; i<actual_len2; i+=BMI160_MAG_FRAME_SIZE){
                mag_data = (phy_mag_data_t *)(&mag_fifo_buf[i]);

                if(print_cnt2%100==0 && do_fifo_read)
                    cu_print("\tMAG (%d, %d, %d)\n", mag_data->x, mag_data->y, mag_data->z);

                print_cnt2++;
            }

        }
    }
#endif

    phy_sensor_close(p_bmi160_accel);
    phy_sensor_close(p_bmi160_gyro);
#if BMI160_ENABLE_MAG
    phy_sensor_close(p_bmi160_mag);
#endif
}
