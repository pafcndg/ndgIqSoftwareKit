@page curie_create_project Create a new project based on Intel&reg; Curie&trade;

The Intel&reg; Curie&trade; SDK already implements most generic build targets
required to create Intel&reg; Curie&trade;-based products.

As a consequence, the process of creating a project is greatly simplified: as a
matter of fact, the only requirement to create a minimal Makefile for a
Intel&reg; Curie&trade;-based project is to include the
Intel&reg; Curie&trade; common targets sub-makefile and to define a few
variables.

The <i>projects/curie_common/curie_common_targets.mk</i> file contains all minimal
targets required to build a Intel&reg; Curie&trade;-based product:
- bootloader,
- bootupdater,
- charging OS,
- main core image,
- sensor core image,
- bluetooth core image.

It MUST be included in the project Makefile, after having defined the following
variables:
- PROJECT, BOARD, BUILDVARIANT, as defined in the
  [generic build process](@ref curie_build_project),
- QUARK_DEFCONFIG, ARC_DEFCONFIG, BLE_CORE_DEFCONFIG, that should point to
  valid [build configurations](@ref build_config) for the main, sensor and
  bluetooth cores.

Example projects can be found under the projects directory. Here is the content
of a Makefile:

~~~~~~~~~~
PROJECT      = curie_reference
BOARD        ?= ctb
BUILDVARIANT ?= debug

QUARK_DEFCONFIG ?= $(PROJECT_PATH)/quark/defconfig
ARC_DEFCONFIG ?= $(PROJECT_PATH)/arc/defconfig
BLE_CORE_DEFCONFIG ?= $(PROJECT_PATH)/ble_core/defconfig

include ../curie_common/build/curie_common_targets.mk
~~~~~~~~~~

The project-specific sources are not listed in the project Makefile, but should
instead be organized according to a generic @ref build_kbuild, which root folder
MUST be the directory that contains the project Makefile.

The Intel&reg; Curie&trade; SDK already includes several generic boards, drivers
and services, but your project can add custom items using optional
[Kconfig](@ref build_config) extension files.

Example:

~~~~~~~~~~
.
├── Kbuild.mk
├── project.Kconfig
├── services.Kconfig
├── drivers.Kconfig
├── arc
│   ├── Kbuild.mk
│   ├── arc_defconfig_ctb
│   ├── arc_defconfig_crb
│   └── main.c
├── quark
│   ├── Kbuild.mk
│   ├── quark_defconfig_ctb
│   ├── quark_defconfig_crb
│   ├── ble_app.c
│   ├── ble_app.h
│   └── main.c
├── ble_core
│   └── ble_core_defconfig
└── Makefile
~~~~~~~~~~
