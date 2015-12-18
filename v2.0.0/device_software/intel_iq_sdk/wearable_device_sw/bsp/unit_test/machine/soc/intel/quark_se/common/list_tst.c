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

#include "util/cunit_test.h"
#include <stdlib.h>
#include "util/list.h"

#define LIST_MAGIC_KEY 0x600D7157

struct list_test_item {
    list_t list;
    int id;
};
static int list_count;

static void lt_check(void *item, void *priv)
{
    if ((uint32_t)priv == LIST_MAGIC_KEY) {
        list_count += ((struct list_test_item*)item)->id;
    }
}

static int lt_rm_impair(void *item, void *priv)
{
    return ((struct list_test_item*)item)->id & 1;
}

static bool lt_find_specific_item(list_t *item,void *param)
{
    return ((struct list_test_item*)item)->id  == (uint32_t)param;
}

void list_test(void)
{
    int i;
    list_head_t lt_head;
    struct list_test_item lt_items[10];

    for (i=0; i<10; i++) {
        lt_items[i].id = i;
    }

    list_init(&lt_head);

    list_add_head(&lt_head, (list_t*)&lt_items[2]);
    list_add_head(&lt_head, (list_t*)&lt_items[1]);
    list_add_head(&lt_head, (list_t*)&lt_items[5]);
    list_add_head(&lt_head, (list_t*)&lt_items[6]);
    list_add_head(&lt_head, (list_t*)&lt_items[7]);
    list_add_head(&lt_head, (list_t*)&lt_items[8]);
    list_add(&lt_head, (list_t*)&lt_items[3]);
    list_add(&lt_head, (list_t*)&lt_items[4]);
    list_add(&lt_head, (list_t*)&lt_items[9]);

    // Check list
    list_count = 0;
    list_foreach(&lt_head, lt_check, (void*)LIST_MAGIC_KEY);
    CU_ASSERT("bad list content", list_count == 1+2+3+4+5+6+7+8+9);

    // Find first item 4
    list_t *ret = list_find_first(&lt_head, lt_find_specific_item, (void*)4);
    CU_ASSERT("bad item found", ((struct list_test_item*)ret)->id == 4);

    // Test remove items 3 and 4
    list_remove(&lt_head, (list_t*)&lt_items[3]);
    list_remove(&lt_head, (list_t*)&lt_items[4]);
    // Check list
    list_count = 0;
    list_foreach(&lt_head, lt_check, (void*)LIST_MAGIC_KEY);
    CU_ASSERT("bad list content", list_count == 1+2+5+6+7+8+9);

    // Test list_foreach_del on impair items
    list_foreach_del(&lt_head, lt_rm_impair, NULL);
    // Check list
    list_count = 0;
    list_foreach(&lt_head, lt_check, (void*)LIST_MAGIC_KEY);
    CU_ASSERT("bad list content", list_count == 2+6+8);

    // Remove remaining items
    list_remove(&lt_head, (list_t*)&lt_items[2]);
    list_remove(&lt_head, (list_t*)&lt_items[6]);
    list_remove(&lt_head, (list_t*)&lt_items[8]);

    // Check list
    list_count = 0;
    list_foreach(&lt_head, lt_check, (void*)LIST_MAGIC_KEY);
    CU_ASSERT("bad list content", list_count == 0);
}
