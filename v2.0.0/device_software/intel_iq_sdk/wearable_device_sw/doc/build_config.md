@page build_config Defining a custom build configuration

The SDK exposes a set of features to be selected based on the actual hardware
capabilities of each platform.

The consistency of build configurations is guaranteed by rules written in the
[Kconfig language](https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt).

## Step-by-step basic configuration

The build configuration can be customized through a _curses_ interface launched
by the menuconfig target:

    make TARGET_menuconfig

Where TARGET is project-specific.

Main keystrokes used in the interface:
- Left, Right, Up, Down : navigate,
- y: select an option,
- n: unselect an option,
- Space: toggle an option,
- Enter: activate commands at the bottom,
- ?: Get help on the current symbol,
- /: Search a symbol.

### Select a board

The first thing you need to specify when creating a custom configuration is the
board you will use under the top-level "Board" menu.

If your board is not listed under this menu, then you need to define it in your
project (see the corresponding paragraph below).

Some hardware characteristics of the board may be optional though, like the
support of some peripherals on serial buses (Led, Haptic, ...).
Such optional hardware components are usually located under the Drivers menu.

### Select a processor core

The second step is to select the core to which this configuration applies under
the "Core" module.

If your core is not listed here, then you're out of luck (for now), as you
probably need a custom toolchain to generate the firmware, and the SDK doesn't
support external toolchains yet.

### Select an Operating System

In the third step, you select an OS from the "Operating System" menu.

The recommended OS for the SDK is Zephyr, which comes in two flavors: Micro or
Nano (please refer to Zephyr documentation for details).

### Select services

Probably the most important step: select the @ref services you want to activate
under the "Services" menu.

When selecting a service, you are given the choice to select:
- a client (API) module,
- a server (Implementation) module.

API modules can be compiled on every target, but some services implementations
are only available on some cores due to specific hardware dependencies (as a
matter of fact, selecting a service implementation will automatically pull the
driver dependencies to your build configuration).

Needless to say, it is up to you to make sure that if you select a service API
on one core, you need to make sure the corresponding implementation is available
somewhere on another core.

### Select Drivers

The Drivers menu contains a limited number of optional hardware features and all
the drivers available for a given board/core.

Drivers are typically selected automatically by the services that need them, but
you can still select them manually under the Drivers menu.

This is specially useful if you are using a modified setup including optional
hardware components.

Imagine for instance that you are using a "bare" board on which you have
connected a display.

Since the display is not available on all configurations for this board, you
need to select the "Display" hardware feature manually so that you can
activate services that need a display. You also need to select the driver
corresponding to your display.

### Further customizations

The last step consists in customizing the common features the SDK provides (most
of them are located under the "Infra" menu):

- log backends,
- @ref infra_tcmd,
- ...

## Advanced configuration

### Configuration patterns

The SDK build configuration rules roughly follow a common pattern involving
four types of entities:
- Features
- Services
- Drivers
- Boards

#### Features

Features represent hardware capabilities of the target, like USB support, the
ability to control SPI peripherals, an embedded Flash, ...

Features are described in build configurations using two flags:
- one HAS_XX flag to indicate that a feature is available,
- one XX flag to indicate that a feature is selected.

Example:

	HAS_USB, USB
	HAS_FLASH, FLASH

Generic features can be split into more specific features that implement them
to offer more flexibility.

Example

	config HAS_SPI_FLASH
		select HAS_FLASH

#### Services

Services represent high-level functional capabilities of the target, like
battery status or sensors.
Services can typically be distributed on multiple cores.

Services are usually described in build configurations using two flags:
- SERVICES_XX selects the service API
- SERVICES_XX_IMPL selects the service implementation.

Services implementations typically depend on the availability of specific
hardware features, and will select them automatically if they are available.

Example:

	config SERVICES_STORAGE_IMPL
		depends on HAS_FLASH
		select SPI_FLASH if HAS_SPI_FLASH
		select SOC_FLASH if HAS_SOC_FLASH

### Drivers

Drivers represent the piece of software giving access to a specific hardware
feature.

Drivers are described by flags corresponding the component they provide access
to.

Drivers are typically dependent on the availability of the feature they give
access to.

Example:

	config SPI_FLASH_MX25U12835F
		bool "SPI NOR Flash Macronix MX25U12835F"
		depends on HAS_SPI_FLASH

#### Boards

Boards are meta configuration flags used to simplify build configuration for the
same hardware platform.

Boards will typically declare hardware features availability and select the
corresponding driver when the feature is selected.

Example:

	config BOARD_CURIE_CRB
		...
		select HAS_SPI_FLASH if QUARK
		select SPI_FLASH_MX25U12835F if SPI_FLASH

#### All pieces put together

According to the set of rules defined in the previous paragraphs as examples,
the following base configuration:

    CONFIG_BOARD_CURIE_CRB=y
    CONFIG_SERVICES_STORAGE_IMPL=y

Will produce the following expanded configuration:

    CONFIG_BOARD_CURIE_CRB=y
    CONFIG_SERVICES_STORAGE_IMPL=y
    CONFIG_HAS_FLASH=y
	CONFIG_HAS_SPI_FLASH=y
	CONFIG_SPI_FLASH=y
	CONFIG_SPI_FLASH_MX25U12835F=y

Because:
- the board will advertize the availability of a SPI flash,
- that provides the flash feature,
- allowing the storage service implementation to be selected,
- in turn selecting the SPI flash feature,
- telling the board to select its SPI flash driver.

### Extending the build configuration

The SDK supports extending the build configuration in several ways.

#### Adding a new board definition

When parsing board rules, the SDK will look for a boards.Kconfig file at the
root of your project directory.

Any board defined in this file will be automatically added to the "Board" menu.

To define a board, you need first to create a configuration entry for it and
select its characteristics:
- the cores it contains,
- the hardware features available for each core,
- the drivers to use to activate a hardware feature.

Note that the SDK comes with SoC or hardware modules descriptions that can be
reused to create new boards.

Example:

    config BOARD_CURIE_WITH_DISPLAY
		bool "A curie Test board with a display"
		select CURIE
		select HAS_DISPLAY
		select ULPT43PT if DISPLAY

The second thing you need to define is where the SDK can find the board
"machine" header for each core. This is achieved by overriding the generic
CONFIG_MACHINE_INCLUDE_PATH option.

You can take advantage of the predefined PROJECT_PATH and CORE_NAME variables to
specify this path in a generic way.

Example:

	if BOARD_CURIE_WITH_DISPLAY

	config MACHINE_INCLUDE_PATH
		string
		default "$(PROJECT_PATH)/machine/$(CORE_NAME)"

	endif

Any board-specific code you need to add must be available somewhere in the
@ref build_kbuild.

#### Adding a new service

When parsing service rules, the SDK will look for a services.Kconfig file at the
root of your project directory.

Any service defined in this file will be automatically added to the "Services"
menu.

To define a service, you just need to create a configuration entry for it.
It is nevertheless advised to split the service between a client (API) and a
server (Implementation).

As specified before, services implementations typically depend on the
availability of hardware features and activate them if they are present.

Example:

    menu "Heart rate feedback panel"

    config SERVICE_HEART_RATE_PANEL
		bool "Client"

    config SERVICE_HEART_RATE_PANEL_IMPL
		bool "Server"
		depends on HAS_DISPLAY
		select DISPLAY

The actual source code for this new service must be available somewhere in the
@ref build_kbuild.

The newly added service shall be enabled using the project build configuration procedure.

#### Adding a new driver

When parsing driver rules, the SDK will look for a drivers.Kconfig file at the
root of your project directory.

Any driver defined in this file will be automatically added to the "Drivers"
menu.

To define a driver, you just need to create a configuration entry for it.

Example:

	config ULPT43PT
		bool "Display driver for ULPT43"

The actual source code for this new driver must be available somewhere in the
@ref build_kbuild.

The newly added driver shall be enabled using the project build configuration procedure.

#### Adding other project specific flags and rules

For any other flag than boards, services or drivers, the SDK will look for a
project.Kconfig file at the root of your project directory.

You can add any new configuration items you may require there, including for
instance subprojects selectors to select the application code (the "main") that
should be used for each core.

All these new items will be located at the bottom of the front configuration
page. It is advised to gather all your items in a menu and giving it a
meaningful title.

Example:

	menu "Project XX"
	...
	config ...

	endmenu