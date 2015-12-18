/*
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SENSOR_SVC_PLATFORM_H__
#define __SENSOR_SVC_PLATFORM_H__

#include "cfw/cfw_service.h"
#include "services/sensor_service/sensor_svc_api.h"
#include "machine.h"

#define SENSOR_SERVICE_DEBUG        0

/*\brief:
 * BASE_SENSOR_SERVICE ,it indicate  features that all  sensor service have
 * ARC_SENSOR_CORE_SERVICE ,it indicate the freatures that sensor_core service have
 * QRK_SENSOR_SERVICE ,it indicate the features that QRK_SENSOR_SERVICE have
 * */
#define    BASE_SENSOR_SERVICE      0
#define    ARC_SENSOR_CORE_SERVICE  1
#define    QRK_SENSOR_SERVICE       2
#define    HOST_SENSOR_SERVICE      3
#define    MAX_SENSOR_SERVICE_NUM   4

#ifdef CONFIG_SERVICES_SENSOR_IMPL
#define SENSOR_SERVICE              ARC_SENSOR_CORE_SERVICE
#define SS_SVC_ID                   ARC_SC_SVC_ID
#endif

typedef void (*start_scanning_req_f)(ss_start_sensor_scanning_req_t *, void *);
typedef void (*stop_scanning_req_f)(ss_stop_sensor_scanning_req_t *, void *);
typedef void (*sensor_subscribe_req_f)(ss_sensor_subscribe_data_req_t *, void *);
typedef void (*sensor_unsubscribe_req_f)(ss_sensor_unsubscribe_data_req_t *, void *);
typedef void (*sensor_set_property_req_f)(ss_sensor_set_property_req_t*, void *);
typedef void (*sensor_get_property_req_f)(ss_sensor_get_property_req_t*, void *);
typedef void (*sensor_calibration_req_f)(ss_sensor_calibration_req_t*, void *);

#ifdef CONFIG_SENSOR_SERVICE_RESERVED_FUNC
typedef void (*sensor_start_req_f)(ss_sensor_start_req_t *, void *);
typedef void (*sensor_pair_req_f)(ss_sensor_pair_req_t *, void *);
typedef void (*sensor_unpair_req_f)(ss_sensor_unpair_req_t *, void *);
typedef void (*sensor_device_id_req_f)(ss_sensor_t, void *);
typedef void (*sensor_product_id_req_f)(ss_sensor_t, void *);
typedef void (*sensor_manufacture_id_req_f)(ss_sensor_t, void *);
typedef void (*sensor_serial_number_req_f)(ss_sensor_t, void *);
typedef void (*sensor_sw_version_req_f)(ss_sensor_t, void *);
typedef void (*sensor_hw_version_req_f)(ss_sensor_t, void *);
#endif

typedef struct {
    start_scanning_req_f start_scanning;
    stop_scanning_req_f  stop_scanning;
    sensor_subscribe_req_f sensor_subscribe;
    sensor_unsubscribe_req_f sensor_unsubscribe;
    sensor_set_property_req_f    sensor_set_property;
    sensor_get_property_req_f    sensor_get_property;
    sensor_calibration_req_f    sensor_calibration;
#ifdef CONFIG_SENSOR_SERVICE_RESERVED_FUNC
    sensor_pair_req_f    sensor_pair;
    sensor_start_req_f   sensor_start;
    sensor_unpair_req_f sensor_unpair;
    sensor_device_id_req_f sensor_device_id;
    sensor_product_id_req_f sensor_product_id;
    sensor_manufacture_id_req_f sensor_manufacturer_id;
    sensor_serial_number_req_f sensor_serial_number;
    sensor_sw_version_req_f sensor_sw_version;
    sensor_hw_version_req_f sensor_hw_version;
#endif
} svc_platform_handler_t;

#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == ARC_SENSOR_CORE_SERVICE)
svc_platform_handler_t* get_arc_svc_handler(void);
service_t *get_arc_svc(void);
#elif defined(SENSOR_SERVICE) && (SENSOR_SERVICE == QRK_SENSOR_SERVICE)
svc_platform_handler_t* get_quark_svc_handler(void);
service_t *get_quark_svc(void);
#elif defined(SENSOR_SERVICE) && (SENSOR_SERVICE == HOST_SENSOR_SERVICE)
svc_platform_handler_t* get_host_svc_handler(void);
service_t *get_host_svc(void);
#endif

#include "infra/log.h"
/* Sensor Service Trace Control. 0:Disable 1:Enable */
#define SENSOR_SVC_TRACE                   0
#include "infra/panic.h"
#define force_panic() panic(-1)

#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == ARC_SENSOR_CORE_SERVICE)
#include "ipc_comm.h"
#include "services/sensor_service/sensor_svc_sensor_core.h"
#endif

#define SS_PRINT_LOG(format,...)        \
    do {                                \
        pr_info(LOG_MODULE_SS_SVC,"[%s]"format,__func__,##__VA_ARGS__); \
    } while (0)

#define SS_PRINT_ERR(format,...)        \
    do {                                \
        pr_error(LOG_MODULE_SS_SVC,"[%s]"format,__func__,##__VA_ARGS__); \
    } while (0)

static inline service_t *get_svc()
{
#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == ARC_SENSOR_CORE_SERVICE)
    return get_arc_svc();
#elif defined(SENSOR_SERVICE) && (SENSOR_SERVICE == QRK_SENSOR_SERVICE)
    return get_quark_svc();
#elif defined(SENSOR_SERVICE) && (SENSOR_SERVICE == HOST_SENSOR_SERVICE)
    return get_host_svc();
#endif
    return NULL;
}

static inline svc_platform_handler_t *get_svc_handler()
{

#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == ARC_SENSOR_CORE_SERVICE)
    return get_arc_svc_handler();
#elif defined(SENSOR_SERVICE) && (SENSOR_SERVICE == QRK_SENSOR_SERVICE)
    return get_quark_svc_handler();
#elif defined(SENSOR_SERVICE) && (SENSOR_SERVICE == HOST_SENSOR_SERVICE)
    return get_host_svc_handler();
#endif
    return NULL;
}
#endif
