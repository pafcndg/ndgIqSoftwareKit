
********************************************************************************
                                  ATTENTION
********************************************************************************

This version of OpenOCD is to be used for Intel internal debugging/development/
evaluation purposes only. It can't be shared with external customers - even
with NDA/CNDA - without Legal approval.

Do NOT redistribute the fileset, instead please redirect the access requests to
the SSG/DPD/ECDL OpenOCD Team:

 ivan.de.cesaris@intel.com
 jessica.gomez.hernandez@intel.com
 stefano.dellosa@intel.com

********************************************************************************
                                  ATTENTION
********************************************************************************


Instructions:
1. Install your Flyswatter2 probe drivers:
   Windows:
    + Plug in your probe to the host
    + Run "zadig_2.1.2.exe".
    + Options > List all devices
    + Select your probe (Flyswatter2) and install WinUSB
        driver for Interface 0 and Interface 1.
    + Replug your probe.
   Linux:
    + Create a text file in the rules directory:
          $ sudo vim /etc/udev/rules.d/99-openocd.rules

      With the following content:
          SUBSYSTEM=="usb", ATTR{idVendor}=="0403", ATTR{idProduct}=="6010", MODE="0666"
          SUBSYSTEM=="usb", ATTR{idVendor}=="15ba", ATTR{idProduct}=="002b", MODE="0666"

    + Plug in your probe to the host

2. Launch OpenOCD:
    + Open a CMD terminal and switch into the folder where is located.
    + Call OpenOCD as follows:
    bin\openocd.exe -f scripts\interface\ftdi\flyswatter2.cfg -f scripts\board\firestarter.cfg

    Configurations details:
    firestarter.cfg     - ARC + LMT
    firestarter-arc.cfg - ARC only
    firestarter-lmt.cfg - LMT only

3. Connect OpenOCD:
    + Open a new terminal or putty and connect it through telnet:
        telnet localhost 4444

    + Load your binaries - is recommended to use ARC, because memory access is faster:
        > load_image path_to_file.bin address       (write binary into memory)
        > verify_image path_to_file.bin address     (check loaded binary)
        Example:
           load_image C:\\data\\FSRom.bin 0xffffe000
           verify_image C:\\data\\FSRom.bin 0xffffe000

           ARC: 0x40000000 sss_fw.bin | nanokernel.bin
           LMT: 0x40030000 fst_elf_ia.com | NODE1.bin

        Helper utilities:
            > clk32M                (stabilize SoC clock to 32MHz Si)
            > mass_erase            (erases ROM + flash0 + flash1)
            > del_flash0            (deletes flash0)
            > del_flash1            (deletes flash0)

    + To reset your target:
        > reset                     (triggers a warm reset and keep running)
        > reset halt                (triggers a warm reset and stop at reset vector)

    + If using dual configuration(ARC+LMT), you can use the following commands:
        > targets                   (state of the current cores)
        > targets 0                 (select arc core)
        > targets 1                 (select lmt core)

        > enable_x_arc2lmt_trigger  (halt arc when lmt halts)
        > enable_x_lmt2arc_trigger  (halt arc when lmt halts)
        > enable_x_dual_trigger     (halt both cores when one stops)
        > disable_x_trigger         (halt cores independently)
        > resume_all                (set both cores to run)

    + Cheat sheet for OpenOCD:
        > step                      (instruction step)
        > resume                    (set core to run)
        > halt                      (stops core)

        > bp <address> <length>     (set SW breakpoint)
        > bp <address> <length> hw  (set HW breakpoint)

        > mww <address> <value>     (write word into memory)
        > mdw <address> <count>     (read count time words from start address)

        > reg [name]                (displays value of registers)
        > adapter_khz [kHz]         (change JTAG speed)
        > help [keyword]            (displays available commands)
        * Please check that there might be target specific commands, use keyboard = arc | x86_32.
        Example: ARC - breakpoints and access to auxiliary memory.

4. Connect GDB:
    Depending on the configuration you will need to connect to a specific port.
    Single target configurations (ARC / LMT) answer at port 3333, when in the dual
    target configuration the first target is ARC at port 3333, the second LMR at port 3334.

    All commands mentioned in the OpenOCD part, can me triggered from gdb using the word "monitor"

    To debug the ARC core using the provided "firestarter-arc.cfg" configuration the sequence will look like:
    + ARC: "gdb-arc.exe"
      Example:
        (gdb)set arch EM
        (gdb)target remote 127.0.0.1:3333
        (gdb)monitor mww 0xa8000000 0x4000ba40
        (gdb)monitor gdb_breakpoint_override hard
        (gdb)set directories D:\\2014_09_26-ww39.6\\peg_fs-sensor_fw\\apps
        (gdb)symbol-file D:\\2014_09_26-ww39.6\\binary\\sss_fw.elf


    To debug the LMT core using the provided "firestarter-lmt.cfg" configuration
    the sequence will look like:
    + LMT: "gdb-ia.exe"
      Example:
        (gdb)target remote 127.0.0.1:3333
        (gdb)monitor gdb_breakpoint_override hard
        (gdb)monitor reset halt
        (gdb)flushregs
        (gdb)set directories D:\\2014_09_26-ww39.6\\peg_fs-lmt_core_fw
        (gdb)symbol-file D:\\2014_09_26-ww39.6\\binary\\fst_elf_ia.elf
        (gdb)b main


    For the adventurous using the provided "firestarter.cfg" dual target configuration
    the sequence will look like:
    + ARC: "gdb-arc.exe"
      Example:
        (gdb)set arch EM
        (gdb)target remote 127.0.0.1:3333
        (gdb)monitor mww 0xa8000000 0x4000ba40
        (gdb)monitor gdb_breakpoint_override hard
        (gdb)set directories D:\\2014_09_26-ww39.6\\peg_fs-sensor_fw\\apps
        (gdb)symbol-file D:\\2014_09_26-ww39.6\\binary\\sss_fw.elf

    + LMT: "gdb-ia.exe"
      Example:
        (gdb)target remote 127.0.0.1:3334
        (gdb)monitor gdb_breakpoint_override hard
        (gdb)monitor reset halt
        (gdb)flushregs
        (gdb)set directories D:\\2014_09_26-ww39.6\\peg_fs-lmt_core_fw
        (gdb)symbol-file D:\\2014_09_26-ww39.6\\binary\\fst_elf_ia.elf
        (gdb)b main

    + Xtrigger of halt/run events between the two cores, by default is disable.
      If you would like to activate it, please choose accordingly:
      Example:
        (gdb)monitor enable_x_arc2lmt_trigger
        (gdb)monitor enable_x_lmt2arc_trigger
        (gdb)monitor enable_x_dual_trigger
        (gdb)monitor disable_x_trigger
        (gdb)monitor resume_all

5. SW breakpoints in flash:
    Be aware that:
      + Setting a breakpoint will have to overwrite a full flash page (2KB).
      + Stepping/resuming over breakpoints needs to write a page twice.
      + Flashing speed depends on the JTAG speed (adapter_khz) and the SoC Flash Controller Speed.
      + By default GDB set/remove all breakpoints every time you resume/halt execution.
      + Reset will remove all breakpoints to restore memory, but if you terminate to debug session
         without deleting breakpoints, they are going to remain in memory.

    In other words:
      The more SW breakpoints you have, the slower your debug session may get, use them wisely!
      However, this commands would make your debug much easier.
      Example:
        (gdb)monitor gdb_breakpoint_override disable
        (gdb)set breakpoint always-inserted on
        (gdb)set remotetimeout 30

