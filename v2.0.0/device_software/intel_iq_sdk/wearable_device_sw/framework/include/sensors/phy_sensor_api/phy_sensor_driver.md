@addtogroup phy_sensor_drv_api
@{
##How to write sensor driver in physical sensor layer
Physical sensor layer provides generic physical sensor APIs to access physical sensors.
Physical sensor layer also defines physical sensor driver interface.
Sensor driver authors need to implement physical sensor driver interfaces, so that physical sensors can be accessed via physical sensor APIs.

###Physical Sensor Driver Data Structure
struct @ref phy_sensor_t\n
This structure defines attributes and interfaces for a physical sensor driver.\n
You can embed phy_sensor_t into another data structure to define your own driver structrue.\n
Attributes sensor driver author must initialize of phy_sensor_t:
- phy_sensor_t.type: Specify the type of the physiscal sensor. See pre-defined types of physical sensor @ref ss_sensor_type_t.\n
- phy_sensor_t.hw_fifo_len: If your sensor supports hardware fifo, driver author need to init this filed (in byte) according to hardware.\n
- phy_sensor_t.raw_data_len: The size of sensor sample data in byte report in physical sensor layer.

phy_sensor_api_t defined a set of interfaces for physical sensor driver. It is the member of struct phy_sensor_t.
Sensor driver need to implement some/all of the interfaces and initialize phy_sensor_t.api properly.\n
Interfaces sensor driver author need to implement:

- phy_sensor_api_t.open:\n
The interface will be called by @ref phy_sensor_open.\n
Some initialization work can be done here.\n
See return type @ref DRIVER_API_RC (Mandatory).

- phy_sensor_api_t.close:\n
The interface will be called by @ref phy_sensor_close.\n
After this is called, the sensor is no longer needed by anyone.\n
You can do cleanup work here. (Mandatory)

- phy_sensor_api_t.activate:\n
Activate or deactivate the sensor.\n
The interface will be called by @ref phy_sensor_enable.\n
See return type @ref DRIVER_API_RC (Optional)

- phy_sensor_api_t.set_odr:\n
Set output data rate of the sensor. The unit is 0.1HZ.\n
You should implement this interface if the sensor need to set up output data rate to work.\n
See return type @ref DRIVER_API_RC (Optional)

- phy_sensor_api_t.query_odr:\n
Provide the Output Data Rate the sensor support according to the input.\n
Usually, return the minimal output data rate the sensor support that greater or equal to the output data rate queried.\n
The interface will be called by @ref phy_sensor_query_odr_value.\n
If phy_sensor_api_t.set_odr is implemented, you should implement this interface too.\n
See return type @ref DRIVER_API_RC (Optional)

- phy_sensor_api_t.read:\n
Read one sensor sample.\n
If something is wrong or nothing read, just return 0. Else, return the actual length of a sensor sample.\n
The interface will be called by @ref phy_sensor_data_read in polling mode.(Optional)

- phy_sensor_api_t.fifo_read:\n
Read one or multiple sensor samples from hardware fifo.\n
The interface will be called by @ref phy_sensor_fifo_read in polling mode.\n
If something is wrong or nothing read, just return 0. Else, return the actual length of a sensor samples.\n
If the sensor support hardware fifo, the driver should implement this interface. (Optional)

- phy_sensor_api_t.enable_int_mode:\n
Enable or disable data report in interrupt mode.\n
If enable is TRUE, the interface is called by @ref phy_sensor_data_register_callback. Sensor data will be reported in interrupt mode.\n
If enable is FALSE, the interface is called by @ref phy_sensor_data_unregister_callback. Sensor data will not be reported via interrupt.\n
If hardware fifo is enabled, the driver should report fifo data in interrupt mode.\n
If hardware fifo is disabled or not supported, the driver should report sensor data according the the data ready interrupt of sensor.\n
Sensor driver should to implement this interface if the sensor supports interrupt for data report.\n
See callback function type @ref generic_read_data_cb. Sensor driver need to call such a callback function to report sensor data.\n
See return type @ref DRIVER_API_RC (Optional)
@note Besides sensor data, sensor event can be reported via the callback function. See pre-defined sensor event @ref phy_sensor_event.\n

- phy_sensor_api_t.get_fifo_len:\n
Get the hardware fifo length if the sensor supports it.\n
If @ref phy_sensor_t.raw_data_len is not initialized, @ref phy_sensor_data_get_hwfifo_length will call this interface to get fifo length. (Optional)

- phy_sensor_api_t.enable_fifo:\n
Enable sensor hardware fifo.\n
The interface will be called by @ref phy_sensor_enable_hwfifo.\n
If the sensor support hardware fifo, the driver should implement this interface.\n
See return type @ref DRIVER_API_RC (Optional)

- phy_sensor_api_t.selftest:\n
Sensor selftest interface.\n
See return type @ref DRIVER_API_RC (Optional)

- phy_sensor_api_t.get_property:\n
Get the value of specific property of sensor.\n
The interface will be called by @ref phy_sensor_get_property.\n
All property can't get by other physical sensor driver interface can be got via this interface.\n
See pre-defined properties for physical sensor @ref phy_sensor_property_type_t.\n
See return type @ref DRIVER_API_RC (Optional)

- phy_sensor_api_t.set_property:\n
Set the value of specific property of sensor.\n
The interface will be called by @ref phy_sensor_set_property.\n
All property can't set by other physical sensor driver interface can be set via this interface.\n
See pre-defined properties for physical sensor @ref phy_sensor_property_type_t.\n
See return type @ref DRIVER_API_RC (Optional)


###Register Sensor driver
sensor driver can use @ref sensor_register to register sensor driver into system.\n
Thunderdome defined a device driver management structure struct @ref driver\n

	struct xxx_sensor_drv_t {
		struct phy_sensor_t sensor;
		uint8_t xxx_sensor_specific;
	};

	struct xxx_sensor_drv_t xxx_drv = {
		.sensor = {
					...
				  },
		.xxx_sensor_specific = xxxxx,
	};

	static int xxx_init(struct device *dev)
	{
		int ret;
		ret = sensor_register(&xxx_drv.sensor); /* use sensor_register to register your driver */
		//do other things if needed
		...
		return ret;
	}

	struct driver xxx_driver = {
		.init = xxx_init,
		.suspend = NULL,
		.resume = NULL
	};

@ref driver.init will be called during system initialization.\n
So, you need to implement 'init' interface of struct @ref driver to register your sensor driver to sensor subsystem.\n

###Power Management
System can enter/quit deep sleep state. The device driver management structure struct @ref driver also provide the interfaces for device driver.
Power management framework will call @ref driver.suspend when system are going to enter deep sleep state. It will call @ref driver.resume when system is woken up by specific events. See @ref infra_pm for more information about power management.
@note When driver.suspend or driver.resume is called, interrupt is disabled. So any operation depending on interrupt cannot exist in these two interfaces.

###Serial BUS
Typically, sensors are connected via i2c or spi bus.\n
Thunderdome provides Serial Bus Access APIs, see @ref sba.\n
Sensor driver will need use these APIs to communicate via i2c or spi.\n

###GPIO for interrupt pins
If your sensor is using GPIO pin(s) for interrupt, you need use APIs provided by @ref gpio_arc_driver or/and @ref soc_gpio.

###Interrupt handling
- ISR context part:\n
In ISR context, interrupt is disabled, so i2c or spi operation cannot be finished.\n
It is recommended to use @ref workqueue to queue the actual workload which will be executed in fiber context.\n
- Workqueue part:\n
Workqueue execute works in fiber context. You can use APIs provided by @ref workqueue to delay your actual workload in workqueue.

###An example of physical sensor driver structure(Accelerometer)

	struct xxx_sensor_drv_t {
		struct phy_sensor_t sensor;	/* struct phy_sensor_t embedded to customized driver structure */
		uint8_t xxx_type;
	};

	struct xxx_sensor_drv_t xxx_accel = {
		.sensor = {
					.type = SENSOR_ACCELEROMETER,
					.raw_data_len = sizeof(phy_accel_data_t),
					.hw_fifo_len = 1024,
					.api = {
							.open = xxx_open,
							.close = xxx_close,
							.query_odr = xxx_query_odr,
							.set_odr = xxx_set_odr,
							.enable_int_mode = xxx_enable_int_mode,
							.get_fifo_len = xxx_get_fifo_len,
							.enable_fifo = xxx_enable_fifo,
							.read = xxx_read_data,
							.fifo_read = xxx_read_fifo,
							.set_property = xxx_set_property,
							},
				},
		.xxx_type = XXXXXX_SENSOR_ACCEL,
	};
@}
