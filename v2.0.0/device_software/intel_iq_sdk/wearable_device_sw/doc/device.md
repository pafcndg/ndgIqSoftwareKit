@addtogroup infra_device
@{

# Platform Device Tree
## Device Tree Structure

The device tree is built upon the device structure. Devices can have a parent
device. For example a SPI device will usually have the SPI Bus device as parent.

This parent-child relationship is used to indicate a dependency between child
and parent, i.e. the SPI bus must be initialized before the child SPI device.
Conversely, a child SPI device must be suspended before its parent bus.

![Device tree example structure](device_tree.png "Device tree structure")

## Device Tree Declaration


The device tree is declared only once and cannot be modified afterward (it does
not support device hotplug). The user must declare one array of device*
containing all the devices of the platform.
The ordering of this array is very important as it must respect
initialization order. For example, a spi_flash device must be positioned after
the spi bus device using the following code:

~~~~~{.c}
// SPI Bus device
struct device my_spi_bus = {
	.id = SBA_SOC_SPI_0_ID,
	.driver = &my_spi_driver,
	.priv = NULL,
};

struct device my_spi_device = {
	.id = SPI_DEVICE1D,
	.driver = &my_spi_device1_driver,
	.parent = &my_spi_bus,
};

static struct device *all_platform_devices[] = {
	&my_spi_bus,
	&my_spi_device,
};
~~~~~

## Implementing a Device Driver

When implementing a new device driver, the developer has to use the
@ref driver structures.

This structure contains three hooks that can be implemented if needed:
- init hook is called at platform init
- suspend hook is called when suspend/shutdown is requested
- resume hook is called to resume device after a deep sleep

The @ref driver structure also contain a _priv_ private
field. It is commonly used to store a pointer to a structure that handles the
device data.
This allows the developer to point to a custom structure, so that the driver
can access custom data.

Configuration data can be set when the structure is declared.

Here is an example for an UART driver:
~~~~~{.c}
	{
		.id = UART0_PM_ID,
		.driver = &ns16550_pm_driver,
		.priv = &(struct ns16550_pm_device) {
			.uart_num = 0,
			.init_info =  &(struct uart_init_info) {
				.regs= COM1_BASE_ADRS,
				.sys_clk_freq = UART_XTAL_FREQ,
				.baud_rate = COM1_BAUD_RATE,
				.options = 0,
				.irq = COM1_INT_LVL,
				.int_pri = COM1_INT_PRI,
			},
		},
	},
~~~~~

## Device Power Management

Each device has a powerstate field. This is used to track device power
state, defined in the @ref PM_POWERSTATE enum, from infra/pm.h file.

A device starts in PM_NOT_INIT state. PM_RUNNING is the nominal state, when
device is working fine. PM_SUSPENDED indicates that device is ready for deep
sleep mode. PM_SHUTDOWN indicates that device is stopped and a platform reset is
needed to restart it.

### Device Initialization

The device tree is also used to init all devices. If a device fails to init, an
error message is output and the function will cause a *panic*.

For a simple platform which uses only gpio and spi_device1 devices, the init
workflow is:
\msc
 user,device_API,gpio,spi_bus,spi_device1;
 |||;
 user box device_API [label="platform devices are declared"];
 user=>device_API [label="init_devices", URL="@ref init_devices"];
 device_API box spi_device1 [label="All devices are initialized by iterating the device tree"];
 device_API=>gpio [label="init_device"];
 gpio box gpio [label="init gpio device"];
 device_API<<gpio;
 device_API=>spi_bus [label="init_spi_bus_device"];
 spi_bus box spi_bus [label="init spi_bus device"];
 device_API<<spi_bus;
 device_API=>spi_device1 [label="init spi_device1, only if spi_bus is init", URL="@ref init_devices"];
 spi_device1 box spi_device1 [label="init spi_device1 device"];
 device_API<<spi_device1;
 user<<device_API;
\endmsc

### Device Resume

@ref resume_devices is very similar to @ref init_devices, except it calls resume
hook in devices. If a device fails to resume, the call will *panic* to prevent
unexpected behavior.

### Device Suspend

The @ref suspend_devices function works like @ref resume_devices, but in the
opposite order. child devices are suspended before parent devices:

The workflow is:
\msc
 user,device_API,gpio,spi_bus,spi_device1;
 |||;
 user box device_API [label="platform devices and bus devices are running"];
 user=>device_API [label="suspend_devices", URL="@ref suspend_devices"];
 device_API=>spi_device1 [label="suspend spi_device1", URL="@ref init_devices"];
 spi_device1 box spi_device1 [label="suspend spi_device1 device"];
 device_API<<spi_device1;
 device_API=>spi_bus [label="suspend spi_bus_device, only if all SPI devices are suspended"];
 spi_bus box spi_bus [label="suspend spi_bus device"];
 device_API<<spi_bus;
 device_API=>gpio [label="suspend_device"];
 gpio box gpio [label="suspend gpio device"];
 device_API<<gpio;
 user<<device_API;
\endmsc

Note that if a device fails to suspend, platform is resumed.

### Device Shutdown

@ref suspend_devices needs a state parameter, from @ref PM_POWERSTATE enum. If
this state is equal to @ref PM_SHUTDOWN, then the device manager will not resume
devices in case of device suspend failure, and will continue to suspend the other
devices. An error notification is returned for each device that fails to
shutdown.

# Working with Devices

Each device should be defined as _extern_ in the file _soc_config.h_. They also
have a unique device ID for display, for example.

Driver APIs should use the extern declaration of a device to use it.

Because the _init_ function will panic if there is an error during
initialization, all the devices *are* initialized correctly when the application
is running.
