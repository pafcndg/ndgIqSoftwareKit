@addtogroup infra_log
@{

### Features

The log system allows the code to output ASCII log messages on a backend such as
UART or BLE.

A typical output looks like this:

    2502|QRK|    FG_S| INFO| fg_timer started
    2502|QRK|    FG_S|ERROR| terminate voltage too low 0 now is 3200
    2220|ARC|  SS_SVC| INFO| [arc_svc_client_connected]18
    2430|ARC|     LOG| WARN| -- log saturation --
    2430|ARC|  SS_SVC| INFO| [ss_svc_msg_handler]is subscribing
    2675|QRK|    CHGR| INFO| CHARGER State : CHARGE COMPLETE
    2430|ARC|PSH_CORE|ERROR| ([sensor_register_evt]ms=2430,sid=0): WARNING ASSERT sensor_register_evt:73

A log message contains:
 - a time stamp in ms
 - a CPU ID in case of multi-CPU log
 - a module from which it originates, e.g. USB, LED, UTIL
 - a log level: debug, info, warning or error
 - the actual ASCII string content


### Log Modules

Each log message is associated to a log module from which it originates.

The list of log modules is defined in the project-specific log_modules.h file
which must be present in the include path. Each log module is defined using
the DEFINE_LOGGER_MODULE [X_MACRO](https://en.wikipedia.org/wiki/X_Macro).

Example log_modules.h file:

@anchor log_debug_level
~~~~~~~~~~~~~~~~~~~~~
DEFINE_LOGGER_MODULE(LOG_MODULE_USB, "USB", 1)
DEFINE_LOGGER_MODULE(LOG_MODULE_BAR, "BAR", 0)
DEFINE_LOGGER_MODULE(LOG_MODULE_FOO, "FOO", 0)
~~~~~~~~~~~~~~~~~~~~~

### Usage

The log system needs to be initialized at program start, and an instance of
log_backend needs to be provided so that the log is actually output to the user.

Once the init is done, messages can be logged using the helper functions:
pr_debug(), pr_info(), pr_warning() and pr_error()

~~~~~~~~~~~~~~~~~~~~~{.c}
/* Use an UART log backend */
log_set_backend(log_backend_uart);

/* Initialize the log system */
log_init();

/* Output a warning log for the USB module */
pr_warning(LOG_MODULE_USB, "There are %d USB interfaces", 5);
~~~~~~~~~~~~~~~~~~~~~

It is also possible to adjust the verbosity of the log by adjusting the log
level by using the log_set_global_level() function.

A special case exists for the debug logs, which must also be explicitly
activated at compile time for each modules, by setting the 3rd parameter of the
DEFINE_LOGGER_MODULE [X_MACRO](https://en.wikipedia.org/wiki/X_Macro) to 1.
See example [above](@ref log_debug_level) for display USB debug logs.


### Multi-Core Log Architecture

On a multi-processors SoC, it is possible to aggregate logs from different cores
to a single core connected to a unique log_backend. For that reason, the log
system supports a collaborative architecture with several "slaves" and one
"master":
 - only the master actually writes on the back-end
 - the "master" log core notifies each slave when it is ready to accept a new
   log message calling the IPC_REQUEST_LOGGER IPC
 - the "slave" log cores then format their log messages and send them to the
   "master" also using the IPC_REQUEST_LOGGER IPC

@dot
digraph example {
    node [shape=box, fontname=Helvetica, fontsize=10];
    edge [arrowhead="open", fontname=Helvetica, fontsize=10];
    m [ label="Master Core"];
    s1 [ label="Slave Core 1"];
    s2 [ label="Slave Core 2"];
    backend [ label="backend", shape="Mrecord" ];
    s1 -> m [ label="sends msg to"];
    s2 -> m [ label="sends msg to"];
    m -> backend [ label="writes msg to"];
}
@enddot

The multi-core logging feature is enabled by the CONFIG_LOG_MULTI_CPU_SUPPORT
Kconfig option. When this is done, the project also needs to define the list of
log cores, and for each of them the IPC callback to use to talk to the "master"
cpu (or NULL for the maste itself).
There can be only one "master" per project.

The list of cores is defined in the project-specific log_cores file, which must
be present in the include path. Each log core is defined using the
DEFINE_LOGGER_CORE [X_MACRO](https://en.wikipedia.org/wiki/X_Macro).

Example log_cores file:

    DEFINE_LOGGER_CORE(LOG_CORE_QUARK, "QRK", NULL, CPU_ID_QUARK)
    DEFINE_LOGGER_CORE(LOG_CORE_ARC, "ARC", ipc_request_sync_int, CPU_ID_ARC)

The type of core is defined by setting the CONFIG_LOG_SLAVE or CONFIG_LOG_MASTER
Kconfig option.

@}
