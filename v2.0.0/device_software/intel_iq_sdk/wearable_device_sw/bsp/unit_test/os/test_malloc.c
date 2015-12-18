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

/*
 * OS abstraction / test malloc API (memory allocation)
 */

#include "os/os.h"
#include "os/os_types.h"
#include "utility.h"
#include "util/cunit_test.h"

struct mem_pool
{
	uint32_t nb_elem;
	uint32_t size;
};

#define DECLARE_MEMORY_POOL(index, sz, count) \
	[index] = { \
		.nb_elem = count, \
		.size = sz \
		},

static const struct mem_pool all_pools[] = {
#include "memory_pool_list.def"
};
#undef DECLARE_MEMORY_POOL

#define NB_OF_MEMORY_POOL DIM(all_pools)

void isr_clbk_test_free(void* data);
void isr_clbk_test_malloc(void* data);

void test_malloc_and_free_1(void)
{
	OS_ERR_TYPE err = E_OS_OK;
	uint8_t * tab[50] = {NULL};
	uint8_t i;

	CU_ASSERT("test not valid if:", all_pools[0].nb_elem<DIM(tab));

	for (i=0; i<all_pools[0].nb_elem; i++)
	{
	   tab[i] = balloc(all_pools[0].size, &err);
	   CU_ASSERT("balloc not successful.", err == E_OS_OK);
	}

	err = bfree(tab[0]);
	CU_ASSERT("free not successful.", err == E_OS_OK);

	tab[0] = balloc(all_pools[0].size, &err);
	CU_ASSERT("balloc not successful.", err == E_OS_OK);

	for (i=0; i<all_pools[0].nb_elem; i++)
	{
		err = bfree(tab[i]);
		CU_ASSERT("free not successful.", err == E_OS_OK);
	}
}

/* This test is really buggy, it relies on specific pool alignment and
 on a specific memory_pool_list.def file */
void test_malloc_and_free_2(void)
{
	uint8_t * p;
	static uint8_t * ptr[NB_OF_MEMORY_POOL] = {0};
	static uint32_t count[NB_OF_MEMORY_POOL] = {0};
	static uint32_t  size[NB_OF_MEMORY_POOL] = {0};  /* element/block size */
	OS_ERR_TYPE err = E_OS_OK;
	uint8_t i,j,k, idx;
	uint32_t index = 0;
	cu_print("NB_OF_MEMORY_POOL %d\n", NB_OF_MEMORY_POOL);

	/* run the test twice */
	for(k=0; k<2; k++)
	{
		uint32_t previous_max_size= 0xFFFFFFFF;

		/* for each pool allocate all the memory by starting by the largest pool and set a pattern */
		for(i=0; i<NB_OF_MEMORY_POOL; i++)
		{
			uint32_t max_size = 0;
			uint32_t not_allocated=0;

			/* sort the pool from the largest to the smallest one */
			for(j=0; j<NB_OF_MEMORY_POOL; j++)
			{
				if(all_pools[j].size > max_size && all_pools[j].size < previous_max_size)
				{
					max_size = all_pools[j].size;
					index = j;
					size[i]=all_pools[j].size;
				}
			}
			previous_max_size = max_size;

			/* allocate all the memory for this pool */
			for (j=0; j<all_pools[index].nb_elem; j++)
			{
				p = balloc(all_pools[index].size, &err);
				if(p)
				{
					cu_print("allocated %d bytes addr %x\n", all_pools[index].size, p);
					ptr[i] = p;
					count[i]++;
					CU_ASSERT("no error returned.", err == E_OS_OK );

					/* fill the block */
					for (idx=0; idx<all_pools[index].size; idx++)
					{
						/* write pattern */
						p[idx] = (uint8_t)(0xFF & (((uint32_t)(p+idx))+idx));
					}
				}
				else
				{
					not_allocated++;
					CU_ASSERT("no error returned.", err == E_OS_ERR_NO_MEMORY );
				}
			}

			CU_ASSERT("check number of allocated blocks are correct", all_pools[index].nb_elem == (count[i] + not_allocated));
			p = ptr[i];
		}

		/* free each pool */
		for(i=0; i<NB_OF_MEMORY_POOL; i++)
		{
			p = ptr[i];
			/* verify each block */
			for(j=0; j<count[i]; j++)
			{
				for (idx=0; idx<size[i]; idx++)
				{
					CU_ASSERT("verif data", (*(p+idx)) == (uint8_t)(((0xFF)&(((uint32_t)(p+idx))+idx)))); /* check pattern */
					p[idx] = 0; /* clear memory data */
				}
				p-=size[i]; /* go to next block/element */
			}

			/* free all the memory */
			do{
				err = bfree(ptr[i]);
				if(err == E_OS_OK)
				{
					count[i]--;
					ptr[i]-=size[i];
				}
			}while(err == E_OS_OK && count[i] != 0);

			CU_ASSERT("free fail", count[i] == 0);
		}
	}
}

/* check malloc functions in an interrupt context */
void test_malloc_in_interruption_ctx(void)
{
	OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;
	uint32_t address = 0;
	uint8_t * ptr1 = balloc(all_pools[1].size, &err);
	uint8_t * ptr2 = balloc(all_pools[2].size, &err);
	uint8_t * ptr3 = balloc(all_pools[2].size, &err);

	CU_ASSERT("malloc not successful.", ptr1 != NULL && ptr2 != NULL && ptr3 != NULL);

	/* test free from an interrupt context */

	interrupt_param_t it_parm = {E_CALLBACK_FREE, NULL};

	it_parm.data = ptr1;
	trigger_mbx_isr(&it_parm);				 /* trigger interrupt */
	wait_mbx_isr(&it_parm);
	err = bfree(ptr1);
	CU_ASSERT("free error", err == E_OS_ERR);

	it_parm.data = ptr2;
	trigger_mbx_isr(&it_parm);				 /* trigger interrupt */
	wait_mbx_isr(&it_parm);
	err = bfree(ptr2);
	CU_ASSERT("free error", err == E_OS_ERR);

	it_parm.data = ptr3;
	trigger_mbx_isr(&it_parm);				 /* trigger interrupt */
	wait_mbx_isr(&it_parm);
	err = bfree(ptr3);
	CU_ASSERT("free error", err == E_OS_ERR);

	/* test malloc from an interrupt context */
	it_parm.type = E_CALLBACK_MALLOC;
	it_parm.data = &address;

	trigger_mbx_isr(&it_parm);				  /* trigger interrupt */
	wait_mbx_isr(&it_parm);
	err = bfree((void*)address);
	CU_ASSERT("free error", err == E_OS_OK);

	trigger_mbx_isr(&it_parm);				  /* trigger interrupt */
	wait_mbx_isr(&it_parm);
	err = bfree((void*)address);
	CU_ASSERT("free error", err == E_OS_OK);

	trigger_mbx_isr(&it_parm);				  /* trigger interrupt */
	wait_mbx_isr(&it_parm);
	err = bfree((void*)address);
	CU_ASSERT("free error", err == E_OS_OK);
}


/* test free in isr context */
void isr_clbk_test_free(void* data)
{
	OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;

	if(data != NULL)
	{
		err = bfree(data);
		CU_ASSERT("free error", err == E_OS_OK);
	}
	else
	{
		CU_ASSERT("Unexpected", 0);
	}
}

/* test malloc in isr context */
void isr_clbk_test_malloc(void* data)
{
	static uint8_t idx=0;
	OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;
	*(uint32_t*)data = (uint32_t)balloc(all_pools[idx%3].size, &err);
	CU_ASSERT("isr malloc error", err == E_OS_OK);
	idx++;
}
