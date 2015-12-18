Flashing with Flash Tool {#flash_local_file}
==============================

- With Intel Flash Tool, in "Flash" tab, select your flash image zip (1).
- Select "jtag_rom+bootloader" in "Configuration" (2).
- Once selected, you can flash it using "Start to flash" button (3) (on the
FireStarter Hardware).

![Intel Flash Tool](@ref flash_local_file.png)

Finally, Select "usb_full" in "Configuration" (1) and flash using "Start to
flash" button (2).

    WARNING: You must flash with "usb_full" in less than 10 seconds after the
    first flash. If you're not sure about timing, you can push the reset button
    and flash with "usb_full" (less than 10 seconds after the reset).

![Intel Flash Tool](@ref flash_local_file_2.png)
