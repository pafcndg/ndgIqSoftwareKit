@addtogroup SS_SERVICE
@{

# Features

## Algorithms

Sensors Service manages all available sensors on the platform and
provides the framework to integrate sensor-fusion algorithms.
By default the below 5 algorithms are integrated in sensor service:
- Activity detection:  walking/running/biking,
- Step counting,
- user tapping detection: double tap/triple tap,
- Simple user gesture detection: single-flick/shaking,
- User-defined gestures: up-down, down-up, left-right, right-left.

## Generic sensor API

The _generic_ sensor API provides the same set of API for
- physical sensors (e.g. accelerometer, gyroscope and magnetometer sensor), and
- abstract sensors (e.g. activity sensor to report the detected walking/running/biking state).

It is composed of the following features:
- Sensor enumeration: that is to enumerate the sensor devices of a certain type
                      and inquiry sensor device’s property
- Sensor control: that is to discover, start/stop sensor and
                  subscribe/unsubscribe sensor data, calibration sensor
- Sensor data: it defines the data format that is reported to application.
- Sensor status: that is to retrieve sensor status, such as sensor device
                battery level, connection with sensor device lost etc.

@warning BLE sensors are not yet supported as BLE Central mode is not yet supported.


# Architecture

![Sensor Service Architecture Figure](sensor_service_architecture.png "Sensor Service Architecture")

Physical sensor driver API is the well-defined layer, which hides the
HW differences of various sensor hardware. This way it provides 2 benefits:
- The sensor drivers running under physical sensor driver API can be replaced
  seamlessly without impacting the SW running on top of physical driver API;
- SW running on top of physical sensor driver API does not care about the
  specific sensor hardware on the platform.

The Sensor Core is designed with flexible architecture, so that new
sensor algorithms can be integrated in a modular way thanks to:
- sensor core API between sensor core service <-> sensor core
- physical sensor driver API between sensor core <-> physical sensor
- alrogithm integration API exposed by Sensor Core

Sensor service software stack is designed with high scalability and provides
2 configuration options to achieve the best power and performance for different
usage scenarios.
- If there’re multiple client applications running on Quark (QRK) to request
  the same sensor device with different sampling/reporting data rate, the sensor
  service on QRK side should be enabled. The sensor service on QRK side can do
  the arbitration and dispatch data to multiple client applications.
  So that the traffic between QRK and ARC is reduced, and more power is saved.
- If there’re no multiple client applications running on QRK to request the same
  sensor device with different sampling/reporting data rate, the sensor service
  on QRK side can be disabled. This way more flash/RAM space is saved for QRK;
  also the data transmission latency is reduced.

@anchor how_to_add_new_sensor_algorithm
## How to integrate new sensor algorithm
The top view of how sensor core works is shown as follows:
@anchor figure_1
![Figure 1: Sensor Core Detailed Structure](sensor_core_detailed_architecture.png "Sensor Core Detailed Structure")

In the upper figure, there are 2 sets of main APIs:
- Sensor core API, i.e. algorithm integration API. In these APIs, the algorithms
  will communicate with upper level applications. Algorithms and sensors can be
  configured by these APIs, and all the output of algorithms can be passed to
  applications.
- [Pattern Matching API](@ref pattern_matching_api). Algorithms that use Pattern Matching as classifier can call these APIs to call Pattern Matching.

### How to start a new algorithm on sensor core

The detailed steps are as follows:
- Define new sensor data structure definition in sensor_data_format.h for the
  new algorithm.
- Add new sensor type definition in enum of sensor_data_format.h.
- Add algorithm type definition in enum of opencore_algo_support.h.
- Generate call back function file.
- Add new sensor scanning/sensor data subscribing/sensor data processing code
  in QRK main.c.
- Add related configuration in configuration files.

### How to add new algorithms to sensor core

The following things need to be clarified before the example.
- Sensor core runs in ARC, and all algorithms are called from QRK. So we need
  to add code in _corresponding location_ in QRK.
- This example only introduces how to have sensor raw data fed into exec
  callback function. How to use those raw data is beyond the scope of this
  example.

@warning Only file names are referred to. No absolute file path is given here as
         the code location might change. To find the absolute paths of those
         files, search their file names in root folder of code base.


Here are the steps about how to add an algorithm:

#### Add new sensor data structucture definition in sensor_data_format.h.

~~~~~~~~~~~{.c}
struct recognition_udg
{
    u16 size;
    s16 nClassLabel;
    s16 reserved;
}__packed;
~~~~~~~~~~~

#### Add new sensor type definition in enum of sensor_data_format.h:

~~~~~~~~~~~{.c}
    SENSOR_ABS_ACTIVITY,
    SENSOR_ALGO_DEMO,
    SENSOR_ALGO_UDG,         // new sensor type
    SENSOR_MOTION_DETECTOR,
    ......
    #define ACTIVITY_TYPE_MASK	        (1 << SENSOR_ABS_ACTIVITY)
    #define HUMI_TYPE_MASK     	        (1 << SENSOR_HUMIDITY)
    #define ALGO_DEMO_MASK              (1 << SENSOR_ALGO_DEMO)
    #define ALGO_UDG_MASK               (1 << SENSOR_ALGO_UDG)
    ......
    ACTIVITY_TYPE_MASK	|  \
    TEMP_TYPE_MAS       |  \
    ALGO_DEMO_MASK      |  \
    ALGO_UDG_MASK       |  \
~~~~~~~~~~~

#### Add algorithm type definition in enum of opencore_algo_support.h:

~~~~~~~~~~~{.c}
typedef enum
{
    BASIC_ALGO_GESTURE = 0,
    BASIC_ALGO_STEPCOUNTER,
    BASIC_ALGO_TAPPING,
    BASIC_ALGO_SIMPLEGES,
    BASIC_ALGO_RAWDATA,
    BASIC_ALGO_DEMO,
    BASIC_ALGO_UDG,    //new algo type
} basic_algo_type_t;
~~~~~~~~~~~

#### Generate call back function file as follows:

~~~~~~~~~~~{.c}
#include "opencore_algo_support.h"
static struct recognition_udg gs_rpt_buf;
static int udg_algo_exec(void** sensor_data, feed_general_t* feed)
{
    int ret = 0;
    int idx = GetDemandIdx(feed, SENSOR_ACCELEROMETER);
    common_data_buf_t* pdata;

    if(idx >= 0 && idx < PHY_SENSOR_SUM)
    {
        pdata = (common_data_buf_t *)sensor_data[idx];
        printk("%d,%d,%d\n",pdata->x, pdata->y, pdata->z);
    }
    return ret;
}
static int udg_algo_init(feed_general_t* feed_ptr)
{
    return 0;
}
static int udg_algo_goto_idle(feed_general_t* feed_ptr)
{
    return 0;
}
static int udg_algo_out_idle(feed_general_t* feed_ptr)
{
    return 0;
}
sensor_data_demand_t    atlsp_algoF_sensor_demand[]=
{
    {
        .type = SENSOR_ACCELEROMETER,
        .freq = 100,
        .rt = 1000,
    }
};
static feed_general_t atlsp_algoF=
{
    .type = BASIC_ALGO_UDG,
    .demand = atlsp_algoF_sensor_demand,
    .demand_length = sizeof(atlsp_algoF_sensor_demand)/sizeof(sensor_data_demand_t),
    .ctl_api = {
        .init = &udg_algo_init,
        .exec = &udg_algo_exec,
        .goto_idle = &udg_algo_goto_idle,
        .out_idle = &udg_algo_out_idle,
    },
};
define_feedinit(atlsp_algoF);
static exposed_sensor_t udg_sensor=
{
    .depend_flag = 1 << BASIC_ALGO_UDG,
    .type = SENSOR_ALGO_UDG,
    .rpt_data_buf = (void*)&gs_rpt_buf,
    .rpt_data_buf_len = sizeof(struct recognition_udg),
};
define_exposedinit(udg_sensor);
~~~~~~~~~~~

The variables and function names should be customized according your algorithms.
You can also refer to other existing algorithms for this file.

#### Add new sensor scanning/sensor data subscribing/sensor data processing code in QRK main.c

Receive sensor data:

~~~~~~~~~~~{.c}
......
case SENSOR_PATTERN_MATCHING_GESTURE: {
    struct gs_personalize {
        short size;
        short nClassLabel;
        short reserved;
    }__packed;

    struct gs_personalize *p = (struct gs_personalize *) p_data_header->data;
    pr_info(LOG_MODULE_MAIN, "GESTURE=%d,size=%d", p->nClassLabel, p->size);
    sensing_callback(0, p->nClassLabel);
}
break;
case SENSOR_ALGO_UDG: {
    struct recognition_udg *p = (struct recognition_udg *) p_data_header->data;
    pr_info(LOG_MODULE_MAIN, "UDG GESTURE=%d,size=%d", p->nClassLabel, p->size);
    sensing_callback(0, p->nClassLabel);
}
break;
......
~~~~~~~~~~~

Start sensor data subscribing:

~~~~~~~~~~~{.c}
......
case SENSOR_PATTERN_MATCHING_GESTURE:
    ss_sensor_subscribe_data(sensor_handle, NULL, p_evt->handle,
                             ACCEL_DATA, 100, 10);
    break;
case SENSOR_ALGO_UDG:
    ss_sensor_subscribe_data(sensor_handle, NULL, p_evt->handle,
                             ACCEL_DATA, 100, 10);
    break;
......
~~~~~~~~~~~

Start sensor scanning:

~~~~~~~~~~~{.c}
......
case ARC_SC_SVC_ID:
    sensor_handle = req->client_handle;
    ss_start_sensor_scanning(sensor_handle, NULL,
                            PATTERN_MATCHING_TYPE_MASK
                            | TAPPING_TYPE_MASK
                            | STEPCOUNTER_TYPE_MASK
                            | ALGO_UDG_MASK);
    break;
......
~~~~~~~~~~~

#### Update Kconfig and Kbuild.mk

Once the callback function file is added in algo_support_src folder,
the Kbuild.mk file should be updated

~~~~~~~~~~~
obj-$(CONFIG_SENSOR_CORE_ALGO_TAPPING) += opencore_tapping.o
obj-$(CONFIG_SENSOR_CORE_ALGO_SIMPLEGES) += opencore_simpleges.o
obj-$(CONFIG_SENSOR_CORE_ALGO_DEMO) += opencore_demo.o
obj-$(CONFIG_SENSOR_CORE_ALGO_OPENCOREUDG) += opencore_udg.o
~~~~~~~~~~~

The configuration flag should be added in defconfig_crb:

~~~~~~~~~~~
CONFIG_SERVICES_SENSOR_IMPL=y
CONFIG_SENSOR_CORE_ALGO_UDG=y
CONFIG_SENSOR_CORE_ALGO_OPENCOREUDG=y
~~~~~~~~~~~

The configuration flag should also be added in Kconfig:

~~~~~~~~~~~
......
config SENSOR_CORE_ALGO_UDG
    bool "User Defined Gesture"
    depends on HAS_PATTERN_MATCHING
    select SENSOR_CORE_ALGO_COMMON
    select PATTERN_MATCHING_DRV

config SENSOR_CORE_ALGO_OPENCOREUDG
    bool "UDG Non Pattern Matching"
    select SENSOR_CORE_ALGO_COMMON
......
~~~~~~~~~~~

@anchor pattern_matching_api
# Pattern Matching API

Use a hardware neural-network classifier.
It supports 128 Neurons, with 128 components for each Neuron.

As Pattern Matching is implemented in hardware, it can process pattern recognition
algorithms in parallel.

## How to integrate new physical sensor for sensor core to pool

#### Add new member to type_convert_array
See in opencore_support.c: type_convert_array[PHY_SENSOR_SUM],
the example of SENSOR_MAGNETOMETER:

~~~~~~~~~~~{.c}
phy_sensor_type_convert_t type_convert_array[PHY_SENSOR_SUM] =
{
    {
      .ss_type = SENSOR_MAGNETOMETER,
      .sensor_data_frame_size = sizeof(struct mag_phy_data)
    }
};
~~~~~~~~~~~

#### Modify the Macro value of PHY_SENSOR_SUM

It is located in opencore_algo_common.h: _make the previous value to plus 1_.

~~~~~~~~~~~{.c}
#define PHY_SENSOR_SUM 3
~~~~~~~~~~~

#### Add the new type to enum ss_sensor_type_t.

It is located in sensor_data_format.h.

####  Add the data struct of new physical sensor.

See the example of SENSOR_MAGNETOMETER in sensor_data_format.h:

~~~~~~~~~~~{.c}
struct mag_phy_data
{
    int mx;
    int my;
    int mz;
}__packed;
~~~~~~~~~~~


## How to start calibration for raw sensor data

for example as calibration of accelerometer raw sensor data:

1) Start sensor scanning for accel sensor by inputing < ss startsc ACCEL> at test_cmd interface.

2) Start sensor subscribe for accel sensor by inputing <ss sbc ACCEL 1 1000> at test_cmd interface.

3) Start sensor calibration to be ready for calibration by inputing <ss clb start 0 ACCEL> at test_cmd interface. It will begin to output accel raw sensor data from test_cmd interface.

4) According to the raw sensor data, put the board in proper position and fasten it to make sure it could be into no motion status.

	In terms of proper position, handle the board as below:

	In case of accelerometer calibration, make the target axle vertical to horizon line and move softly until the target value reach the maximum value.

	In case of gyroscope calibration, just make the board in no motion status.

5) After the board into no motion status for a while, start to get the calibraion result data by inputing <ss clb get 0 ACCEL> at test_cmd interface. It will go on outputing raw sensor data and a set of calibration offset xyz. And we need to remember them. After a while, the outputing raw data will be change to the correctted value and stored into sensor_core system. During get calibration result process, make sure the board is in no motion status all the time.

	In case of accelerometer calibration, need to take turns to process step 4 and step 5 to calibrate all the three axles.

6) Set calibration by inputing < ss clb set 0 ACCEL x y z> at test_cmd interface, these x/y/z were descided by step 4 and step 5. And these data will be stored into flash.

7)(optional) Stop calibration by inputing <ss clb stop 0 ACCELEROMETER> at test_cmd interface.

8) When reboot next time, the stored calibraion result data will be set automaticly.

Note: just support calibration of both accelerometer and gyroscope raw sensor data in open sensor core v1 currently.

@}

