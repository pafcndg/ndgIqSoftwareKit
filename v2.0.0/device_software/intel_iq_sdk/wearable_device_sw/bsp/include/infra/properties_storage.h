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

#ifndef __PROPERTIES_STORAGE_H
#define __PROPERTIES_STORAGE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup infra_properties_storage Properties storage
 *
 * The property storage API allows to store key/values pairs on non-volatile
 * memory. Each value can be flagged as persistent to factory reset or not.
 *
 * The API of the property storage is synchronous, and not thread safe.
 *
 * @ingroup infra
 * @{
 */

/** Error codes */
typedef enum {
	PROPERTIES_STORAGE_SUCCESS = 0,       /*!< Operation succeed */
	PROPERTIES_STORAGE_BOUNDS_ERROR,      /*!< Overflow */
	PROPERTIES_STORAGE_IO_ERROR,          /*!< I/O error */
	PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR,  /*!< Key not found */
	PROPERTIES_STORAGE_INVALID_ARG,       /*!< Invalid argument */
} properties_storage_status_t;

/** Maximum size for the value of a property in byte */
#define PROPERTIES_STORAGE_MAX_VALUE_LEN 256

/** Maximum number of properties that can be stored in the store  */
#define PROPERTIES_STORAGE_MAX_NB_PROPERTIES 32

/**
 * Initialize the property storage.
 */
void properties_storage_init(void);

/**
 * Format property storage partitions.
 *
 * It also re-initialize the property storage.
 */
void properties_storage_format_all(void);

/**
 * Set the value for a property.
 *
 * If the property didn't previously exists, it is created.
 * This function blocks until completion.
 *
 * @param factory_reset_persistent set to true to create a reset persistent
 * property. This argument is only used if the property is created. To change a
 * property flag it needs to be deleted first.
 * @param key the key of the property
 * @param buf the new value to store
 * @param len length of buf in bytes
 * @return status code:
 *  CBUFFER_STORAGE_BOUNDS_ERROR: buffer length is larger than limit, or store
 *    is full
 *  PROPERTIES_STORAGE_IO_ERROR:  write failure, element is not stored
 *  CBUFFER_STORAGE_SUCCESS:      store succeed
 */
properties_storage_status_t properties_storage_set(uint32_t key,
	const uint8_t* buf, uint16_t len, bool factory_reset_persistent);

/**
 * Get the value for a property.
 *
 * This function blocks until completion.
 *
 * @param key the key of the property
 * @param buf the output buffer where the value is read. It is the
 * responsibility of the caller to provide a buffer large enough to contain
 * the property value.
 * @param len length of output buffer in bytes
 * @param readlen the length of the value.
 * @return status code:
 *  PROPERTIES_STORAGE_IO_ERROR:  read failure, element is not read
 *  CBUFFER_STORAGE_KEY_NOT_FOUND_ERROR: unknown key
 *  CBUFFER_STORAGE_BOUNDS_ERROR: buf length is too small to contain the value
 *  CBUFFER_STORAGE_SUCCESS:      store succeed
 */
properties_storage_status_t properties_storage_get(uint32_t key, uint8_t *buf,
	uint16_t len, uint16_t* readlen);

/**
 * Get info about a property.
 *
 * This function blocks until completion.
 *
 * @param key the key of the property
 * @param len the size of the property value in bytes
 * @param factory_reset_persistent return true if the property will persist upon
 * a factory reset.
 * @return status code:
 *  CBUFFER_STORAGE_KEY_NOT_FOUND_ERROR: unknown key
 *  CBUFFER_STORAGE_SUCCESS:      the key exists
 */
properties_storage_status_t properties_storage_get_info(uint32_t key,
	uint16_t* len, bool* factory_reset_persistent);

/**
 * Delete a property.
 *
 * This function blocks until completion.
 *
 * @param key the key of the property
 * @return status code:
 *  PROPERTIES_STORAGE_IO_ERROR: read failure, element is not read
 *  CBUFFER_STORAGE_KEY_NOT_FOUND_ERROR: unknown key
 *  CBUFFER_STORAGE_SUCCESS:      delete succeed
 */
properties_storage_status_t properties_storage_delete(uint32_t key);

/** @} */

#endif /* __PROPERTIES_STORAGE_H */
