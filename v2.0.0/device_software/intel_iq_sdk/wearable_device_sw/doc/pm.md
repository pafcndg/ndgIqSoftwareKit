@addtogroup infra_pm
@{

# Platform Power Management

The power management main principle for the platform is to go to lowest power
consumption state as soon as possible. For independent nodes, this can be done
transparently.

As an example, the BLE chip will be going to its lowest power consumption mode
as soon as possible, but will always be able to execute requests from the main
SoC by a control line that will make the BLE chip to wake up prior to the
request to be sent.

For the Quark SE Soc, Deep Sleep is the lowest power consumption state.
In this state only the RAM content is preserved. Both the ARC and the QRK
CPUs are sharing the same system constraints, and both will be shut down in
deep sleep cycle.

This implies the need to synchronize both the QRK and the ARC for transitioning
to the deep sleep state.

## Power Management Tools

### Wakelocks

Wakelocks are used to prevent the platform to go into deep sleep. Everyone can
use them, including drivers, services and users.

When a user wants to prevent the platform to go into deep sleep mode, he needs to
acquire a wakelock. This is useful for critical tasks, like flash memory
operations.

The user may then release the wakelock as soon as possible to unlock the deep
sleep mode again.

The user can register a unique callback which is called when last wakelock is
released. The power manager is currently using this callback, so a user
callback can be overwritten at any time (it's not safe to use it).

Each wakelocks has a unique ID on a specific core, which is used for debug
purposes when a wakelock times out (task/driver failed to release it).

When a suspend request is received by a node, it will check that all wakelocks
are released before processing this request:
- For deep sleep requests, node will send an error code and cancel this request.
- For shutdown requests, node will block and wait for all wakelocks to be
  released.


#### Initialize Wakelocks

\msc
 user,API;
 |||;
 user=>API [label="pm_wakelock_init_mgr", URL="\ref pm_wakelock_init_mgr"];
 user<<API;
 user box API [label="Wakelock manager is now ready"];
 |||;
 user=>API [label="pm_wakelock_init(&wl0, 0)", URL="\ref pm_wakelock_init"];
 user=>API [label="pm_wakelock_init(&wl1, 1)", URL="\ref pm_wakelock_init"];
\endmsc


#### Example of Wakelock Usage

\msc
 user,API,callback,log;
 |||;
 user box API [label="Wakelocks are ready"];
 |||;
 user=>API [label="pm_wakelock_acquire(&wl0)", URL="\ref pm_wakelock_acquire"];
 user<<API;
 user=>API [label="pm_wakelock_acquire(&wl1)", URL="\ref pm_wakelock_acquire"];
 user<<API;

 user box API [label="Wait for 100ms"];
 API=>log [label="WARN: wakelock 0 expired"];
 user=>API [label="pm_wakelock_release(&wl1)", URL="\ref pm_wakelock_release"];
 API=>>callback [label="all wakelock released", URL="\ref pm_wakelock_set_list_empty_cb"];
 user<<API;
\endmsc

#### Infinite Wakelocks

Current wakelock implementation also supports infinite wakelocks. These
wakelocks can prevent deep sleep mode forever and must be used with caution
(the platform shutdown/deepsleep is impossible if an infinite wakelock is currently
acquired).

A warning log is printed if an infinite wakelock is held too long (each
WAKELOCK_MAX_TIMEOUT). An infinite wakelock is acquired when using
WAKELOCK_FOREVER parameter as timeout.

\msc
 user,API,callback,log;
 |||;
 user box API [label="Wakelocks are ready"];
 |||;
 user=>API [label="pm_wakelock_acquire(&wl0, WAKELOCK_FOREVER)", URL="\ref pm_wakelock_acquire"];
 user<<API;
 user box API [label="Wait for 5000 ms (WAKELOCK_MAX_TIMEOUT)"];
 API=>log [label="WARN: infinite wakelock 0 hold for 5000 ms"];
 user box API [label="Wait for 5000 ms (WAKELOCK_MAX_TIMEOUT)"];
 API=>log [label="WARN: infinite wakelock 0 hold for 5000 ms"];
 user=>API [label="pm_wakelock_release(&wl0)", URL="\ref pm_wakelock_release"];
 API=>>callback [label="all wakelock released", URL="\ref pm_wakelock_set_list_empty_cb"];
 user<<API;
\endmsc

### Suspend Blockers

Suspend blockers are another mechanism to prevent deep sleep/shutdown modes.
This consists of an array of callbacks, which check if deep sleep/shutdown is
allowed.

A suspend blocker callback takes as parameter a device and a power state to go
to (deep sleep or shutdown).

If a callback returns false, then deep sleep/shutdown is not allowed.
This is used, for example, to prevent the platform to go to deep sleep mode if USB
is plugged, but to allow shutdown.

Suspend blockers are defined in soc_config.c file:
~~~~~{.c}
DECLARE_SUSPEND_BLOCKERS(
    {
		.cb = usb_pm_is_suspend_allowed,
		.dev_id = USB_PM_ID
    },
    /* etc ... */
);
~~~~~

#### Suspend Blockers Use Case

\msc
 user,API,usb_pm;
 |||;
 user box API [label="deep sleep request"];
 user=>API [label="pm_check_suspend_blockers(PM_SUSPENDED)", URL="\ref pm_check_suspend_blockers"];
 API=>usb_pm [label="usb_pm_is_suspend_allowed(USB_PM_ID, PM_SUSPENDED)", URL="\ref usb_pm_is_suspend_allowed"];
 API<<usb_pm [label="true/false"];
 API box usb_pm [label="check other suspend blockers"];
 user<<API [label="true/false"];
\endmsc

## Power Management Framework

### Power State Modes

The platform supports several power states, defined in enum \ref PM_POWERSTATE.

#### Deep Sleep Transition

\msc
 user,API,wakelock,suspend_blockers,Core;
 |||;
 user=>API [label="pm_core_deepsleep(time)"];
 API box Core [label="interrupt lock"];
 API=>suspend_blockers [label="pm_check_suspend_blockers(PM_SUSPENDED)", URL="\ref pm_check_suspend_blockers"];
 API<<suspend_blockers;
 user<<API [label="exit if failed"];
 API=>wakelock [label="pm_wakelock_is_list_empty", URL="\ref pm_wakelock_is_list_empty"];
 API<<wakelock;
 user<<API [label="exit if failed"];
 API=>Core [label="pm_core_deepsleep"];
 API box Core [label="Handle core devices suspend and go to deep sleep. This function also handles the low level/devices resume"];
 API<<Core;
 API box Core [label="interrupt unlock"];
 user<<API;
\endmsc

Quark pm_core_deepsleep handles:
- deep sleep request propagation to ARC and BLE Core (Quark is the master core)
- devices/low-level suspend
- RTC configuration to trigger a wakeup event
- write resume data in RAM to allow the bootstrap to detect that the platform wants
  to resume and jump direlty on the Quark resume hook
- devices/low-level resume
- resume synchronization between all cores

ARC pm_core_deepsleep handles:
- deep sleep request synchronization with master core
- devices/low-level suspend
- write resume data in shared memory RAM to resume ARC core from Quark
- devices/low-level resume
- resume synchronization with master core

#### Shutdown Transition

Shutdown transition is close to deep sleep. There is one generic function
\ref shutdown and a specific function for each cores
pm_core_shutdown.

The user may call this function if a shutdown is required.

The shutdown function can only fail because of a suspend blocker on a
master/slave core.

\msc
 user,API,wakelock,suspend_blockers,Core;
 |||;
 user=>API [label="shutdown()", URL="\ref shutdown"];
 API box Core [label="interrupt lock"];
 API=>suspend_blockers [label="pm_check_suspend_blockers(PM_SHUTDOWN)", URL="\ref pm_check_suspend_blockers"];
 API<<suspend_blockers;
 user<<API [label="exit if suspend_blockers check failed"];
 API box wakelock [label="wait for all wakelocks to be released, with IRQ unlocked"];
 API=>Core [label="pm_core_shutdown"];
 Core box Core [label="Handle core devices shutdown and stop"];
 API<<Core [label="can fail on master core if one slave as suspend_blockers"];
 API box Core [label="interrupt unlock"];
 user<<API [label="fail"];
\endmsc

#### Reboot

\ref reboot currently just reboot the platform, like reset. Devices are not
properly stopped and wakelock/suspend_blockers are not checked (not safe to use).

There are several reboot modes, defined in enum \ref boot_targets.

### Core Synchronization

#### Core Synchronization Between Quark and ARC

Two synchronizations are needed between ARC and Quark cores:
- When Quark wants to transition to deep sleep mode
- When Quark resumes and restart ARC core

This synchronization is based on a soft IPC that uses shared memory.
Requests are always sent by the master.
Slaves are only allowed to acknowledge master core requests.

Several macros are defined:
\msc
 Quark,ARC;
 |||;
 Quark->ARC [label="PM_INIT_REQUEST(PM_XXX_REQUEST)"];
 Quark<-ARC [label="PM_ACK_SET_OK/ERROR"];
\endmsc

#### Core Synchronization Between Quark and BLE Core

Not implemented yet.

## Drivers Responsibility

As in Quark SE the peripherals blocks are also shut down in deep sleep state, it
is required that the drivers save and restore their state in the transitions in
and out of deep sleep.

All drivers must then implement two callbacks: suspend and resume.
The system will call all the suspend functions of the drivers prior to going to
deep sleep, and will call all resume functions upon return of deep sleep.

The drivers should also configure whatever is needed to allow wake up from
deep sleep if they are connected to a device that is supposed to wake the
platform up on certain events.

Each driver is responsible for handling clock gating for its devices when they
are not used. It allows lower power consumption.

## Wakeup Source Events

The platform can be awakened from deep sleep mode by several sources:
- comparator interrupt
- AON GPIO interrupt
- AON timer interrupt
- RTC timer interrupt

@}
