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

#ifndef CONFIG_DYNAMIC_INT_STUBS

#include <arch/x86/asm.h>
    /* exports (internal APIs) */
    GTEXT(_loApicTimerIntStub)

	/* externs (internal APIs) */

    GTEXT(_IntEnt)
    GTEXT(_IntExit)
    GTEXT(_LoApicIntEoi)
    GTEXT(__IoApicIntEoi)
    GTEXT(_mbxIntIsr)
    GTEXT(_mbxIntStub)

SECTION_FUNC(TEXT, _mbxIntStub)
    call    _IntEnt			/* Inform kernel interrupt has begun */
    pushl   $0			    /* Push dummy parameter */
    call    mbxIsr	        /* Call actual interrupt handler */
    call    _ioapic_eoi	/* Inform ioapic interrupt is done */
    addl    $4, %esp		/* Clean-up stack from push above */
    jmp     _IntExit		/* Inform kernel interrupt is done */

#endif /* !CONFIG_DYNAMIC_INT_STUBS */
