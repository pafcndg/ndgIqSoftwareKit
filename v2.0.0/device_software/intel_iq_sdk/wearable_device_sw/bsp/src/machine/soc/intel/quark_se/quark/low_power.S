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

#define _ASMLANGUAGE

#include <arch/x86/asm.h>

/* exports (external APIs) */

GTEXT(quark_deep_sleep)

.equ  DEEP_SLEEP_MAGIC_NUMBER, (0xabcd1234)
.equ  DEEP_SLEEP_SAVE_CTXT, (0xa800e000)

.equ  CR0_CACHE_DISABLE, 0x040000000
.equ  CR0_NO_WRITE,      0x020000000

.align 4
SECTION_FUNC(TEXT, quark_deep_sleep)

    pushf                                  /* Save EFLAG register */
    pusha                                  /* Save all GPs registers */

    movl %cr0, %eax
    orl $(CR0_CACHE_DISABLE + CR0_NO_WRITE), %eax
    invd
    movl    %eax, %cr0

    movl $DEEP_SLEEP_SAVE_CTXT, %edx       /* Load dump location address in edx */
    movl $DEEP_SLEEP_MAGIC_NUMBER,-4(%edx) /* Save magic key */
    movl $resume_trap,-8(%edx)             /* Save resume address to jump on in bootstrap */
    movl %ebp,-0xc(%edx)                   /* Save ebp */
    movl %esp,-0x10(%edx)                  /* Save esp */
    sidt -0x18(%edx)                       /* Save IDT register */

    movl $13, %eax                         /* Set bit 13, to enable sleep in PM1C register */
    lock bts %eax,0xb0800518               /* Write to Power Management 1 Control and trigger sleep */
.align 4
wait_for_deep_sleep:
    hlt                                    /* loop until deep sleep */
    jmp wait_for_deep_sleep


.align 4
BRANCH_LABEL(resume_trap)
    movl $DEEP_SLEEP_SAVE_CTXT, %edx       /* Load dump location address in edx */
    lidt -0x18(%edx)                       /* Restore IDT */
    movl $0x600df00d,-4(%edx)              /* Clear magic key */
    movl -0xc(%edx),%ebp                   /* Restore ebp */
    movl -0x10(%edx),%esp                  /* Restore ebp */
    popa                                   /* Restore all GPs registers */
    popf                                   /* Restore EFLAG register */
    ret
