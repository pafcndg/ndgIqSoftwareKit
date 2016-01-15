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
- the BLE core APIs which offer Zephyr opensource smp/gap/gatt APIs. this allows to abstract from the actual BLE stack or chip.

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
- call the ble_init_service_xxx(params) (xxx: dis, bas, etc) for each service you want to use
- register for events, e.g. MSG_ID_BLE_CONNECT_EVT, MSG_ID_BLE_DISCONNECT_EVT, MSG_ID_BLE_ADV_TO_EVT, etc. otherwise your application will not receive any event!
- e.g. call ble_update_data(handle/cb pointer) to update characteristic (gatt server) on local BLE controller.
- ble_subscribe(BLE handle): subscribe to be notified on a specific characteristic (gatt client)
- ble_get_remote_data() read a characteristic on a remote device (gatt client) on an @b existing connection
- ble_set_remote_data() write a characteristic on a remote device (gatt client) on an @b existing connection
- incoming data is signaled by the following events (if registered for them!):
  - MSG_ID_BLE_NOTIF_EVT: ble_subscribe() has been called and the remote notifies new data


# How-to Add new BLE service

Extending the current Component Framework BLE service into a custom service can be achieved by using the zephyr gatt APIs.
BLE ISPP can be used an example for a proprietary _BLE service and protocol_.

- As a convention, a new BLE service should be implemented in a ble_service_XXX.[ch] files (e.g. XXX: gap, bas, dis, etc).
- A function ble_init_service_XXX() should be implemented and called by an application.
  It should trigger the initialization of the new BLE service by calling bt_gatt_register()
- Each BLE service is defined by const attribute table using the macros found in gatt.h (:
  - BT_GATT_PRIMARY_SERVICE: the UUID of the service
  - BT_GATT_CHARACTERISTIC: the UUID of the characteristics and its properties
  - BT_GATT_DESCRIPTOR: permissions, read & write functions, optionally pointer to data storage
    - read() used init ble controller database
      - if buf pointer is NULL, it needs to return the init/max length if charactristic is readable
      - if buf pointer is not NULL, it needs copy the init data to buf pointer
    - write(): if handler for writeable characteristic
    - data pointer: optional, maybe used for init value or storage for write operation
  - BT_GATT_CCC: for notifiable or indicatable characteristics, must follow above 2 macros
    - the two macro parameters should be NULL as not used. however the macro internal CCC value is updated (see bas) via bt_gatt_attr_write_ccc().
  - additional descriptors for a characteristic maybe defined. however only AFTER above macros
- the service must define an init_svc() and init_svc_complete() function. The complete function sends back the response.
- additional helper functions maybe required to update the ble controller database
- the service may register with connect/disconnect events using ble_service_register_for_conn_st()

A protocol type of service, e.g. ISPP, may also support the protocol events to read/write in addition to ble_update_data()
@}
