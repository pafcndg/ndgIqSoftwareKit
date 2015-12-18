# Copyright (c) 2015, Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.



ifeq ($(CONFIG_QUARK_SE_QUARK),y)
obj-$(CONFIG_SERVICES_QUARK_SE_CHARGER)         += charging_sm.o \
                                              em_board_config.o
obj-$(CONFIG_QUARK_CHARGER)                += quark_se_charger_driver.o
obj-$(CONFIG_USB_POWER_SUPPLY)             += usb_power_supply_driver.o
obj-$(CONFIG_QI_BQ51003)                   += qi_bq51003_driver.o
obj-$(CONFIG_SERVICES_QUARK_SE_BATTERY_IMPL)    += battery_service.o
obj-$(CONFIG_SERVICES_QUARK_SE_FUELGAUGE)       += adc_fuel_gauge_api.o
obj-y += battery_LUT/

ifeq ($(CONFIG_SERVICES_QUARK_SE_BATTERY),y)
obj-$(CONFIG_SERVICES_QUARK_SE_BATTERY_IMPL)    += battery_property_api.o
obj-y                                      += battery_service_api.o
obj-$(CONFIG_TCMD)                         += battery_service_tcmd.o \
                                              battery_tcmd.o
endif

endif
