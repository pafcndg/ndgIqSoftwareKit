#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for testing the Notifications IQ
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

import binascii
from IntelNotifications_pb2 import Notification
import time
import util

PATTERN_TIME_TUNING = 100
MAX_WAVE_DURATION = 1000

class data_parameter:
    def __init__(self,
                 notif_has_vibr = False,
                 vibr_t_on_1 = 500,
                 vibr_t_off_1 = 500,
                 vibr_t_on_2 = 500,
                 vibr_t_off_2 = 500,
                 vibr_t_off_3 = 500,
                 vibr_t_off_4= 500,
                 effect_1 = 16, # 1000 ms Alert 100%
                 effect_2 = 70, # Transition Ramp Down 100 to 0%
                 effect_3 = 89, # Transition Ramp Up 0 to 100%
                 effect_4 = 32, # Short Double Click 80%
                 effect_5 = 59, # Transition Click 2 80%
                 vibr_amplitude = 127,
                 vibr_repetition_count = 2,
                 vibr_pattern_type = Notification.VIBRA_SQUARE,
                 notif_has_led = False,
                 led_rgb_count = 3,
                 led_t_on_1 = 500,
                 led_t_off_1 = 500,
                 led_t_on_2 = 500,
                 led_t_off_2 = 500,
                 led_t_on_3 = 500,
                 led_t_off_3 = 500,
                 led_intensity = 50,
                 led_rgb1_red = 255,
                 led_rgb1_green = 0,
                 led_rgb1_blue = 0,
                 led_rgb2_red = 0,
                 led_rgb2_green = 255,
                 led_rgb2_blue = 0,
                 led_rgb3_red = 0,
                 led_rgb3_green = 0,
                 led_rgb3_blue = 255,
                 led_repetition_count = 1,
                 led_pattern_type = Notification.LED_BLINK,
                 delay_startup = 0):
        self.notif_has_vibr = notif_has_vibr
        self.vibr_t_on_1 = vibr_t_on_1
        self.vibr_t_off_1 = vibr_t_off_1
        self.vibr_t_on_2 = vibr_t_on_2
        self.vibr_t_off_2 = vibr_t_off_2
        self.vibr_t_off_3 = vibr_t_off_3
        self.vibr_t_off_4 = vibr_t_off_4
        self.effect_1 = effect_1
        self.effect_2 = effect_2
        self.effect_3 = effect_3
        self.effect_4 = effect_4
        self.effect_5 = effect_5
        self.vibr_amplitude = vibr_amplitude
        self.vibr_repetition_count = vibr_repetition_count
        self.vibr_pattern_type = vibr_pattern_type
        self.notif_has_led = notif_has_led
        self.led_rgb_count = led_rgb_count
        self.led_t_on_1 = led_t_on_1
        self.led_t_off_1 = led_t_off_1
        self.led_t_on_2 = led_t_on_2
        self.led_t_off_2 = led_t_off_2
        self.led_t_on_3 = led_t_on_3
        self.led_t_off_3 = led_t_off_3
        self.led_intensity = led_intensity
        self.led_rgb1_red = led_rgb1_red
        self.led_rgb1_green = led_rgb1_green
        self.led_rgb1_blue = led_rgb1_blue
        self.led_rgb2_red = led_rgb2_red
        self.led_rgb2_green = led_rgb2_green
        self.led_rgb2_blue = led_rgb2_blue
        self.led_rgb3_red = led_rgb3_red
        self.led_rgb3_green = led_rgb3_green
        self.led_rgb3_blue = led_rgb3_blue
        self.led_repetition_count = led_repetition_count
        self.led_pattern_type = led_pattern_type
        self.delay_startup = delay_startup

def notif_iq_automatic_test(device):
    print("-------------------------------------")
    print("-   AUTOMATIC TEST ARE IMPOSSIBLE   -")
    print("-------------------------------------")
    print("")
    print("")

def notif_iq_test(device, data):

    # Instantiate an haptic and led notification
    vibr_duration = 0
    led_duration = 0

    notif = Notification()
    if(data.notif_has_vibr == True):
        notif.vibra_notif.type = data.vibr_pattern_type
        notif.vibra_notif.amplitude = data.vibr_amplitude
        dur1 = notif.vibra_notif.duration.add()
        dur1.duration_on = data.vibr_t_on_1
        dur1.duration_off = data.vibr_t_off_1
        dur2 = notif.vibra_notif.duration.add()
        dur2.duration_on = data.vibr_t_on_2
        dur2.duration_off = data.vibr_t_off_2
        notif.vibra_notif.repetition_count = data.vibr_repetition_count
        if(notif.vibra_notif.type == Notification.VIBRA_SPECIAL_EFFECTS):
            dur3 = notif.vibra_notif.duration.add()
            dur3.duration_off = data.vibr_t_off_3
            dur4 = notif.vibra_notif.duration.add()
            dur4.duration_off = data.vibr_t_off_4
            notif.vibra_notif.effect.append(data.effect_1)
            notif.vibra_notif.effect.append(data.effect_2)
            notif.vibra_notif.effect.append(data.effect_3)
            notif.vibra_notif.effect.append(data.effect_4)
            notif.vibra_notif.effect.append(data.effect_5)
            vibr_duration = 5 * MAX_WAVE_DURATION + dur1.duration_off + dur2.duration_off + dur3.duration_off + dur4.duration_off
        else:
            vibr_duration = (dur1.duration_on + dur1.duration_off + dur2.duration_on + dur2.duration_off) * (notif.vibra_notif.repetition_count + 1) - dur2.duration_off

    if(data.notif_has_led == True):
        notif.led_notif.id = 0
        notif.led_notif.type = data.led_pattern_type
        notif.led_notif.intensity = data.led_intensity
        if(data.led_rgb_count >=1):
            rgb1 = notif.led_notif.rgb.add()
            rgb1.red = data.led_rgb1_red
            rgb1.green = data.led_rgb1_green
            rgb1.blue = data.led_rgb1_blue
            duration1 = notif.led_notif.duration.add()
            duration1.duration_on = data.led_t_on_1
            duration1.duration_off = data.led_t_off_1
        if(data.led_rgb_count >=2):
            rgb2 = notif.led_notif.rgb.add()
            rgb2.red = data.led_rgb2_red
            rgb2.green = data.led_rgb2_green
            rgb2.blue = data.led_rgb2_blue
            duration2 = notif.led_notif.duration.add()
            duration2.duration_on = data.led_t_on_2
            duration2.duration_off = data.led_t_off_2
        if(data.led_rgb_count >=3):
            rgb3 = notif.led_notif.rgb.add()
            rgb3.red = data.led_rgb3_red
            rgb3.green = data.led_rgb3_green
            rgb3.blue = data.led_rgb3_blue
            duration3 = notif.led_notif.duration.add()
            duration3.duration_on = data.led_t_on_3
            duration3.duration_off = data.led_t_off_3
        notif.led_notif.repetition_count = data.led_repetition_count
        if(data.led_rgb_count == 1):
            led_duration = (data.led_t_on_1 + data.led_t_off_1 + PATTERN_TIME_TUNING) * (1 + data.led_repetition_count)

        if(data.led_rgb_count == 2):
            led_duration = (data.led_t_on_1 + data.led_t_off_1 +
                            data.led_t_on_2 + data.led_t_off_2 +
                            PATTERN_TIME_TUNING) * (1 + data.led_repetition_count)

        if(data.led_rgb_count == 3):
            led_duration = (data.led_t_on_1 + data.led_t_off_1 +
                            data.led_t_on_2 + data.led_t_off_2 +
                            data.led_t_on_3 + data.led_t_off_3 + PATTERN_TIME_TUNING) * (1 + data.led_repetition_count)


    # Publish to intel/core/
    device.itm_publish("\x49\x43\x4E\x00", notif.SerializeToString())

    time_between_test = 1

    time.sleep(max(vibr_duration, led_duration)/1000 + time_between_test)

def notif_iq_test_run_all_tests(device):

    print("--------------step 1-----------------")
    print("-  TEST BLINK_X1 (FIXED) white 2s   -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_led = True,
                          led_pattern_type = Notification.LED_BLINK,
                          led_rgb_count = 1, led_t_on_1 = 0, led_t_off_1 = 0,
                          led_rgb1_red = 255, led_rgb1_green = 255, led_rgb1_blue = 255,
                          led_repetition_count = 0)
    notif_iq_test(device, data)
    time.sleep(2)

    print("--------------step 2-----------------")
    print("-    LED_NONE (TURN OFF FIXED)      -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_led = True,
                          led_pattern_type = Notification.LED_BLINK,
                          led_rgb_count = 0, led_t_on_1 = 0, led_t_off_1 = 0,
                          led_repetition_count = 0)
    notif_iq_test(device, data)

    print("--------------step 3-----------------")
    print("-          TEST BLINK X1            -")
    print("-      RED for 0.5s, OFF for 0.5s   -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_led = True,
                          led_pattern_type = Notification.LED_BLINK, led_rgb_count = 1,
                          led_repetition_count = 0)
    notif_iq_test(device, data)

    print("--------------step 4-----------------")
    print("-         TEST BLINK X2             -")
    print("-     RED for 0.5s, OFF for 0.5s    -")
    print("-    GREEN for 0.5s, OFF for 0.5s   -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_led = True,
                          led_pattern_type = Notification.LED_BLINK, led_rgb_count = 2,
                          led_repetition_count = 0)
    notif_iq_test(device, data)

    print("--------------step 5-----------------")
    print("-         TEST BLINK X3             -")
    print("-    RED for 0.5s, OFF for 0.5s     -")
    print("-    GREEN for 0.5s, OFF for 0.5s   -")
    print("-    BLUE for 0.5s, OFF for 0.5s    -")
    print("-    1 repetitions (run 2 times)    -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_led = True,
                          led_pattern_type = Notification.LED_BLINK,
                          led_rgb_count = 3)
    notif_iq_test(device, data)

    print("--------------step 6-----------------")
    print("-          TEST WAVE X2             -")
    print("-    RED for 2s, OFF for 1s         -")
    print("-    GREEN for 2s, OFF for 1s       -")
    print("-    1 repetitions (run 2 times)    -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_led = True,
                          led_pattern_type = Notification.LED_WAVE,
                          led_t_on_1 = 2000, led_t_off_1 = 1000,
                          led_t_on_2 = 2000, led_t_off_2 = 1000,
                          led_repetition_count = 0,
                          led_rgb_count = 2)
    notif_iq_test(device, data)

    print("--------------step 7-----------------")
    print("-       TEST VIBR SQUARE_X2         -")
    print("-    ON for 0.5s, OFF for 0.5s      -")
    print("-    ON for 0.5s, OFF for 0.5s      -")
    print("-    2 repetitions (run 3 times)    -")
    print("-    medium amplitude               -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_vibr = True,
                          vibr_amplitude = 127,
                          vibr_pattern_type = Notification.VIBRA_SQUARE
    )
    notif_iq_test(device, data)

    print("--------------step 8-----------------")
    print("-       step 6 with AMPL=100%       -")
    print("-  without repetition (run 1 time)  -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_vibr = True,
                          vibr_repetition_count = 0,
                          vibr_amplitude = 255)
    notif_iq_test(device, data)

    print("--------------step 9-----------------")
    print("-       step 6 with AMPL=20%        -")
    print("-        without repetition         -")
    print("-  without repetition (run 1 time)  -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_vibr = True,
                          vibr_repetition_count = 0,
                          vibr_amplitude = 51)
    notif_iq_test(device, data)

    print("--------------step 10----------------")
    print("-        TEST VIBRA T ON 1=0        -")
    print("-        T OFF 1 = 0.5s             -")
    print("-        T ON 2 = 0.5s              -")
    print("-        T OFF 2 = 0.5s             -")
    print("-  without repetition (run 1 time)  -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_vibr = True,
                          vibr_repetition_count = 0,
                          vibr_t_on_1 = 0)
    notif_iq_test(device, data)

    print("--------------step 11----------------")
    print("-   TEST VIBRA  (medium amplitude)  -")
    print("-        T ON 1 = 0.1s              -")
    print("-        T OFF 1 = 0.1s             -")
    print("-        T ON 2 = 0.1s              -")
    print("-        T OFF 2 = 0.1s             -")
    print("-  255 repetition (run 256 time)    -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_vibr = True,
                          vibr_amplitude = 127,
                          vibr_repetition_count = 255,
                          vibr_t_on_1 = 100,
                          vibr_t_off_1 = 100,
                          vibr_t_on_2 = 100,
                          vibr_t_off_2 = 100,
                          vibr_pattern_type = Notification.VIBRA_SQUARE)
    notif_iq_test(device, data)

    print("--------------step 12----------------")
    print("-     TEST VIBR SPECIAL EFFECT      -")
    print("-  1 1000 ms Alert 100%             -")
    print("-  2 Transition Ramp Down 100 to 0%  ")
    print("-  3 Transition Ramp Up 0 to 100% -  ")
    print("-  4 Short Double Click 80%          ")
    print("-  5 Transition Click 2 80%         -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_vibr = True,
                          vibr_pattern_type = Notification.VIBRA_SPECIAL_EFFECTS
    )
    notif_iq_test(device, data)

    print("--------------step 13----------------")
    print("-         TEST SQUARE VIBRA         -")
    print("-          + BLINK_X2 LED           -")
    print("-    1 repetitions (run 2 times)    -")
    print("-------------------------------------")
    print("")
    print("")
    data = data_parameter(notif_has_vibr = True,
                          vibr_repetition_count = 1,
                          notif_has_led = True,
                          led_rgb_count = 2,
                          led_repetition_count = 1)
    notif_iq_test(device, data)

if __name__ == "__main__":
    device = util.connection(["\x49\x43\x4E\x00"])

    notif_iq_test_run_all_tests(device)

    device.disconnect()
