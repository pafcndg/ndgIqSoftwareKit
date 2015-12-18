@page memory_partitioning Memory partitioning

Memory Partitioning
===================

Quark SE main processors (Quark and ARC) are sharing the same on-die non-volatile memory for code/data and SRAM for volatile data, and optionally an SPI flash for storing user-data.
BLE chip has its own dedicated non-volatile memory and SRAM.

## Quark SE
### Non Volatile Memory Partitions
#### OTP Partitions
Partition | Purpose | Firmware Update Behaviour | Factory Reset Behaviour
:--- | --- | --- | --- |
OTP Code/Data | The one-time-programming memory area for boot code/data| Can't be updated (1) | Can't be erased(1)
Intel-OTP Provisioning Data | Contains data that identifies the Intel&reg; Curie&trade; Module, Keys... and in general all data that shall not be updatable (@ref oem_data)| Can't be updated (1) | Can't be erased(1)
Customer-OTP Provisioning Data | Contains data that identifies the Customer product, Keys... and in general all data that shall not be updatable (@ref customer_data)| Can't be updated (1) | Can't be erased(1)

#### Non-Volatile-Memory Partitions
Partition | Purpose | Firmware Update Behaviour | Factory Reset Behaviour
:--- | --- | --- | --- |
Bootloader 	| It holds bootloader code / data | Not updated	| Not updated
Recovery	| Not yet supported		| | |
Firmware	| Application/services/os/drivers code&data (both for ARC and Quark)	| Updated	| Not updated
Factory Reset Persistent Runtime Data	| Partition to store data that are updated at runtime by the device, accessible over Properties Storage Service, but not reset during Factory Reset procedure	| Not updated	| Erased
Factory Reset Non Persistent Runtime Data	| Partition to store data that are updated at runtime by the device, accessible over Properties Storage Service, but reset during Factory Reset procedure	| Not updated	| Not erased
Factory Settings	| Contains factory default settings	| Not updated	| Not erased
UserData (0..n)	| User-Data partition (multiple different partitions can be supported)	| Not updated	| Erased


(1) Can't be erased/updated if the OTP bit has been set

#### Memory layout without external SPI flash

![memory layout without external spi flash Figure](memory_layout_without_external_spi_flash.png "Memory layout without external spi flash")

HW Config Block (first part of the firmware flash block)
 - Start address of ARC code
 - Start address of the context restoration procedure (for wake from deep sleep) for ARC
 - Start address of QRK Code
 - Start address of the context restoration procedure (for wake from deep sleep) for QRK
 - SHA1 of the eventual part of FW that goes beyond 192kB

#### Memory layout with external SPI flash

![memory layout with external spi flash Figure](memory_layout_with_external_spi_flash.png "Memory layout with external spi flash")

HW Config Block (first part of the firmware flash block)
 - Start address of ARC code
 - Start address of the context restoration procedure (for wake from deep sleep) for ARC
 - Start address of QRK Code
 - Start address of the context restoration procedure (for wake from deep sleep) for QRK
