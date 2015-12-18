@page system_architecture System architecture

System Software Architecture
============================

The Intel&reg; Curie&trade; hardware module embeds Quark SE system-on-chip, Bluetooth Low Energy chip, Battery Charger, Accelerometer/Gyroscope and Crystals.

Following is describing the Thunderdome software stack deployed on this module.

# High Level Architecture

The three subsystems (Quark (QRK), ARC and BLE) are exposing:
- Local APIs that can be called only within the same processor
- Services that can be called from all susbsystems thanks to the underlying thunderdome framework


![System Software Architecture Figure](system_software_architecture.png "System Software Architecture")

## Quark SE SoC

The Quark is the main processor inside Quark SE and it is in charge of executing device end-user application.
It provides following:
- UI (User Interaction) Service for managing LEDs, Buttons, Haptics
- Storage Service for managing embedded Quark SE on-die non-volatile memory and SPI flash if present
- BLE Service for managing BLE chip and exposing supported Bluetooth Services and Profiles
- Battery Service for managing the battery state of charge estimation and charging state
- GPIO Service for managing Quark accessible GPIOs
- Power Management Infrastructure APIs for managing power states of the Quark, and the Intel&reg; Curie&trade; module (active, sleep, off)
- USB Infrastructure APIs for managing USB
- Debug/Panic/Logging APIs


All services are running under the supervision of the Service Manager on top of the Zephyr RTOS, infrastructure software components and peripherals drivers. Details on each service, infrastructure components and drivers are provided in the SDK documentation.

Zephyr RTOS is configured in multi-threaded mode.

## ARC
ARC is the sensor subsystem and it is in charge of handling all sensors and ARC-only resources.
- Sensors Service & Sensor Core Service for managing all sensors and all the sensor related algorithms (in the scope of Intel&reg; Curie&trade; module, it handles the gyroscope/accelerometer)
- ADC Service for managing ADCs
- Sensors Subsystem GPIO Service for managing dedicated Sensors Subsystem GPIOs
- Power Management Infrastructure APIs for managing power states of the ARC
- Debug/Panic/Logging APIs

All services are running under the supervision of the Service Manager Proxy on top of the Zephyr RTOS, infrastructure software components and peripherals drivers. Details on each service, infrastructure components and drivers are provided in the SDK documentation.

Zephyr RTOS is configured in single-threaded mode

## BLE
BLE chip is managing Bluetooth related functions and is exposing a GATT(Server/Client)/GAP Service to Quark SE.

## Design / Architecture Constraints
### ARC specific peripherals (Sensor Subsystem)
Sensor subsystem peripherals are only accessible from the ARC core
- ADC is only available to the ARC core => so in order to get ADC values from QRK, it
is required to use the ADC service.
- I2C and SPI busses connected to the Sensor Subsystem cannot be accessed by the
Quark, and thus sensors connected to this bus will only be available through
the sensor services.

### Fabric peripherals

The resources that are on the fabric are hardware accessible by both cores, however those resources have to be
statically bound in software to one or the other core.

As a consequence, the driver of the peripheral is only accessible in the core it has been bound to.

If the peripheral needs to be used by the other core, it shall be done with the use of a service. i.e. GPIO, ADC.

The binding consists in instantiating right sofware drivers on the right cores at board initialization time.

# Security Aspects
The hardware provides the capability to lock both the 8KB OTP region and the JTAG.

The OTP bit is programmed by writing '0' to bit 0 of address 0 in the OTP region.

Once this bit is set to '0', the hardware prevents:
- All writes to the OTP region
- All accesses to the JTAG

<b>The OTP bit shall not be programmed in a debug environment as it is not possible to revert it and then not possible to re-enable the JTAG.</b>
