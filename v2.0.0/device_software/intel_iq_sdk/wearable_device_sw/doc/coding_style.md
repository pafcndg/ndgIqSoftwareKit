Coding Style {#coding_style}
============

### C Code

For the C code, the coding rules are the ones from the linux kernel.
See the [Linux kernel coding style](https://www.kernel.org/doc/Documentation/CodingStyle)
web page for more informations.

Other points:
 - Only Comment with C style comments, i.e. /* */


### Commit Message Rules

See the [Commit Message Rules](https://confluence.ndg.intel.com/display/DESW/Commit+Message+Rules)
wiki page

### Documenting C Code

API reference is documented using doxygen. Use the following style:
~~~~~~~~~~~~~~~~~~~~~
/**
 * Send a test command synchronously.
 *
 * This involves several steps:
 * - parse the incoming command buffer to select a command handler
 * - acknowledge the command by returning an invocation id
 * - invoke the relevant command handler
 * All responses will be provided through the response callback.
 *
 * @param command a null-terminated buffer containing the command
 * @param callback the function to call repeatedly until command is complete
 * @param data opaque data to be passed back to the caller on each response
 * @return true in case of success, false otherwise
 */
 bool tcmd_send(char *command, tcmd_rsp_cb_t callback, void *data);
~~~~~~~~~~~~~~~~~~~~~

#### Dos and Don't:

 - Document with doxygen only the code in public headers, i.e. the ones in the
include/ directories. Internal functions can be commented with normal C comments.
 - Don't repeat doc in a header and its matching .c file
 - Mark doxygen commands with @, not with \ or other methods
 - Don't use the \@brief tag, we use autobrief, i.e. the first line terminating
with a '.' is the brief. The rest is the detailed description;
 - Don't use the \@fn, \@struct etc tag, doxygen usually finds this out by
itself, which avoids duplication
 - Document each function parameter: indicate assumptions such as units,
available ranges etc..
 - Document return values: list meaning of all return codes, errors etc..
 - Document public struct, members must be documented using the /*!< my doc for
member */ tag
 - Document public macros
 - Structure the doc hierarchically using groups:
   - groups can be created using the \@defgroup tag
   - headers part of a same module have to be put in a group using the the
\@addtogroup tag
 - Stand-alone pages (like this one) can be created by adding them in the doc/
directory
