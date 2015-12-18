@addtogroup infra_panic
@{

### Preamble: a Panic is not an Assert

assert(int expression) is to capture programming errors during development phase.
It is disabled as soon as NDEBUG is defined and is consequently only enabled for
debug builds (see [BUILDVARIANT](@ref curie_build_project)).
It is widespread all over the code.

panic(int error) is to reset the device in case of non-recoverable errors,
dumping minimal information for reporting/debugging.
It is enabled for both debug and release builds and is sparingly used.


### Calling a Panic

    void panic (int error);

Error code definition is developer's responsibility. Is it local to a function.
Recommendation is to use an enum.
Panic post processing uses "PC+error code" combo as an unique identifier (panic
ID) to ease correlation from build to build:
- PC post processing is providing the function name, the file name and the line
  number,
- Line number varying from build to build, error code is used.

Example:

Release 1 code only deals with ERROR_1.

~~~~~~~~~~~~~~~~~~~~~{.c}
typedef enum my_errors {
    ERROR_1,
    ERROR_MAX
} my_errors_t;

int my_function(param_t *param)
{
    int ret = 0;

    switch (param->id) {
        case PARAM_ID_1:
            assert(param->payload == NULL);
            break;
        case PARAM_ID_2:
        case PARAM_ID_3:
        case PARAM_ID_4:
            panic(ERROR_1);
            break;
        }
    }
    return ret;
}
~~~~~~~~~~~~~~~~~~~~~

Release 2 code is adding ERROR_2 but "function name+ERROR_1" remains an
unchanged identifier.

~~~~~~~~~~~~~~~~~~~~~{.c}
typedef enum my_errors {
    ERROR_1,
    ERROR_2,
    ERROR_MAX
} my_errors_t;

int my_function(param_t *param)
{
    int ret = 0;

    switch (param->id) {
        case PARAM_ID_1:
            assert(param->payload == NULL);
            break;
        case PARAM_ID_2:
            panic(ERROR_2);
            break;
        case PARAM_ID_3:
        case PARAM_ID_4:
            panic(ERROR_1);
            break;
        }
    }
    return ret;
}
~~~~~~~~~~~~~~~~~~~~~

### Panic mechanism

Processing is kept minimal during panic handling.

Platform reset is done as soon as possible.

Once reset is complete, additional processing is done.

@if PANIC_INTERNAL
@msc
 Master, Slave;
 Master=>Master    [label="panic(error)", URL="\ref panic"];
 Master box Master [label="Dump panic informations into RAM"];
 Master=>Slave     [label="panic_notify(timeout)", URL="\ref panic_notify"];
 ---               [label="Case 1: Slave is alive"];
 Slave=>Slave      [label="handle_panic_notification(core_id)", URL="\ref handle_panic_notification"];
 Slave=>Master     [label="panic_done", URL="\ref panic_done"];
 ---               [label="Case 2: timeout expires before Slave handshake"];
 Master=>Master    [label="reboot()"];
 ...               [label=""];
 Master box Master [label="Boot"];
 Master box Master [label="Copy panic informations from RAM into NVM"];
@endmsc
@endif

@}
