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

/**
 * \file utility.c
 *
 * Implement utility functions for the CUnit tests
 */

#include "os/os.h"
#include "utility.h"
#include "board.h"
#include "test_task.h"
#include "util/cunit_test.h"

#if (BSP == quark_se-x86)
#include "machine.h"
#endif

#include "nanokernel.h"
#if   defined (ZEPHYR_NANO)
#include "nanokernel.h"

#elif defined (ZEPHYR_MICRO)
#include "microkernel.h"
#include <zephyr.h>

#elif defined (OS_FREERTOS )

#else
#error "No OS selected"

#endif

//#define MBX_CTRL(_x_) (* (volatile unsigned int *)(0xB0800000 + 0xA00 + (0x18*(_x_))) )
//#define MBX_STS(_x_)  (* (volatile unsigned int *)(0xB0800000 + 0xA00 + (0x18*(_x_)) + 20))
static interrupt_param_t * isr_param = NULL;
static void (*ipc_msg_handler)(void) = NULL;


#if defined ZEPHYR_NANO

#define TEST_FIBER_STACK_SIZE   512
#define TEST_FIBER1_PRIORITY    30
#define TEST_FIBER2_PRIORITY    20
#define TEST_FIBER_OPTIONS      0

/* allocate stacks for the Unit Test fibers */
static uint32_t g_TestFiberStack1 [TEST_FIBER_STACK_SIZE/sizeof(uint32_t)];
static uint32_t g_TestFiberStack2 [TEST_FIBER_STACK_SIZE/sizeof(uint32_t)];

#endif
void ipc_msg_handler_set(void (*hdl)(void)) {
	ipc_msg_handler = hdl;
}
/**
 * \brief Starts the test tasks
 */
void cunit_start_tasks(void)
{
#if defined ZEPHYR_NANO

    fiber_fiber_start (
        (char*) g_TestFiberStack1,
        TEST_FIBER_STACK_SIZE,
        task1,
        0,0,
        TEST_FIBER1_PRIORITY,
        TEST_FIBER_OPTIONS );

    fiber_fiber_start (
        (char*) g_TestFiberStack2,
        TEST_FIBER_STACK_SIZE,
        task2,
        0,0,
        TEST_FIBER2_PRIORITY,
        TEST_FIBER_OPTIONS );



#elif defined  ZEPHYR_MICRO
    task_start(Task1);
    task_start(Task2);

#elif defined OS_FREERTOS

#endif
}

/**
 * \brief Trigger the test-dedicated IRQ
 *
 * This function uses target-specific assembly
 * to trigger an exception
 */
void trigger_test_interrupt (void)
{
#ifdef __GNUC__
#ifndef CONFIG_ARC
    __asm__ ("INT %0" : : "i" ((uint8_t) UNIT_TESTS_IRQ_LINE));
#else
    ;
#endif
#else
#warning "trigger_test_interrupt is not implemented for this compiler (requires the correct inline ASM syntax)"
#endif
}


/* mbxIsr */
#ifndef CONFIG_ARC
extern void _mbxIntStub(void);
NANO_CPU_INT_REGISTER(_mbxIntStub, 32+21, 3); /* 21 == mailbox */
#endif

inline void mbx_status_reset(void)
{
    /* Reset mailbox status */
    MBX_STS(4) = 0x3;
}

/* this function is called by the mailbox interrupt routine: _mbxIntStub */
void mbxIsr(int param)
{
    extern void isr_clbk_test_mutex(void* data);
    extern void isr_clbk_test_sema(void* data);
    extern void isr_clbk_test_queue(void* data);
    extern void isr_clbk_test_irq(void* data);
    extern void isr_clbk_test_malloc(void* data);
    extern void isr_clbk_test_free(void* data);
    if(ipc_msg_handler)
		ipc_msg_handler();
    mbx_status_reset();
    if(isr_param)
    {
        interrupt_param_t * it = isr_param;

        switch(it->type) {
            case E_CALLBACK_MUTEX: {isr_clbk_test_mutex(it->data);} break;
            case E_CALLBACK_SEMA:  {isr_clbk_test_sema(it->data);}  break;
            case E_CALLBACK_QUEUE: {isr_clbk_test_queue(it->data);} break;
            case E_CALLBACK_IRQ:   {isr_clbk_test_irq(it->data);}   break;
            case E_CALLBACK_MALLOC: {isr_clbk_test_malloc(it->data);} break;
            case E_CALLBACK_FREE: {isr_clbk_test_free(it->data);} break;
            default: { cu_print("unexpected ISR received, param = %d\n",param);} break;
        }
        it->isr_called = true;
        isr_param = NULL;
    }
}

void trigger_mbx_isr(interrupt_param_t * param)
{
    param->isr_called = false;
    isr_param = param;
    /* Trigger Mailbox interrupt */
    MBX_CTRL(4) = 0x80000000;
}

bool wait_mbx_isr(interrupt_param_t * param)
{
    unsigned int timeout = 0x1fffff; // about 1 sec timeout
    while(timeout-- && param->isr_called == false){};
    return param->isr_called;
}

void test_interrupt_init(void)
{
#ifndef CONFIG_ARC
    _SysIntVecProgram (32+21, 21);
    interrupt_enable(21);
    MMIO_REG_VAL(IO_REG_MAILBOX_INT_MASK) &= ~(1 << 4);
#endif
}






