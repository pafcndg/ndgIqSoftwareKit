Flashing {#flashing}
========

## Users Prerequisites

- Install Flash Tool. (links below)
- Install NDG Tools. (links below)
- If it's the first time you flash this board, please
[change FTDI descriptor with FT_Prog](@ref change_ftdi_usb) (must be done on
Windows).


    WARNING: Installation order is very important to have correct external tool path

## Developers Prerequisites

    make help
    make check_host_setup
    sudo make one_time_setup


#### In both cases (developers and users), unplug and plug usb cables to allow udev applying rules on usb devices.


## Intel Tools Packages

Latest versions to be used for all Intel Projects are present [here] (http://goto.intel.com/flashing) in "NDG tools packages" paragraph.


## How to Flash with Flash Tool GUI

[Flashing with Flash Tool](@ref flash_local_file)


## Flashing from Development Environment

    make package flash FLASH_CONFIG=(configuration)

#### Recommended Linux Command to Flash total package

    make package flash

## Specifics

 - **Which USB port to use?** Refer to [HW Setup](@ref hw_setup)
 - **OpenOCD shall not be installed with apt-get install as official version
 does not have support for Quark SE SOC. Use Intel Tools package**
 - **FTDI USB descriptor has to be updated to allow enumeration with
 FlashTool (only one time by board) :** [How to change FTDI USB descriptor on Quark SE for Intel&reg; Curie&trade; Test Board hardware ?](@ref change_ftdi_usb)
 - **Mac setup :** [How to flash Quark SE from Mac ?](@ref flash_for_mac)

### Infos
#### JTAG based FLASH_CONFIG

JTAG based flash configuration | Description
------------------------------ | -----------
jtag_x86_rom+bootloader        | Flash rom and bootloader

#### USB based FLASH_CONFIG

Note : available 10s after boot.

USB based flash configuration | Description
----------------------------- | -----------
usb_full                      | Flash applications on Intel&reg; Curie&trade; hardware

More configurations are available for advanced users from cmdline.
