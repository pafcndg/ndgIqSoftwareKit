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

#ifndef _PLIST_H
#define _PLIST_H

#include "common.h"

struct pshlist_entry {
    struct pshlist_entry *prev;
    struct pshlist_entry *next;
};

#define pshlist_for_each_entry(entry, head) \
    for ((entry) = (head)->next; \
            (entry) != (head); \
            (entry) = (entry)->next)

static inline void pshlist_init(struct pshlist_entry *entry)
{
    entry->prev = entry;
    entry->next = entry;
}

static inline void pshlist_add_tail(struct pshlist_entry *new_entry,
        struct pshlist_entry *head)
{
    new_entry->next = head;
    new_entry->prev = head->prev;
    head->prev->next = new_entry;
    head->prev = new_entry;
};

static inline void pshlist_remove(const struct pshlist_entry *rm_entry,
        struct pshlist_entry *head)
{
    struct pshlist_entry *entry;

    pshlist_for_each_entry(entry, head) {
        if (rm_entry == entry) {
            entry->prev->next = entry->next;
            entry->next->prev = entry->prev;
            return;
        }
    }
    ASSERT(0);
}

#endif
