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

#ifndef _COMMON_H
#define _COMMON_H

// #include "autoconf.h"
#include "os/os.h"
#include "cfw/cfw.h"
#include "sensors/phy_sensor_api/phy_sensor_common.h"
#include "util/compiler.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;

#define BITMASK_64
#ifdef BITMASK_64
typedef u64 bitmask_t;
#else
typedef unsigned int bitmask_t;
#endif

struct counter {
    unsigned int gpio_counter;
    unsigned int dma_counter;
    unsigned int i2c_counter;
    unsigned int print_counter;
};

#include "nano_api.h"

extern struct counter module_counter;

#define E_GENERAL       ((int)(-1))
#define E_NOMEM         ((int)(-2))
#define E_PARAM         ((int)(-3))
#define E_BUSY          ((int)(-4))
#define E_HW            ((int)(-5))
#define E_NOSUPPORT     ((int)(-6))
#define E_RPC_COMM      ((int)(-7))
#define E_LPE_COMM      ((int)(-8))
#define E_CMD_ASYNC     ((int)(-9))
#define E_CMD_NOACK     ((int)(-10))
#define E_LBUF_COMM     ((int)(-11))

#if 0
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikly
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#endif

/*Make reporter_interview, buff_delay valid, config a buff to pack the sensor-data*/
#define CONFIG_BATCHED_MODE

#include "sc_exposed.h"
#include <stdint.h>
#include <string.h>


static inline void msleep(u32 ms)
{
    int start = getms();
    while ((int)getms() - start < ms);
}

#ifndef MAX_POLL_FREQ
#define MAX_POLL_FREQ (100)
#endif

#define OMS_1MS ((u32)8)
#define OMS_1SEC_TIME_BASE ((u32)(1000 * OMS_1MS))
#define MS_TO_OMS(ms) ((u32)ms << 3)
static inline u32 OMS_TO_MS(u32 oms)
{
    u32 left = oms & (u32)0x7;
    oms = (oms >> 3);
    if (left)
        oms++;
    return oms;
}

/* stddef is necessary for defination of soffsetof */
#include <stddef.h>
#ifndef container_of
#define container_of(ptr, type, member) ({\
    (type *)( (char *)(ptr) - offsetof(type,member) );})
#endif
/* each main.c should implement IPC/DDR sync functions */
struct loop_buffer;
int psh_setup_ddr(struct loop_buffer *lbuf, const void *param);

#define MAX_POLL_TIMEOUT ((int)(~0U>>1))

#define DO_PRAGMA(x)        _Pragma(#x)
#define TODO(x)         DO_PRAGMA(message("TODO - " #x))
#define FIXME(x)        DO_PRAGMA(message("FIXME - " #x))

typedef void (*initfn_t)(void);
#define define_initcall(fn) \
    static initfn_t __initfn_##fn __used \
        __section(".pshinit.fn") = fn
extern char _s_pshinit_fn[], _e_pshinit_fn[];

#define define_drvinit(name) \
    static void* __drvinit_##name __used \
        __section(".pshinit.drv") = &name
extern char _s_drvinit[], _e_drvinit[];

//MAYBE_Q
#define CONFIG_CODE_SECTION
#define CONFIG_CODE_SECTION_INIT

#ifdef CONFIG_CODE_SECTION /* decide if we use section for code / data colocalization */
#ifdef CONFIG_CODE_SECTION_INIT
#define __pshinit __section(".pshinit")
#define __pshinitdata __used __section(".pshinit.data")
extern char _s_pshinit[], _e_pshinit[];
void free_init_mem();
#else
#define __pshinit
#define __pshinitdata __section(".pshinit.data")
#endif /* CONFIG_CODE_SECTION_INIT */
#else
#define __pshinit
#define __pshinitdata __section(".pshinit.data")
#endif /* CONFIG_CODE_SECTION */

/* declare where to allocate memory */
#define MEM_NORMAL  0   /* use normal memory to allocate */
#define MEM_INIT    1   /* reuse init section to allocate,
                available only for CONFIG_CODE_SECTION_INIT */


#define SNR_NAME_MAX_LEN 6
#define BOARD_NAME_MAX_LEN 16

#include "psh_dbg.h"
#include "options.h"
#include "infra/panic.h"

static inline void* ZMALLOC(int size, int flag)
{

    char *p;
    OS_ERR_TYPE err = E_OS_OK;
    int flags = interrupt_lock();

    p = (char* )balloc(size, &err);
    if(!p){
        if(err < 0){
            psh_err2(0,"[ZMALLOC ERR=%d]\n",(int)err);
            panic(-1);
        }
        psh_err2(0,"Cannot allocate memory!\n");
    }
    if(flag == MEM_INIT){
        memset(p, 0, size);
    }
    interrupt_unlock(flags);
    return p;

}

static inline void FREE(void *p)
{
    int flags = interrupt_lock();
    bfree(p);
    interrupt_unlock(flags);
}

#endif
