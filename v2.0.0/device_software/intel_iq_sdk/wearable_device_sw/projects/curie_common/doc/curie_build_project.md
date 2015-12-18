@page curie_build_project Building a project using the Intel&reg; Curie&trade; SDK

Assuming all [host prerequisites](@ref build_one_time_setup) have been
fulfilled, the process of creating a firmware always includes the following
steps:

1. Setup
2. Configuration (optional)
3. Build

Most of these steps actually correspond to GNU make targets that need to be
invoked in sequence, as described in the following paragraphs.

## Setup

~~~~~~~~~
cd /path/to/project
make setup
~~~~~~~~~
or
~~~~~~~~~
make setup BUILDVARIANT=xx BOARD=yy
~~~~~~~~~
Where xx is _debug_ or _release_, and yy is the name of a board.

The SDK setup will create the build output directory and prepare the
environment:
- compile host tools, and
- save setup parameters.

Several build environments can coexist under a project tree, all located under
the "./out" directory.
The naming convention for a specific build output directory is:

    ./out/PROJECT_BOARD_BUILDVARIANT

The current environment (the last setup command) is identified using
a symbolic link.

Example:

    out
     ├── reference_ctb_release
     ├── reference_ctb_debug
     ├── current -> /home/user/quark_se/out/reference_ctb_release
     └── host_tools

@warning After making a _make clean_, you need to call _make setup_ again.

## Configuration

The SDK uses a set of Kconfig-based rules to select features to be included in
a build, filtered out by the capabilities of the selected hardware.

Whenever the project build configuration needs to evolve or when you
want to test a specific configuration, you can customize the build using the
Kconfig front-end tool.

~~~~~~~~~
make XX_menuconfig
~~~~~~~~~

Where X is one of:
quark (core), arc (sensors), ble_core (ble), bootupdater

Example:

~~~~~~~~~
make quark_menuconfig
~~~~~~~~~

Custom configurations can be saved and reused using the following commands:

~~~~~~~~~
make TARGET_savedefconfig DEFCONFIG=/path/to/defconfig
~~~~~~~~~

These options can be extended by the project with custom Kconfig files located
in the [project source tree](@ref build_kbuild).

Refer to @ref build_config for a detailed explanation of how to define a
custom build configuration.


## Build

~~~~~~~~~
make package
~~~~~~~~~

This will build the project and generate a flashable package. Refer to
the [flashing guide](@ref flashing) to flash this package on the target.
