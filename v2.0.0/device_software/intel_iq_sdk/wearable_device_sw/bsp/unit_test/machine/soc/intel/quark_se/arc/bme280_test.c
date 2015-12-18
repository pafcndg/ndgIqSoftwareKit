#include "drivers/eiaextensions.h"
#include "util/cunit_test.h"
#include "infra/time.h"
#include "sensors/phy_sensor_api/phy_sensor_api.h"
#include "drivers/sensor/sensor_bus_common.h"

#define PRINT_INTERVAL 10
#define TICKS_1S    1000

static sensor_t p_temp;
static sensor_t p_press;
static sensor_t p_humid;
static uint32_t tick_test_duration;

void bme280_unit_test(void)
{
    cu_print("###################################################################\n");
    cu_print("# bme280 is connected via i2c or spi in the system                #\n");
    cu_print("#                                                                 #\n");
    cu_print("# Purpose of bmi280 unit tests :                                  #\n");
    cu_print("#            Test integration of bme280 driver and phy_sensor_api #\n");
    cu_print("#            Read sensor data from sensor data register           #\n");
    cu_print("###################################################################\n");

    phy_sensor_type_bitmap bitmap;
	sensor_id_t sensor_id;
    int time_wait = 5000 / 100;
    phy_temp_data_t *reg_temp;
    phy_humi_data_t *reg_humi;
    phy_baro_data_t *reg_baro;
    uint32_t time_start, time_eslapse;
    uint32_t cnt =0;
    uint8_t sensor_data_bytes[20] = {0};
    struct sensor_data *sensor_data = (struct sensor_data *)sensor_data_bytes;
    phy_sensor_range_property_t sensing_range;

    uint8_t raw_data_len;
    uint8_t report_mode_mask;

    tick_test_duration = 2000;

    bitmap = 1 << SENSOR_TEMPERATURE;
    get_sensor_list(bitmap, &sensor_id, 1);
    p_temp = phy_sensor_open(sensor_id.sensor_type, sensor_id.dev_id);
    CU_ASSERT("Failed to open temp\n", p_temp !=NULL);

    phy_sensor_get_property(p_temp, SENSOR_PROP_SENSING_RANGE, &sensing_range);
    cu_print("BME280 temperature sensing range: %d ~ %d degrees Celsius\n", sensing_range.low, sensing_range.high);

    phy_sensor_get_raw_data_len(p_temp, &raw_data_len);
    CU_ASSERT("raw data length mismatch\n", raw_data_len==sizeof(phy_temp_data_t));
    phy_sensor_get_report_mode_mask(p_temp, &report_mode_mask);
    CU_ASSERT("report mode mask mismatch\n", report_mode_mask==PHY_SENSOR_REPORT_MODE_POLL_REG_MASK);

	bitmap = 1 << SENSOR_BAROMETER;
    get_sensor_list(bitmap, &sensor_id, 1);
    p_press = phy_sensor_open(sensor_id.sensor_type, sensor_id.dev_id);
    CU_ASSERT("Failed to open temp\n", p_press !=NULL);

    phy_sensor_get_property(p_press, SENSOR_PROP_SENSING_RANGE, &sensing_range);
    cu_print("BME280 temperature sensing range: %d ~ %d Pa\n", sensing_range.low, sensing_range.high);

    phy_sensor_get_raw_data_len(p_press, &raw_data_len);
    CU_ASSERT("raw data length mismatch\n", raw_data_len==sizeof(phy_baro_data_t));
    phy_sensor_get_report_mode_mask(p_press, &report_mode_mask);
    CU_ASSERT("report mode mask mismatch\n", report_mode_mask==PHY_SENSOR_REPORT_MODE_POLL_REG_MASK);

	bitmap = 1 << SENSOR_HUMIDITY;
    get_sensor_list(bitmap, &sensor_id, 1);
    p_humid = phy_sensor_open(sensor_id.sensor_type, sensor_id.dev_id);
    CU_ASSERT("Failed to open temp\n", p_humid !=NULL);


    phy_sensor_get_property(p_humid, SENSOR_PROP_SENSING_RANGE, &sensing_range);
    cu_print("BME280 temperature sensing range: %d%% ~ %d%%\n", sensing_range.low, sensing_range.high);

    phy_sensor_get_raw_data_len(p_humid, &raw_data_len);
    CU_ASSERT("raw data length mismatch\n", raw_data_len==sizeof(phy_humi_data_t));
    phy_sensor_get_report_mode_mask(p_humid, &report_mode_mask);
    CU_ASSERT("report mode mask mismatch\n", report_mode_mask==PHY_SENSOR_REPORT_MODE_POLL_REG_MASK);

    phy_sensor_enable(p_temp, true);
    phy_sensor_enable(p_press, true);
    phy_sensor_enable(p_humid, true);

    cu_print("<Temperature Data Read, %d s>\n", tick_test_duration/TICKS_1S);
    sensor_data->data_length = 4;
    time_start = get_uptime_ms();
    while (1) {
        phy_sensor_data_read(p_temp, sensor_data);
        cnt++;
        reg_temp = (phy_temp_data_t *) sensor_data->data;
        if (cnt%PRINT_INTERVAL==0)
            cu_print("\tTemper[id:%d type:%d len:%d time:%x] (%d/100 DegC)\n",
                   sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
                   sensor_data->data_length,
                   sensor_data->timestamp,
                   reg_temp->value);
        sensor_delay_ms(time_wait);
        if ((time_eslapse =
             get_uptime_ms() - time_start) >= tick_test_duration)
            break;
    }
    cu_print("\t%d Temperature Sample Read in %d ms\n", cnt, time_eslapse);

    cu_print("<Pressure Data Read, %d s>\n", tick_test_duration/TICKS_1S);
	cnt = 0;
    sensor_data->data_length = 4;
    time_start = get_uptime_ms();
    while (1) {
        phy_sensor_data_read(p_press, sensor_data);
        cnt++;
        reg_baro = (phy_baro_data_t *) sensor_data->data;
        if (cnt%PRINT_INTERVAL==0)
            cu_print("\tPress [id:%d type:%d len:%d time:%x] (%u Pa)\n",
                   sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
                   sensor_data->data_length,
                   sensor_data->timestamp,
                   reg_baro->value);
        sensor_delay_ms(time_wait);
        if ((time_eslapse =
             get_uptime_ms() - time_start) >= tick_test_duration)
            break;
    }
    cu_print("\t%d Pressure Sample Read in %d ms\n", cnt, time_eslapse);

    cu_print("<Humidity Data Read, %d s>\n", tick_test_duration/TICKS_1S);
    cnt = 0;
    sensor_data->data_length = 4;
    time_start = get_uptime_ms();
    while (1) {
        phy_sensor_data_read(p_humid, sensor_data);
        cnt++;
        reg_humi = (phy_humi_data_t *) sensor_data->data;
        if (cnt%PRINT_INTERVAL==0)
            cu_print("\tHumid [id:%d type:%d len:%d time:%x] (%u/1024 %%rH)\n",
                   sensor_data->sensor.dev_id, sensor_data->sensor.sensor_type,
                   sensor_data->data_length,
                   sensor_data->timestamp,
                   reg_humi->value);
        sensor_delay_ms(time_wait);
        if ((time_eslapse =
             get_uptime_ms() - time_start) >= tick_test_duration)
            break;
    }
    cu_print("\t%d Pressure Sample Read in %d ms\n", cnt, time_eslapse);

    phy_sensor_close(p_temp);
    phy_sensor_close(p_press);
    phy_sensor_close(p_humid);
}
