@addtogroup test_command_syntax
@{

## Test Commands Description

Here is a list of all test commands.

### Engineering Test Commands

**Power Management:**

~~~~~~~~
system deepsleep <auto_wake_time>
~~~~~~~~
Put the board into deepsleep:
   - auto_wake_time: Wakeup time in ms

~~~~~~~~
system idle
~~~~~~~~
Enable deepsleep when system is idle

**RTC:**

~~~~~~~~
rtc set <time>
~~~~~~~~
Set initial RTC time:
   - time: Configuration value for the 32bit RTC value (timestamp)

~~~~~~~~
rtc read
~~~~~~~~
Read current RTC

~~~~~~~~
rtc alarm <rtc_alarm_time>
~~~~~~~~
Set RTC Alarm time, and start RTC:
   - rtc_alarm_time: Configuration value for the 32bit RTC alarm value (in s)

~~~~~~~~
rtc status
~~~~~~~~
Get status of RTC(not config, running, finished)

**Pulse Width Modulation:**

~~~~~~~~
pwm conf <channel> <frequency> <unit> <duty_cycle>
~~~~~~~~
Configure a channel of PWM:
   - channel: PWM channel number [0-3]
   - frequency: Frequency
   - unit: mHz|Hz|kHz
   - duty_cycle: PWM duty cycle in nanoseconds (time high)

~~~~~~~~
pwm start <channel>
~~~~~~~~
Start a channel of PWM:
   - channel: PWM channel number

~~~~~~~~
pwm stop <channel>
~~~~~~~~
Stop a channel of PWM:
   - channel: PWM channel number

**Logger:**

~~~~~~~~
log list
~~~~~~~~
List modules

**Panic:**

~~~~~~~~
debug panic <panic_id>
~~~~~~~~
Panic generator:
   - panic_id: 0 - Division by 0, 1 - Unaligned access, 2 - Watchdog panic
     (timeout 2,097s), 3 - Invalid adress, 4 - User panic, 5 - Stack Overflow,
     6 - Invalid OpCode

**Factory:**

~~~~~~~~
factory get
~~~~~~~~
Display factory data

~~~~~~~~
factory end <mode>
~~~~~~~~
Set factory end of production mode:
   - mode: oem|customer

~~~~~~~~
factory lock <uuid>
~~~~~~~~
Disable jtag port and rom write access:
   - uuid: uuid must be equal to the one written on rom

**Sensors:**

~~~~~~~~
ss startsc <sensor_type>
~~~~~~~~
Start scanning a sensor:
   - sensor_type: GESTURE|TAPPING|SIMPLEGES|STEPCOUNTER|ACTIVITY|ACCELEROMETER|GYROSCOPE

~~~~~~~~
ss stopsc <sensor_type>
~~~~~~~~
Stop scanning a sensor
   - sensor_type: GESTURE|TAPPING|SIMPLEGES|STEPCOUNTER|ACTIVITY|ACCELEROMETER|GYROSCOPE

~~~~~~~~
ss sbc <sensor_type> <sampling_freq> <reporting_interval>
~~~~~~~~
Set the parameter of sensor subscribing data:
   - sensor_type: GESTURE|TAPPING|SIMPLEGES|STEPCOUNTER|ACTIVITY|ACCELEROMETER|GYROSCOPE
   - sampling_freq: Samling frequency
   - reporting_interval: Reporting interval

~~~~~~~~
ss unsbc <sensor_type>
~~~~~~~~
Unset the parameter of sensor subscribing data:
   - sensor_type: GESTURE|TAPPING|SIMPLEGES|STEPCOUNTER|ACTIVITY|ACCELEROMETER|GYROSCOPE

~~~~~~~~
ss clb <clb_cmd> <sensor_type> <calibration_type> <data>
~~~~~~~~
Operation the calibration of sensors:
   - clb_cmd: start/get/stop/set
   - sensor_type: GESTURE|TAPPING|SIMPLEGES|STEPCOUNTER|ACTIVITY|ACCELEROMETER|GYROSCOPE
   - calibration_type: specific type of different sensors,it is 0 for On_board sensors
   - data: data for clb_cmd=set

**BLE:**

~~~~~~~~
ble enable <enable_flag> <mode> [name]
~~~~~~~~
Enable BLE:
   - enable_flag: 0 - disable; 1 - enable;
   - mode:        0, 1 (mode = 0 - Normal Mode; mode = 1 - Test Mode)
   - [name]:        Local Name of the Local BLE device
   - Example 1: ble enable 1 0 AppCTB - enables BLE with name = "AppCTB" in
     normal mode
   - Example 2: ble enable 1 0 - enables BLE in normal mode (test mode OFF)
   - Example 3: ble enable 0 - disables BLE
   - Example 4: ble enable 1 1 "DtmCTB"- enable BLE test mode (dtm) with name =
     "DtmCTB"

~~~~~~~~
ble tx_test <operation> <freq> <len> <pattern>
~~~~~~~~
Start BLE dtm tx:
   - operation: start|stop
   - freq: actual frequency = 2024 + `<freq>` x 2Mhz, `<freq>` < 40
   - len : Payload length
   - pattern: Packet type
   - Example 1: ble tx_test start 20 4 0
       pattern = 0 - PRBS9, 1 - 0X0F, 2 - 0X55, 0xFFFFFFFF - Vendor Specific
   - Example 2: ble tx_test stop

~~~~~~~~
ble rx_test <operation> <freq>
~~~~~~~~
Start BLE dtm rx:
   - operation: start|stop
   - freq: 2024 + freq x 2Mhz, freq < 40
   - Example 1: ble rx_test start 20
   - Example 2: ble rx_test stop

~~~~~~~~
ble test_set_tx_pwr <dbm>
~~~~~~~~
Set BLE tx power:
   - dbm: -60dBm...+4dBm
   - Example: BLE test_set_tx_pwr -16

~~~~~~~~
ble tx_carrier <operation> <freq>
~~~~~~~~
Start tx carrier test:
   - operation: start|stop
   - freq: actual frequency = 2024 + `<freq>` x 2Mhz, `<freq>` < 40
   - Example 1: ble tx_carrier start 10
   - Example 2: ble tx_carrier stop

~~~~~~~~
ble set_name <name>
~~~~~~~~
Set BLE device name:
   - Example: ble set_name TestCTB
   - the name will be used as BLE name for the device
   - the name set with this command is persistent after reboot

~~~~~~~~
ble set_value <conn_ref> <s_uuid> <c_uuid> <val> <base>
~~~~~~~~
Set a characteristic value:
   - conn_ref - connection reference (can be 0 if disconnected)
   - s_uuid   - UUID of the service
   - c_uuid   - UUID of the characteristic
   - val      - value to set
   - base     - h|d (h for hexadecimal, d for decimal)

Example: ble set_val 0x40003000 180f 2a19 64 h
   - 0x40003000 - connection reference (can be 0 if disconnected)
   - 180f       - UUID of the Battery service
   - 2a19       - UUID of the Battery level characteristic
   - 64         - hexa value of 100% battery level
   - h          - hexa type for value=64 (use d for decimal)

At the present time, the s_uuid and c_uuid are ignored and only the
battery level characteristic value is set.

~~~~~~~~
ble advertise <operation> [<advertise_option_bits>] [name]
~~~~~~~~
Configure the BLE advertisement:
   - operation: start|stop
   - Advertise option bits selects the interval and timeout as defined at: @ref BLE_ADV_OPTIONS
   - Bit 0: Slow advertisement interval
   - Bit 1: Ultra fast advertisement interval
   - Bit 2: Short advertisement timeout
   - Bit 3: NO advertisement timeout
   - Bit 4: Non-discoverable advertisement, minimum advertisement data
   - The default option is fast advertisement and default timeout
   - name: advertised name

~~~~~~~~
ble key <conn_ref> <pass_key>
~~~~~~~~
Return a passkey if required by the bonding:
   - conn_ref - connection reference
   - pass_key - six digits key to return

~~~~~~~~
ble discover <conn_ref> <type> <uuid> [start_handle] [end_handle]
~~~~~~~~
Discover the attributes on a connected device:
   - conn_ref - connection reference
   - type     - type of the discovery (0 = primary, 1 = secondary, 2 = included, 3 = characteristic, 4 = descriptor)
   - uuid     - 16-bit UUID to discover
   - start_handle - optional start attribute handle for the discovery
   - end_handle - optional end attribute handle for the discovery

Example: ble discover 0xa0004000 0 0x180f
   - 0xa0004000 is the connection reference displayed in logs when remote device connected
   - 1 is to retrieve primary service
   - 0x180f is battery service UUID

~~~~~~~~
ble write <conn_ref> <handle> <offset> <value>
~~~~~~~~
Write an attribute on a connected device:
   - conn_ref - connection reference
   - handle   - handle of the attribute to write
   - offset   - offset at which to write
   - value    - 32-bit value to write

Example: ble write 0xa0004000 14 0xAA
   - 0xa0004000 is the connection reference displayed in logs when remote device connected
   - 14 is characteristic handle
   - 0xAA is value to write

~~~~~~~~
ble read <conn_ref> <handle> <offset>
~~~~~~~~
Read an attribute on a connected device:
   - conn_ref - connection reference
   - handle   - handle of the attribute to read
   - offset   - offset at which to read

Example: ble read 0xa0004000 14 0
   - 0xa0004000 is the connection reference displayed in logs when remote device connected
   - 14 is characteristic handle
   - 0 is the offset

~~~~~~~~
ble connect <type> <address>
~~~~~~~~
Initiate a connection as central to a remote peripheral:
   - type    - Address type (0 = public, 1 = random)
   - address - Address (format = AA BB CC DD EE FF)

Example: ble connect 0 AA BB CC DD EE FF

~~~~~~~~
ble disconnect <conn_ref>
~~~~~~~~
Disconnect from a connected device:
   - conn_ref - connection reference

~~~~~~~~
ble clear
~~~~~~~~
Clear the BLE bonding information

~~~~~~~~
ble subscribe <conn_ref> <ccc_handle> <value> <value_handle>
~~~~~~~~
Subscribe to a characteristic on a connected device:
   - conn_ref     - connection reference
   - ccc_handle   - handle of the Client Characteritic Configuration attribute
   - value        - value to write
   - value_handle - handle of the Characteritic Value attribute

Example: ble subscribe 0xa0004000 3 1 2

~~~~~~~~
ble unsubscribe <conn_ref> <subscribe_ref>
~~~~~~~~
Unsubscribe from a characteristic on a connected device:
   - conn_ref      - connection reference
   - subscribe_ref - subscription reference returned at subscribe

Example: ble unsubscribe 0xa0004000 0xa0001209


**CFW Message Injection:**

~~~~~~~~
cfw inject <message>
~~~~~~~~
Send a cfw message:
   - message: Message to send (in hexadecimal)

~~~~~~~~
cwf poll
~~~~~~~~
Read a cfw message

**SPI Flash Test:**

~~~~~~~~
spi_flash run
~~~~~~~~
Run the spi flash tests (erase, write, read)

**Slaves:**

~~~~~~~~
tcmd slaves
~~~~~~~~
List registered slaves

**Version:**

~~~~~~~~
tcmd version
~~~~~~~~
Get the tcmd 'engine' version

**Memory:**

~~~~~~~~
dbg pool
~~~~~~~~
Get statistics on memory pool usage

**Battery:**

~~~~~~~~
battery cycle
~~~~~~~~
Get battery charge cycle

### Not engineering Test Commands

**BLE:**

~~~~~~~~
ble info
~~~~~~~~
Get BLE related information.
Currently supported infos:
   - BDA and current name

~~~~~~~~
ble version
~~~~~~~~
Get BLE controller version.

**SPI:**

~~~~~~~~
spi tx <bus_id> <slave_addr> <len> <byte0> [<byte1>... <byte_n>]
~~~~~~~~
Send data on a SPI port:
   - bus_id: spim0|spim1|spis0|ss_spim0|ss_spim1 @ref SBA_BUSID
   - slave_addr: Chip select @ref SPI_SLAVE_ENABLE
   - len: Length of the write buffer
   - byte0 [byte_n]: Bytes to send

~~~~~~~~
spi rx <bus_id> <slave_addr> <len>
~~~~~~~~
Receive data from a SPI port:
   - bus_id: spim0|spim1|spis0|ss_spim0|ss_spim1 @ref SBA_BUSID
   - slave_addr: Chip select @ref SPI_SLAVE_ENABLE
   - len: Length of the read buffer

~~~~~~~~
spi trx <bus_id> <slave_addr> <len_to_write> [<byte0> <byte1>... <byte_n>] <len_to_read>
~~~~~~~~
Send data on a SPI port:
   - bus_id: spim0|spim1|spis0|ss_spim0|ss_spim1 @ref SBA_BUSID
   - slave_addr: Chip select @ref SPI_SLAVE_ENABLE
   - len_to_write: Length of the write buffer
   - byte0 [byte_n]: Bytes to send
   - len_to_read: Length of the read buffer

**GPIO:**

~~~~~~~~
[arc.]gpio conf <soc|aon|8b0|8b1> <index> <mode>
~~~~~~~~
Configure SOC/AON port GPIO:
   - soc|aon|8b0|8b1: Port to config
   - index: Index of the gpio
   - mode: in|out

~~~~~~~~
[arc.]gpio get <soc|aon|8b0|8b1> <index>
~~~~~~~~
Get SOC/AON port GPIO:
   - soc|aon|8b0|8b1: Port to get
   - index: Index of the gpio

~~~~~~~~
[arc.]gpio set <soc|aon|8b0|8b1> <index> <val>
~~~~~~~~
Set SOC/AON port GPIO:
   - soc|aon|8b0|8b1: Port to set
   - index: Index of the gpio
   - val: 0|1 values to be setted

**Power Management:**

~~~~~~~~
system power_off
~~~~~~~~
Power off the board

~~~~~~~~
system shutdown <shutdown_mode>
~~~~~~~~
Shutdown the board:
   - shutdown_mode: 0 - SHUTDOWN, 1 - CRITICAL_SHUTDOWN, 2 - THERMAL_SHUTDOWN,
     3 - BATTERY_SHUTDOWN, 4 - REBOOT

~~~~~~~~
system reset <reboot_mode>
~~~~~~~~
Reboot the board:
   - reboot_mode: 0 - MAIN, 1 - CHARGING, 2 - WIRELESS_CHARGING, 3 - RECOVERY,
     4 - FLASHING, 5 - FACTORY, 6 - OTA, 7 - DTM, 8 - CERTIFICATION,
     9 - RESERVED_0, 10 - APP_1, 11 - APP_2, 12 - RESERVED_1, 13 - RESERVED_2,
     14 - RESERVED_3, 15 - RESERVED_4

**I2C:**

~~~~~~~~
[arc.]i2c write <bus_id> <slave_addr> <register> [value1... valueN]
~~~~~~~~
Start tx:
   - bus_id: i2c0|i2c1|ss_i2c0|ss_i2c1
   - slave_addr: Address of the slave
   - register: Slave register number
   - value1> [value2... valueN]: Value to write

~~~~~~~~
[arc.]i2c probe <bus_id> <slave_addr>
~~~~~~~~
Probe a device:
   - bus_id: i2c0|i2c1|ss_i2c0|ss_i2c1 @ref SBA_BUSID
   - slave_addr: Address of the slave

~~~~~~~~
[arc.]i2c read <bus_id> <slave_addr> <register> <len>
~~~~~~~~
Read data:
   - bus_id: i2c0|i2c1|ss_i2c0|ss_i2c1 @ref SBA_BUSID
   - slave_addr: I2C address if configured as a Slave
   - register: Slave register number
   - len: Length of the read buffer

~~~~~~~~
[arc.]i2c rx <bus_id> <slave_addr> <len>
~~~~~~~~
RX data:
   - bus_id: i2c0|i2c1|ss_i2c0|ss_i2c1 @ref SBA_BUSID
   - slave_addr: I2C address if configured as a Slave
   - len: Length of the read buffer

**ADC:**

~~~~~~~~
adc get <channel>
~~~~~~~~
Get the channel value:
   - channel: ADC channel

**Memory:**

~~~~~~~~
mem read <size> <addr>
~~~~~~~~
Read on a given address:
   - size: Size to read (in byte : 1|2|4)
   - addr: Address to read (in hexadecimal)
   - Example: mem read 4 0xA8000010 - Read the 4 bytes at 0xA8000010 address

~~~~~~~~
mem write <size> <addr> <val>
~~~~~~~~
Write given value to a given address:
   - size: Size to write (in byte : 1|2|4)
   - addr: Address to write (in hexadecimal)
   - val: Value to write
   - Example: mem write 4 0xA8000010 0x51EEb001 - Write 0x51EEb001 at 0xA8000010
     address

**Logger:**

~~~~~~~~
log set <level> or log set <module>
~~~~~~~~
Set an action:
   - module: 0, 1, 2, ...
   - Example 1: log set 1 1 - enable module 1
   - Example 2: log set 2 0 - disable module 2
   - Example 3: log set 1 - set log level to WARNING

~~~~~~~~
log setbackend [none|uart*|usb]
~~~~~~~~
Gets current log backend set and sets logging to specified backend
   - Example 1: log setbackend - gets the current backend set
   - Example 2: log setbackend usb - sets log backend to usb

**Battery:**

~~~~~~~~
battery level
~~~~~~~~
Get battery level

~~~~~~~~
battery status
~~~~~~~~
Get battery status

~~~~~~~~
battery vbatt
~~~~~~~~
Get battery voltage

~~~~~~~~
battery temperature
~~~~~~~~
Get battery temperature

~~~~~~~~
battery period vbatt|temp <period_second>
~~~~~~~~
Set ADC measure interval:
   - period_second: If is equal to 0, corresponding measure is suspended

~~~~~~~~
charger status
~~~~~~~~
Get charger status

~~~~~~~~
charger type
~~~~~~~~
Get charger type

**Version:**

~~~~~~~~
version get
~~~~~~~~
Get the binary version header that allows to uniquely identify the binary used

@}
