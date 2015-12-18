@addtogroup ui_service
@{

The User Interaction service is an event/notification aggregator designed to
- enable all User events an application will subscribe to (user -> app), and
- handle all requests for User notifications (app -> user).

It exposes User inputs to application manager (called hereafter "events"),
and it handles User notifications requests from the application layer.

It is in charge of managing LEDs, Button and Haptics (touch).

User Interaction Service supports the following:
  - Events
    * Enable/Disable events.
    * Expose supported and active events.
    * Power button event.
    * Single button press event.
    * Double button press event.
  - Notifications
    * Enable/Disable notifications.
    * Expose supported and active notifications.
    * Handle notifications requests.
    * LEDs notifications (supporting several pattern and intensity).
    * Vibrator notification (supporting several pattern).

The User Interaction Service relies on either another service or a driver
to catch events.

The features available and provided by User Interaction Service depend on
underlying hardware.

## User interface modules

<!-- All doxygen groups in "ui_service" will be added here -->

@}
