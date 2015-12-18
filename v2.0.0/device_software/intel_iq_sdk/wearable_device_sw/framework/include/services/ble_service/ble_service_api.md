@addtogroup ble_service
@{

# Features

BLE service provides BLE interface, abstracting most of the complexity of BLE.

It provides APIs covering peripheral role and will support central role in the future.

BLE services can be extended by using the BLE core APIs and implement a new BLE service/profile. This is achieved
by using the ble_core service APIs.

## Architecture

![BLE Service Architecture Figure](ble_sw_architecture.png "BLE Software architecture")

This architecture splits BLE support into two modules:
- an application oriented CFW oriented BLE services. This provides more easy to use APIs.
- a the BLE core API which offer smp/gap/gatt APIs. this allows to abstract from the actual BLE stack or chip.

@remark
- It is assumed that a client (application) waits for a response msg before calling another API.
- Some API requests trigger events as the execution is asynchronous (e.g. connecting to a peer device, passkey request).

Currently the following BLE services are implemented and supported:
- Simplified security handling
- BLE DIS service (UUID: 0x180a): Different device info's (SW version etc) are displayed as per BT spec.
- BLE BAS service (UUID: 0x180f): BLE Battery level service as per BT spec. It uses the battery service to update automatically the battery level characteristic
- BLE ISPP service (UUID: dd97c415-fed9-4766-b18f-ba690d24a06a): Intel serial port protocol: A serial port emulation running over BLE
- BLE DTM: via @ref ble_test DTM mode is supported

## Application Development

An application developer should simply use the BLE service APIs. Here is a typical sequence:
- open service with BLE_SERVICE_ID
- enable BLE: ble_enable()
- call for ble_init_svc(XXX, params) for each service you want to use
- register for events, e.g. MSG_ID_BLE_CONNECT_EVT, MSG_ID_BLE_DISCONNECT_EVT, MSG_ID_BLE_ADV_TO_EVT, etc. otherwise your application will not receive any event!
- e.g. call ble_update_data(handle/cb pointer) to update characteristic (gatt server) on local BLE controller.
- ble_subscribe(BLE handle): subscribe to be notified on a specific characteristic (gatt client)
- ble_get_remote_data() read a characteristic on a remote device (gatt client) on an @b existing connection
- ble_set_remote_data() write a characteristic on a remote device (gatt client) on an @b existing connection
- incoming data is signaled by the following 3 events (if registered for them!):
  - MSG_ID_BLE_NOTIF_EVT: ble_subscribe() has been called and the remote notifies new data
  - MSG_ID_BLE_GET_REMOTE_DATA_EVT: ble_get_remote_data() has been called to read remote data
  - MSG_ID_BLE_SVC_DATA_CHANGED_EVT: remote device has updated a characteristic (gatt server role)


# How-to Add new BLE service

Extending the current Component Framework BLE service into a custom service can be achieved by using the gatt APIs.
BLE ISPP can be used an example for a proprietary _BLE service and protocol_.

- As a convention, a new BLE service should be implemented in a ble_service_XXX.[ch] files.
- A function ble_init_service_XXX() should be implemented and called by an application.
  It should trigger the initialization of the new BLE service based on the UUID.
  The UUID being either 16 bits defined by BT spec or 128 bits for user defined services
  - Your new init service should call ble_alloc_init_service_rsp() to pre-allocate the response to be sent back to the client. Pass the pointer in _priv_ pointer in all the calls.
  - ble_gatts_add_service(): register a new service and use returned handle as a reference for registering the characteristics
  - ble_gatts_add_characteristic(): add new characteristic to the service
  - In ble_service_dis.c, function ble_gatts_add_next_char_dis() shows how to chain multiple characteristics registration together. Typically your implementation should contain an ble_gatts_add_next_char_XXX()
    As a convention, this function returns 0 if all characteristic are registered, a positive number for the current char count or negative for error
- after successful adding the last characteristic (ble_gatts_add_next_char_XXX returns 0), send back the pre-allocated MSG_ID_BLE_INIT_SVC_RSP from the _priv_ pointer indicating allocated handles and pointer control block.

A protocol type of service, e.g. ISPP, may also support the protocol events to read/write in addition to ble_update_data()
@}
