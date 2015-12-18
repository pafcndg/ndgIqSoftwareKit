@page build_kbuild Project source tree

The SDK uses a recursive build system inspired by Linux Kernel's
[Kbuild](https://www.kernel.org/doc/Documentation/kbuild/makefiles.txt).

The SDK expects to find the project source tree at a location pointed by the
PROJECT_PATH environment variable (defined in the makefile).

This directory contains:
- the optional project root Kbuild.mk.
- the optional project specific configuration files (see @ref build_config).

Kconfig files are required only if you want to add custom configuration rules.
Kbuild.mk is required only if you want to have some of your projects files built with the SDK build system.

All project-specific sources that need to be compiled must be available from the
root Kbuild.mk file, possibly conditioned by the
[configuration flags](@ref build_config) you have defined.

Example:

~~~~~~~~~~
.
├── Kbuild.mk
├── project.Kconfig
├── services.Kconfig
├── drivers.Kconfig
├── arc
│   ├── Kbuild.mk
│   └── main.c
└── quark
│   ├── Kbuild.mk
    ├── ble_app.c
    ├── ble_app.h
    └── main.c
~~~~~~~~~~