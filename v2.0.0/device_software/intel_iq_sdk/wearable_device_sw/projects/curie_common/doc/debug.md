Debug / Logs Display {#curie_debug}
====================

## Debug
Support is only for Quark for now.

Two ways : either using the console, or using IDE you wish.

You can replace the ELF with app (firmware) below.


### Start server and console client

    make debug_console ELF=out/current/firmware/bootloader_quark.elf

### Start server and use IDE you wish

    make debug_start ELF=out/current/firmware/bootloader_quark.elf

## Logs Display
### Install screen tool

    sudo apt-get install screen

### Launch command

    screen /dev/ttyUSB1 115200

### Output examples

    420748A000:20:02.378 a8009ae4 id: 2001 src: 5[cpu:0] dst: 8[cpu:1] type: REQ|    MAIN| A000:20:02.385 a8010330 id: 2041 src: 8[cpu:1] dst: 5[cpu:0] type: RSP
    INFO|Toggle GPIO
    420749|    MAIN|    INFO|L000:20:02.376 a8009ae4 id: 2001 src: 5[cpu:0] dst: 8[cpu:1] type: REQ
    420759|    MAIN|    INFO|L000:20:02.394 a8010330 id: 2041 src: 8[cpu:1] dst: 5[cpu:0] type: RSP
    testFiber: Hello !!! [3000]
    testFiber: Hello !!! [3000]

### Exit screen tool
Kill the log window with the command:

    CTRL^A K
