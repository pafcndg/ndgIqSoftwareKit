@addtogroup ble_core_gap_api
@{

# Features

BLE Core provides Zephyr smp/gap/gatt server/gatt client APIs. This APIs abstract the underlying stack. It may even run on different CPU (a kind of RPC).

Those APIs are intended to extend the BLE service and should not be used by an application developper directly!

## Architecture

The BLE core provides the following functionalities:

- GAP functions: Connection and security (keys) handling
- Advertisement support: write & update advertisement data and start/stop advertisment
- GATT Server: Base functions to add a new service and its characteristics (including security permission)
- GATT Client: Base functions to discover a remote service and read/write remote characteristics
- DTM interface to the BLE controller
- Asynchronous events from above blocks: contrary to normal CFW requirements, this events are simply sent
 to client that has opened the service (@b no cfw_register_events() required)

@remark
 - As in general expected by the CFW, any client should wait for the response when calling an API of BLE core
 - Currently only a single client is supported due to the UART interface limitations

@}
