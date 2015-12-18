Thunderdome Developer's Documentation {#thunderdome_reference_doc}
=====================================

Thunderdome is a cross-project framework for embedded devices which provides:
 - [Framework](@ref cfw) : a high-level component framework for creating and
   interacting with services
 - [Services](@ref services)
    - [BLE & BLE GATT/GAP Service](@ref ble_service)
    - [Sensors Service](@ref SS_SERVICE)
    - [User Interaction Service](@ref ui_service)
    - [Battery Service](@ref battery_service)
    - [Storage Service](@ref ll_storage_service)
    - [Properties Storage Service](@ref properties_service)
    - [ADC Service](@ref adc_service)
    - [GPIO Service](@ref gpio_service)
 - [Infrastructure](@ref infra)
    - [IPC](@ref ipc)
    - [Power Management](@ref infra_pm)
    - [Driver model](@ref infra_device)
    - [Logging](@ref infra_log) / [Panic](@ref infra_panic)
    - [Factory Data](@ref infra_factory_data)
    - [OS abstraction layer](@ref os) allowing to run the same code on several
      operating systems
    - [Test Command Framework](@ref infra_tcmd)
 - [Drivers](@ref drivers)
 - [Build](@ref curie_build_project)
 - How-Tos:
    - [How to add/build a service/driver/board ?](@ref build_config)
    - [How to add a new algorithm in sensors framework ?](@ref how_to_add_new_sensor_algorithm)
    - How to customize delivered platform for a product?
        - [Battery](@ref how_to_customize_battery_properties)
        - SOC Pin Muxing in bsp/src/machine/soc/intel/`<soc>`/`<master_core>`/pinmux.c
        - Storage Configuration in bsp/src/machine/soc/intel/`<soc>`/`<master_core>`/soc_config.c

## Get Started Coding

 - [coding style](@ref coding_style): Before submitting code, make sure it
   complies with our coding style rules
