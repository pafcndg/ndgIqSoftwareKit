@addtogroup ipc
@{

### Features

The Inter-Processor Communication (IPC) is the layer that allows the cores/nodes to
communicate with each other.

It defines an interface that can be implemented over the underlying physical
layer.

It is intented to be used for shared memory platforms as well as non-shared
memory platform.

- The Quark/ARC IPC is using _shared memory_ in order to transport messages
  and Mailbox registers to interrupt the cores,
- The Quark/BLE Core IPC is using _UART_ to transport messages and GPIOs
  to wake up the cores from deep sleep.

@}
