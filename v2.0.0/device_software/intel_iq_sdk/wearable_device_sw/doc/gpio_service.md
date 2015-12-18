@addtogroup gpio_service
@{

Three kinds of GPIO are reachable:
 - The ARC specific GPIO (two blocks of 8 pin), reachable via the service @b SS_GPIO_SERVICE
 - The common GPIO for QRK and ARC (pin [0..31]), reachable via the service @b SOC_GPIO_SERVICE
 - The AON GPIO (pin [0..5]), reachable via the service @b AON_GPIO_SERVICE

For more detail see hardware documentation


### How to use gpio pin

According to the GPIO you want to reach, you may first open a connection (@ref cfw_open_service_conn)
to the right GPIO service (SS_GPIO_SERVICE, SOC_GPIO_SERVICE or AON_GPIO_SERVICE).

You are now a client of GPIO service.

Next you can configure the pin in input mode or output mode using @ref gpio_configure :
 - output mode: you can change the pin state using @ref gpio_set_state.
 - input mode: you can retrieve the pin state using @ref gpio_get_state
or you can "listen" the pin using @ref gpio_listen ; this way, when the gpio state changes,
the GPIO service automatically sends a message to the client.

@warning Obviously, before using gpio pin via a service, the service should be "registered"
and "initialized" using @ref gpio_service_init.
@}
