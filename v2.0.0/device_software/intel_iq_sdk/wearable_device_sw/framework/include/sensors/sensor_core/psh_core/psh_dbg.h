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

#ifndef _PSH_DBG_H_
#define _PSH_DBG_H_

#include "common.h"
#include "sensor_def.h"
#include "cfw/cfw.h"
#include "infra/log.h"
#include "util/compiler.h"


#define PSH_DBG_ALL ((u16)-1)
#define PSH_DBG_FATAL   ((u16)(0x1 << 0x0))
#define PSH_DBG_ERR ((u16)(0x1 << 0x1))
#define PSH_DBG_WARN    ((u16)(0x1 << 0x2))
#define PSH_DBG_INFO    ((u16)(0x1 << 0x3))
#define PSH_DBG_DBG ((u16)(0x1 << 0x4))
#define PSH_DBG_CTRACE  ((u16)(0x1 << 0x5)) /* config path tracing */
#define PSH_DBG_DTRACE  ((u16)(0x1 << 0x6)) /* data path tracing */
#define PSH_DBG_MTRACE  ((u16)(0x1 << 0x7)) /* mutex_exec tracing */


#define PSH_DBGOUT_ALL  ((u16)-1)
#define PSH_DBGOUT_UART ((u16)(0x1 << 0x0))
#define PSH_DBGOUT_IA   ((u16)(0x1 << 0x1))

#define CTRACE_SCORE    ((u16)0x0) /* Configurig by Sensor Core */
#define CTRACE_S_CFG    ((u16)0x1) /* Configuring by cfg callback */
#define CTRACE_HW_PD    ((u16)0x2) /* HW Power Down */
#define CTRACE_HW_PU    ((u16)0x3) /* HW Power Up */
#define CTRACE_HW_PI    ((u16)0x4) /* HW Power Idle */
#define CTRACE_C_IN ((u16)0x5) /* Configure in */
#define CTRACE_C_OUT    ((u16)0x6) /* Configure out */
#define CTRACE_GO_IDLE  ((u16)0x7) /* Go into idle */
#define CTRACE_OUT_IDLE ((u16)0x8) /* Go out of idle */
#define CTRACE_SUSPEND  ((u16)0x9) /* Go to suspen */
#define CTRACE_RESUME   ((u16)0xA) /* to resume */

#define DTRACE_MISC ((u16)0x0)
#define DTRACE_D_RDY    ((u16)0x1)
#define DTRACE_D_IN ((u16)0x2)
#define DTRACE_D_OUT    ((u16)0x3)
#define DTRACE_DUMP ((u16)0x4)

#define MTRACE_TRY_LOCK ((u16)0x0)
#define MTRACE_LOCK ((u16)0x1)
#define MTRACE_UNLOCK   ((u16)0x2)

#define DBG_MSG_MAX_SIZE 256

struct trace_data {
    u32 timestamp;
    u16 type;
    u16 event;
    u8 sensor_id;
    u8 sensor_cnt;
} __packed;

#define OUTPUT_MSG_HEAD 0xF1
#define TYPE_TRACE_DATA 0xF2
struct output_msg {
    u8 head;
    u8 type;
    u16 length;
    union {
        struct trace_data data;
    } msg;
} __packed;

void psh_dbg_set_mask(u16 mask_out, u16 mask_level);

void psh_dbg_get_mask(u16 *mask_out, u16 *mask_level);

const char *_get_sensor_str(u8 sid);
const char *_get_evt_str(u16 level, u16 evt);

void psh_output(u16 level, const char *fmt, ...);

#define psh_log(fmt, arg...) \
    pr_debug(LOG_MODULE_PSH_CORE,"[%s]"fmt, __func__,## arg)


//#define psh_output_msg(level, sid, evt, fmt, arg...)    ((void)0)
#define psh_output_msg(level, sid, evt, fmt, arg...) \
    pr_debug(LOG_MODULE_PSH_CORE,"([%s]ms=%u,sid=%d)"fmt, \
                                                __func__,\
                                    getms(),sid, ## arg)

/* psh_fatal is the only one could be used in ISR and
 thread with high priority */
#define psh_fatal2(sid, fmt, arg...) \
    pr_error(LOG_MODULE_PSH_CORE,"([%s]ms=%u,sid=%d): "fmt, \
                                                __func__,\
                                        getms(),sid, ## arg)
#define psh_fatal(bs, fmt, arg...) \
    psh_fatal2(bs->sid, fmt, ## arg)

#define psh_info2(sid, fmt, arg...) \
    pr_info(LOG_MODULE_PSH_CORE,"([%s]ms=%u,sid=%d): "fmt, \
                                                 __func__,\
                                        getms(),sid, ## arg)
#define psh_info(bs, fmt, arg...) \
    psh_info2(bs->sid, fmt, ## arg)

#define psh_err2(sid, fmt, arg...) \
    pr_error(LOG_MODULE_PSH_CORE,"([%s]ms=%u,sid=%d): "fmt, \
                                                 __func__,\
                                        getms(),sid, ## arg)
#define psh_err(bs, fmt, arg...) \
    psh_err2(bs->sid, fmt, ## arg)

#define psh_warn2(sid, fmt, arg...) \
    pr_warning(LOG_MODULE_PSH_CORE,"([%s]ms=%u,sid=%d): "fmt, \
                                                    __func__,\
                                        getms(),sid, ## arg)
#define psh_warn(bs, fmt, arg...) \
    psh_warn2(bs->sid, fmt, ## arg)

#if defined(CONFIG_VERBOSE_ON)
#define psh_dbg2(sid, fmt, arg...) \
    psh_output_msg(PSH_DBG_DBG, sid, \
            PSH_DBG_DBG, fmt, ## arg)
#define psh_dbg(bs, fmt, arg...) \
    psh_dbg2(bs->sid, fmt, ## arg)
#endif

/* sensor trace */
#if defined(CONFIG_TRACE_ON)
#define psh_output_trace_info(level, sid, evt, context, fmt, arg...) \
        psh_output(level, "[%u,%s,%s,%s]: "fmt, getus(), \
                _get_sensor_str(sid), \
                _get_evt_str(level, evt), \
                _get_sensor_str(context),\
                ## arg)

#if defined(LINUX_APP)
#define psh_output_trace(level, sid, evt, context, fmt, arg...) \
        psh_output_trace_info(level, sid, evt, context, fmt, ## arg)
#else
void psh_traceout(u16 level, u16 evt, u8 sid, u8 context);
#define psh_output_trace(level, sid, evt, context, fmt, arg...) \
        psh_traceout(level, evt, sid, context)
#endif

#define psh_ctrace(bs, evt, fmt, arg...) {\
    if (CTRACE_##evt == CTRACE_SCORE \
           || CTRACE_##evt == CTRACE_S_CFG)\
        psh_output_trace_info(PSH_DBG_CTRACE, bs->sid, \
            CTRACE_##evt, bs->ctx.vthread, fmt, ## arg);\
    else\
        psh_output_trace(PSH_DBG_CTRACE, bs->sid, \
            CTRACE_##evt, bs->ctx.vthread, fmt, ## arg);\
}

#define psh_dtrace(bs, evt, fmt, arg...) \
    psh_output_trace(PSH_DBG_DTRACE, bs->sid, \
            DTRACE_##evt, bs->ctx.vthread, fmt, ## arg)

#if defined(CONFIG_MTRACE_ON)
#define psh_mtrace(bs, evt, fmt, arg...) \
    psh_output_trace(PSH_DBG_MTRACE, bs->sid, \
            MTRACE_##evt, bs->ctx.vthread, fmt, ## arg)
#endif
#endif /* CONFIG_TRACE_ON */

#if defined(CONFIG_DATA_DUMP_ON)
#define psh_ddump2(sid, fmt, arg...) \
    psh_output_msg(PSH_DBG_DTRACE, sid, DTRACE_DUMP, \
            fmt, ## arg)
#define psh_ddump(bs, fmt, arg...) \
    psh_ddump2(bs->sid, fmt, ## arg)
#endif

#ifndef psh_dbg
#define psh_dbg(...) ((void)0)
#endif

#ifndef psh_dbg2
#define psh_dbg2(...) ((void)0)
#endif

#ifndef psh_ctrace
#define psh_ctrace(...) ((void)0)
#endif
#ifndef psh_dtrace
#define psh_dtrace(...) ((void)0)
#endif
#ifndef psh_mtrace
#define psh_mtrace(...) ((void)0)
#endif

#ifndef psh_ddump
#define psh_ddump(...) ((void)0)
#endif

#ifndef psh_ddump2
#define psh_ddump2(...) ((void)0)
#endif

void psh_trace_ctrl(int to_mute);
void psh_trace_flush(void);
#endif
