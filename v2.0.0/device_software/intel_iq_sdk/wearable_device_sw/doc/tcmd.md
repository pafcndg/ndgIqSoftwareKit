@addtogroup infra_tcmd
@{

## Test Commands

Test Commands are ASCII-encoded messages allowing a Host computer
to perform specific tasks on an embedded device (Target) using a communication
interface such as serial (UART), USB or Bluetooth (BLE).

    +--------+          +--------+
    |        |    TX    |        |
    |        |  +---->  |        |
    |  Host  |          | Target |
    |        |    RX    |        |
    |        |  <----+  |        |
    +--------+          +--------+

On the Host, Test Commands are either typed in by a user in a console
application on a per command basis or run automatically in batch scripts.

On the Target, Test Commands are captured by a Test Command adapter built on top
of a dedicated interface such as UART, parsed by the Test Command engine and
routed towards the appropriate handlers.

Responses are sent back to the Host through the adapter using the interface they
were received on.

### Syntax

#### Commands

Each Test Command is uniquely identified by its Group and Name.

The Group is a noun identifying a well-defined feature to which the command
belongs.

Examples: ble, i2c, led, ...

The Name is a verb or an adjective identifying the unique action(s) to perform
on the target for the specified feature.

Examples: on, off, read, write, ...

The syntax for sending a Test Command is the following:

    <group> <name> (<param1> ... <paramN>)

Where:
- each token is separated by one or more spaces,
- group and names are case-insensitive,
- test command ends with either CR (`\r`) or LF (`\n`).

Examples:

    tcmd version
    ble off
    led ctl on 1,2 off 3,4

#### Responses

A Test Command will always generate an acknowledgement response, followed by an
unlimited number of provisional responses, and terminated by a single success or
error response.

Responses issued for the same request are identified by their Command Invocation
Id (CII).

The syntax for Test Commands responses is the following:

    <group> <name> <cii> <data> ACK | OK | ERROR

Where:
- provisional responses ALWAYS include a data payload
- success and error responses NEVER include a data payload

Examples:

    TX >> tcmd version
    RX << tcmd version 51 ACK
    RX << tcmd version 51 1.0
    RX << tcmd version 51 OK

    TX >> ble off
    RX << ble off 52 ACK
    RX << ble off 52 OK

    TX >> ble off
    RX << ble off 53 ACK
    RX << ble off 53 Not running
    RX << ble off 53 ERROR

### Host Implementation

There is no Host implementation yet allowing the batch processing of Test
Commands.

### Target Implementation

On the target, implementations are done per project, using a set of common
modules.

The typical architecture of a Test Command target implementation is described
below:

      +----------------+ +----------------+ +----------------+
      |  UART Driver   | |   USB Driver   | |   BLE Driver   |
      +----------------+ +----------------+ +----------------+
    ..............................................................
      +----------------+ +----------------+ +----------------+
      | TC UART Adapter| | TC USB Adapter | | TC BLE Adapter |
      +----------------+ +----------------+ +----------------+
      +------------------------------------------------------+
      |                      TC Engine                       |
      +------------------------------------------------------+
      +----------------+ +----------------+ +----------------+
      | TC BLE Handler | | TC LED Handler | | TC I2C Handler |
      +----------------+ +----------------+ +----------------+

The Test Command engine is the heart of the implementation, routing Test Command
requests/responses between Test Command adapters and Test Command handlers.

It provides an API to:
- register a Test Command handler,
- issue Test Commands either synchronously or asynchronously,
- handle Test Command responses.

It also implements two generic Test Commands:
- the 'help' Test Command returning the list of Test Commands,
- the 'tcmd info' Test Command returning the engine version.

Test Command adapters are built on top of a communication interface to capture
external input and send it to the engine for processing.

Test Command handlers provide the actual implementation of the Test Commands.

#### Writing a Test Command Adapter

##### Using Directly the Engine API

The Test Command engine API is based on the exchange of null-terminated buffers
between a test command interface and a test handler.

An adapter will send a null-terminated Test Command buffer to the engine,
specifying a completion callback that accepts response buffers.

The Test Command engine will parse the Test Command, identify the relevant
handler, and invoke it.

Response buffers are then passed back from the handler to the adapter through
the engine using the callback passed during the Test Command invocation, until
final or error response is received.

The adapter and handler are responsible for freeing any dynamic buffer they may
have allocated, but their responsibility are slightly different:
- the handler may free the buffers as soon as the response callback returns,
which means that if the client needs to retain some data, it must copy it,
- in the contrary, the adapter must wait until the final response has been
received to free the Test Command buffer.

The Test Command engine API may be invoked either using a synchronous or an
asynchronous interface.

Activating the asynchronous interface requires calling a specific initialization
function to tell the engine in which task/thread it must invoke the Test Command
handlers by passing it the corresponding message queue.

There are two main reasons for using asynchronous Test Commands:
-# when using a synchronous API would lock your system.
   - Sending a Test Command in the context of an interrupt may lock your
     target if the callback needs to write to a serial interface using a
     semaphore or a mutex:
     + With the synchronous API, everything happens in the same thread/task as
       the caller. 
     + So the processing would be done in interrupt context, possibly with
       operations invalid in interrupt context (waiting for a semaphore).
-# when you want to send several Test Commands in parallel.
   - The adapter must support maintaining several Test Command buffers,
     which increases its complexity. This can however be greatly simplified
     by using the generic console API described below.

##### Using the Generic Console API

The Test Command console is a thin wrapper around the Test Command engine
that can easily be plugged on serial interfaces such as UART to support
multiple Test Commands in parallel: the adapter needs only to provide an
input buffer and a TX callback whose signature is based on putc in stdlib.

The console module acts as a Test Command buffer, allowing multiple test
commands to be issued in parallel:
- it copies input buffers,
- link them to a calling context,
- invoke the test command engine,
- push back Test Command responses as they are received,
- free the calling context when the last response has been received.

Note: as mentioned above, the generic console makes a copy of the input buffer.
If you want to avoid this copy, you need to invoke the Test Command engine
directly from your adapter.

#### Writing a Test Command Handler

Writing a Test Command handler can be as simple as implementing a function with
the Test Command handler signature and declaring it using a macro:

~~~~~~~~~~~~~~~~~~~~~{.c}
#include "infra/tcmd/handler.h"
static void tcmd_version(int argc, char **argv, tcmd_handler_ctx *ctx)
{
    TCMD_RSP_FINAL(ctx, "1.0");
}
DECLARE_TEST_COMMAND(tcmd, version, tcmd_version);
~~~~~~~~~~~~~~~~~~~~~

Because things are not always simple, the paragraphs below go into more details.

##### Handler Implementation

All Test Command handler have the same signature:

    void tcmd_version(int argc, char **argv, tcmd_handler_ctx *ctx)

Where:
- argc is the number of arguments in the Test Command (including group and name),
- argv is a table of null-terminated buffers containing the arguments, argv[0]
being the group and argv[1] being the name,
- ctx is the opaque context to pass to the responses macros.

A typical Test Command handler implementation would include the following steps:
-# (OPTIONAL) Check the input arguments
-# (OPTIONAL) perform the requested action
-# (OPTIONAL) Send one or more PROVISIONAL responses
-# (MANDATORY) Send either a FINAL or ERROR response
-# return (NO MORE responses are allowed beyond this point)

Note: when checking the arguments, argc contains the number of arguments, and
argv a list of null-terminated strings representing them. The first argument is
the group, and the second is the name.

As a side note, even if a FINAL or ERROR never include a response buffer, you
can pass one to the response macros: the engine will duplicate the response,
sending both a PROVISIONAL response containing the buffer and the FINAL or ERROR
response.

Said differently, the following calls are equivalent:

~~~~~~~~~~~~~~~~~~~~~{.c}
TCMD_RSP_PROVISIONAL(ctx, "foo");
TCMD_RSP_FINAL(ctx, NULL);

TCMD_RSP_FINAL(ctx, "foo");
~~~~~~~~~~~~~~~~~~~~~

And will produce the same output:

    <group> <name> <cii> foo
    <group> <name> <cii> OK

You can find below a more detailed example:
~~~~~~~~~~~~~~~~~~~~~{.c}
#include <string.h>
#include <stdbool.h>
#include "infra/tcmd/handler.h"

static void itf_ctl(int argc, char **argv, tcmd_handler_ctx *ctx)
{
    bool start;
    if (argc >= 3) {
        if (strncmp(argv[2], "start", 5) == 0) {
            start = true;
        } else if (strncmp(argv[2], "stop", 4) == 0) {
            start = false;
        } else {
            TCMD_RSP_ERROR(ctx, "Invalid parameter");
            return;
        }
    }
    if (start) {
      /* Start the interface */
      ...
    } else {
        /* Stop the interface */
        ...
    }
    TCMD_RSP_FINAL(ctx, NULL);
}
DECLARE_TEST_COMMAND(itf, control, itf_ctl);
~~~~~~~~~~~~~~~~~~~~~

#### Handler Declaration

The Test Command handlers declaration are not centralized in one file: instead,
all handlers are declared inside a specific code section that is dynamically
parsed at runtime by the Test Command engine.

To declare a Test Command handler, use the DECLARE_TEST_COMMAND macro providing:
- the group to which the Test Command belong,
- the name of the Test Command,
- the function pointer to the Test Command handler.

Example:
~~~~~~~~~~~~~~~~~~~~~
DECLARE_TEST_COMMAND(tcmd, version, tcmd_version);
~~~~~~~~~~~~~~~~~~~~~

Please note the absence of quotes around the group (tcmd) and name (version) in
the DECLARE_TEST_COMMAND invocation: this is required as they will be used to
declare a struct identifying the handler.

Please also note that you CANNOT pass a macro for group and name, because it
will not be expanded: instead the handler will be registered for your unexpanded
macro group/name.

##### Asynchronous Responses in Handlers

Some actions implemented by Test Command handlers will complete asynchronously,
and the final response will typically be sent from a completion callback.

Note: if an asynchronous API doesn't provide a completion callback, the best you
can do is to return a final response from your handler and have the Host poll
for completion.

If you do have a completion callback, it will usually accept also an opaque
context field that you can use to pass the handler context so that you can send
the final response from there.

##### General Implementation Guidelines

As a rule of thumb, keep your Test Command handlers as simple as possible: the
complexity is supposed to be on the Host side, not on the Target.

Do not fulfill asynchronous pre-requisites in your handler: if your handler
requires something to be initialized before being executed, offer another Test
Command for initialization and return on error if your handler is called before
it.

Do not implement scheduling in your handler: if you need to periodically perform
an action, have it scheduled on the Host.

Do not, ever, call another Test Command handler from your handler unless you
return immediately after that: when the first handler will return its final
response, the calling context will be freed !

Avoid blocking wait in your handler: in the best scenario, you will lock the
engine task (if using async mode), in the worst you will lock the adapter task
(which can be an interrupt).

#### Integrating the Test Commands

The compilation of the Test Command components are activated using configuration
flags.

Test Command engine:

    CONFIG_TCMD = y

Test Command console:

    CONFIG_TCMD_CONSOLE = y

Asynchronous Test Commands:

    CONFIG_TCMD_ASYNC = y

In addition, for the test command engine to work, the test commands need to be
stored in a dedicated code section whose boundaries are identified by its start
and end addresses.

This section needs to be specified in a linker script:

    SECTIONS {
        /* Special section to store test command structures */
        .cli_sections : {
            . = ALIGN(8);
            __test_cmds_start = .;
            KEEP(*(SORT(.test_cmds*)))
            __test_cmds_end = .;
        }
    }

### Multi-core Test Commands

On a multi-processors SOC, each core would have its own Test Command engine,
but not necessarily access to communication interfaces.

For that reason, the Test Command Framework supports a collaborative
architecture, where the "master" engine running on the main core can route Test
Commands to "slave" engines on the other cores.

Each "slave" engine would typically need to declare itself to the "master"
engine using a dedicated IPC, providing a friendly name.

Test commands prefixed by this friendly name received on an adapter will then be
routed towards the "slave" engine by the "master", and responses from the
"slave" will be sent back to the adapter.

A special "tcmd slaves" Test Command implemented on the master lists slave
engines.

Example:

    TX >> tcmd slaves

    RX << tcmd slaves 18 ACK
    RX << tcmd slaves 18 arc
    RX << tcmd slaves 18 OK

    TX >> arc.help
    RX << arc.help 1 ACK
    RX << arc.help 1 tcmd: version
    RX << arc.help 1 OK

@}
