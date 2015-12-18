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
 * \file nano_main.c
 *
 * This file implements the main function for Zephyr NanoKernel builds
 * It is run only on Quark
 */

#include "os/os.h"
#include "nanokernel.h"

extern void main_task(int i1, int i2) ;

#define UNIT_TEST_FIBER_STACK_SIZE   512
#define UNIT_TEST_FIBER_PRIORITY      50
#define UNIT_TEST_FIBER_OPTIONS        0

/* allocate stack for the Unit Test fiber */
static uint32_t g_UnitTestFiberStack [UNIT_TEST_FIBER_STACK_SIZE/sizeof(uint32_t)] ;

void main (void)
{
    os_init();

    /* start main_task in a new fiber */
    task_fiber_start (
        (char*) g_UnitTestFiberStack,
        UNIT_TEST_FIBER_STACK_SIZE,
        main_task,
        0,0,
        UNIT_TEST_FIBER_PRIORITY,
        UNIT_TEST_FIBER_OPTIONS );

    /* TODO: launch application tasks/fiber
     *  according to an external list of
     *  functions to be defined */
}
