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

#include "drivers/serial_bus_access.h"

/*
 * Allocate a sba_request_t, fill it and add it to serial bus access queue
 * data_write is copied to a new allocated buffer
 * When request has been executed, allocated buffer and sba_request_t are freed and callback is called
 */
DRIVER_API_RC sba_i2c_write(SBA_BUSID bus_id, uint8_t *data_write, uint32_t data_write_len, uint32_t slave_addr, void (*callback)(struct sba_request *));

/*
 * Allocate a sba_request_t, fill it and add it to serial bus access queue
 * When request has been executed, sba_request_t is freed and callback is called
 */
DRIVER_API_RC sba_i2c_read(SBA_BUSID bus_id, uint8_t *data_read, uint32_t data_read_len, uint32_t slave_addr, void (*callback)(struct sba_request *));

/*
 * Allocate a sba_request_t, fill it and add it to serial bus access queue
 * When request has been executed, sba_request_t is freed and callback is called
 */
DRIVER_API_RC sba_i2c_transfer(SBA_BUSID bus_id, uint8_t *data_write, uint32_t data_write_len, uint8_t *data_read, uint32_t data_read_len, uint32_t slave_addr, void (*callback)(struct sba_request *));

