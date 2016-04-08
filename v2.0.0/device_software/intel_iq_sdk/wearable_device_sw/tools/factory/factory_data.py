#!/usr/bin/python
# -*- coding: utf-8 -*-

# INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its suppliers
# or licensors.
# Title to the Material remains with Intel Corporation or its suppliers and
# licensors.
# The Material contains trade secrets and proprietary and confidential information
# of Intel or its suppliers and licensors. The Material is protected by worldwide
# copyright and trade secret laws and treaty provisions.
# No part of the Material may be used, copied, reproduced, modified, published,
# uploaded, posted, transmitted, distributed, or disclosed in any way without
# Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise.
#
# Any license under such intellectual property rights must be express and
# approved by Intel in writing

import ctypes

MAGIC = "$FA!"
VERSION = 0x01

class HardwareInfo(ctypes.Structure):
    """Hardware info, see declaration in infra/factory_data.h, size: 16 bytes """
    _pack_ = 1
    _fields_ = [
        ("hardware_name", ctypes.c_ubyte),
        ("hardware_type", ctypes.c_ubyte),
        ("hardware_revision", ctypes.c_ubyte),
        ("reserved", ctypes.c_ubyte),
        ("hardware_variant", ctypes.c_uint*3)
    ]

class OEMData(ctypes.Structure):
    """OEM data, see declaration in infra/factory_data.h, size: 512 bytes """
    _pack_ = 1
    _fields_ = [
        ("header", ctypes.c_ubyte * 4),
        ("magic", ctypes.c_ubyte * 4),
        ("version", ctypes.c_ubyte),
        ("production_mode_oem", ctypes.c_ubyte),
        ("production_mode_customer", ctypes.c_ubyte),
        ("reserved0", ctypes.c_ubyte),
        ("hardware_info", HardwareInfo),
        ("uuid", ctypes.c_ubyte * 16),
        ("factory_sn", ctypes.c_ubyte * 32),
        ("hardware_id", ctypes.c_ubyte * 32),
        ("project_data", ctypes.c_ubyte * 84),
        ("security", ctypes.c_ubyte * 304),
        ("soc_data", ctypes.c_ubyte * 16),
    ]

class CustomerData(ctypes.Structure):
    """Customer data, see declaration in infra/factory_data.h, size: 512 bytes """
    _pack_ = 1
    _fields_ = [
        ("product_sn", ctypes.c_ubyte * 16),
        ("product_hw_ver", ctypes.c_ubyte * 4),
        ("reserved", ctypes.c_ubyte * 492)
    ]

class FactoryData(ctypes.Structure):
    """Factory data, see declaration in infra/factory_data.h """
    _pack_ = 1
    _fields_ = [
        ("oem_data", OEMData),
        ("customer_data", CustomerData)
    ]

def init_oem_base_fields(oem_fdata):

    for i in range(0, len(oem_fdata.header)):
        oem_fdata.header[i] = 0xFF

    for i in range(0, len(oem_fdata.soc_data)):
        oem_fdata.soc_data[i] = 0xFF

    oem_fdata.magic = (ctypes.c_ubyte * 4).from_buffer_copy(MAGIC)
    oem_fdata.version = VERSION
    oem_fdata.production_mode_oem = 0xFF
    oem_fdata.production_mode_customer = 0xFF
