#ifndef __SENSOR_BUS_COMMON_H__
#define __SENSOR_BUS_COMMON_H__

#include "drivers/serial_bus_access.h"

#define SPI_READ_CMD        (1<<7)
#define MS_PER_NANO_SYS_TIMER_TICK 10

typedef enum _sensor_bus_type {
    SENSOR_BUS_TYPE_I2C = 0,
    SENSOR_BUS_TYPE_SPI,
} SENSOR_BUS_TYPE;

struct sensor_sba_req {
    sba_request_t req;
    T_SEMAPHORE sem;
};

struct sensor_sba_info {
    struct sensor_sba_req* reqs;
    uint8_t bitmap;
    uint8_t req_cnt;
    SENSOR_BUS_TYPE bus_type;
    uint8_t dev_id;
};


DRIVER_API_RC sensor_bus_access(struct sensor_sba_info *info, uint8_t reg_addr, uint8_t * reg_data,
                                uint32_t cnt, bool req_read, uint8_t *write_buffer);

struct sensor_sba_info * sensor_config_bus(uint32_t dev_addr, uint8_t dev_id, SBA_BUSID bus_id, SENSOR_BUS_TYPE bus_type, int req_num);

/*!
 *  @brief This function is usded to delay specific milli seconds
 *  @param msek: delay in milli seconds
 */
#define AON_CNT_TICK_PER_MS 33
void sensor_delay_ms(uint32_t msecs);
#endif