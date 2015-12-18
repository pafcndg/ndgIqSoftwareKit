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

#include <assert.h>
#include <limits.h>
#include <string.h>

#include "zephyr/bluetooth/gatt.h"
#include "gatt_int.h"

#include "conn_internal.h"

#include "services/ble_service/ble_service_gatt.h"

#include "ble_service_core_int.h"
#include "ble_service_utils.h"
#include "ble_service_int.h"

#include "infra/log.h"

//#define BT_GATT_DEBUG 1

struct ble_gatt_service {
	const struct bt_gatt_attr *attrs; /* Pointer to the array of attributes */
	uint16_t attr_count; /* Number of attributes in the array */
	uint16_t handle; /* Allocated handle of the service attribute */
};

static struct ble_gatt_service m_services_db[BLE_GATTS_MAX_SERVICES] = { { 0 }, };
static uint8_t m_services_count = 0;

struct attr_data_ctx {
	uint8_t *p_buffer; /* allocated buffer */
	uint8_t *p; /* data offset pointer into buffer */
	struct ble_gatt_attr *p_attrs; /* current attribute in buffer */
};

/**
 * Copy a UUID in a buffer using the smallest memory length
 * @param p_buf Pointer to the memory where the UUID shall be copied
 * @param p_uuid Pointer to the UUID to copy
 * @return The length required to store the UUID in the memory
 */
static uint8_t bt_gatt_uuid_memcpy(uint8_t *p_buf, const struct bt_uuid *p_uuid)
{
	uint8_t *p = p_buf;

	/* Store the type of the UUID */
	UINT8_TO_LESTREAM(p, p_uuid->type);

	/* Store the UUID data */
	if (BT_UUID_16 == p_uuid->type) {
		UINT16_TO_LESTREAM(p, p_uuid->u16);
	} else {
		memcpy(p, p_uuid->u128, sizeof(p_uuid->u128));
		p += sizeof(p_uuid->u128);
	}
	return p - p_buf;
}

/**
 * Compute the smallest memory length required to store a UUID
 *
 * @param p_uuid Pointer to the UUID to store
 * @return The minimum length required to store the UUID
 */
static uint8_t bt_gatt_uuid_len(const struct bt_uuid *p_uuid)
{
	if (BT_UUID_16 == p_uuid->type)
		return BT_UUID_16_SZ(*p_uuid);
	return BT_UUID_128_SZ(*p_uuid);
}

static void sum_descriptor_data_len(const struct bt_gatt_attr *attr,
				       void *user_data)
{
	size_t *p_len = user_data;

	/* Add UUID length of descriptor */
	*p_len += bt_gatt_uuid_len(attr->uuid);

	/* Add max length of descriptor user_data */
	*p_len += attr->read(NULL, attr, NULL, 0, 0);
}

static void copy_desc_data(const struct bt_gatt_attr *attr,
			      void *user_data)
{
	struct attr_data_ctx *ctx = user_data;
	int len;

	/* Update the Offset of the descriptor UUID */
	ctx->p_attrs->uuid_offset = ctx->p - ctx->p_buffer;

	/* Copy the descriptor UUID */
	ctx->p += bt_gatt_uuid_memcpy(ctx->p, attr->uuid);

	/* Init data, assumed to be == max length. */
	len = attr->read(NULL, attr, ctx->p, BT_GATT_PDU_MAX, 0);

	if (len) {
		ctx->p_attrs->user_data_offset = ctx->p - ctx->p_buffer;
		ctx->p += len;
	}
	ctx->p_attrs->max_len = len;

	/* Point to the next attribute in the output buffer */
	ctx->p_attrs++;
}

static size_t bt_gatt_foreach_attr_entry(const struct bt_gatt_attr *start_attr,
			       size_t end_idx,
			       void (*func)(const struct bt_gatt_attr *, void *),
			       void *user_data)
{
	size_t i;

	for (i = 0; i < end_idx; i++) {
		const struct bt_gatt_attr *attr = &start_attr[i];

		/* Stop when reached next characteristic descriptor */
		if ((attr->uuid->type == BT_UUID_16) &&
		    (attr->uuid->u16 == BT_UUID_GATT_CHRC)) {
			break;
		}

		func(attr, user_data);
	}
	return i;
}

static void ble_gatt_attr_tbl_cpy(struct ble_gatt_attr *attrs_out,
		const struct bt_gatt_attr *attrs_in, uint16_t count)
{
	int i;

	for (i = 0; i < count; i++) {
		attrs_out[i].flags = attrs_in[i].flags;
	}
}

#define UPDATE_ATTR_DATA_OFFSET(m) 				\
	do {							\
		m.p_attrs->user_data_offset = m.p - m.p_buffer; \
		m.p_attrs++; 					\
	} while (0)

int bt_gatt_register(struct bt_gatt_attr *attrs, size_t count)
{
	size_t idx;
	struct bt_gatt_attr *p_attr;
	struct attr_data_ctx ctx;
	size_t attr_tbl_len;
	struct ble_gatt_register param;

	assert(m_services_count < BLE_GATTS_MAX_SERVICES);

	m_services_db[m_services_count].attrs = attrs;
	m_services_db[m_services_count].attr_count = count;
	param.service_idx = m_services_count++;
	param.attr_count = count;

	/* TABLE SIZE CALCULATION */
	p_attr = &attrs[0];

	/* Primary service description */
	assert((p_attr->uuid->type == BT_UUID_16) &&
		(p_attr->uuid->u16 == BT_UUID_GATT_PRIMARY));

	/* service UUID size to copy, assume 128bit */
	attr_tbl_len = p_attr->read(NULL, p_attr, NULL, 0, 0);

	idx = 1;
	while (idx < count) {
		struct bt_gatt_chrc *chrc;

		p_attr = &attrs[idx];

		/* Characteristic attribute */
		assert((p_attr->uuid->type == BT_UUID_16) &&
			(p_attr->uuid->u16 == BT_UUID_GATT_CHRC));

		/* Retrieve characteristic structure */
		chrc = p_attr->user_data;

		/* Characteristic attribute identifier (UUID 16bits) + attribute data length */
		attr_tbl_len += BT_UUID_16_SZ(*p_attr->uuid) +
				p_attr->read(NULL, p_attr, NULL, 0, 0);

		idx++;
		p_attr = &attrs[idx];

		/* Characteristic value attribute, following characteristic attribute! */

		/* If readable, allocate the space required for the init value */
		if (chrc->properties & BT_GATT_CHRC_READ) {
			/* read with NULL buf -> return only the length */
			attr_tbl_len += p_attr->read(NULL, p_attr, NULL, 0, 0);
		}

		idx++;
		p_attr = &attrs[idx];

		/* Iterate over each additional the descriptors in this characteristic */
		idx += bt_gatt_foreach_attr_entry(p_attr, count - idx,
			sum_descriptor_data_len, &attr_tbl_len);
	}

	/* TABLE FILLING */

	/* Allocate buffer for table and copy base attribute table */
	attr_tbl_len += sizeof(struct ble_gatt_attr) * count;

	/* Allocate an aligned buffer on the stack */
	ctx.p_buffer = balloc(attr_tbl_len, NULL);
	ctx.p_attrs = (struct ble_gatt_attr *)ctx.p_buffer;
	memset(ctx.p_buffer, 0, attr_tbl_len);

	/* Copy the basic elements */
	ble_gatt_attr_tbl_cpy(ctx.p_attrs, attrs, count);

	/* Point the buffer right after the table of attributes descriptors */
	ctx.p = (uint8_t *)&ctx.p_attrs[count];

	p_attr = &attrs[0];

	/* Copy UUID of service */
	UPDATE_ATTR_DATA_OFFSET(ctx);
	ctx.p += p_attr->read(NULL, p_attr, ctx.p, BT_GATT_PDU_MAX, 0);

	idx = 1;
	while (idx < count) {
		struct bt_gatt_chrc *chrc;

		p_attr = &attrs[idx];

		/* Retrieve characteristic structure */
		chrc = p_attr->user_data;

		/* Characteristic descriptor UUID (16 bits) */
		ctx.p_attrs->uuid_offset = ctx.p - ctx.p_buffer;
		ctx.p += bt_gatt_uuid_memcpy(ctx.p, p_attr->uuid);

		/* Characteristic attribute data (UUID + property) */
		UPDATE_ATTR_DATA_OFFSET(ctx);
		ctx.p += p_attr->read(NULL, p_attr, ctx.p, BT_GATT_PDU_MAX, 0);

		idx++;
		p_attr = &attrs[idx];

		/* Characteristic value attribute, following characteristic attribute! */

		if (chrc->properties & BT_GATT_CHRC_READ) {
			/* If readable, fetch the init value */
			ctx.p_attrs->max_len = p_attr->read(NULL, p_attr, ctx.p, BT_GATT_PDU_MAX, 0);
			ctx.p_attrs->user_data_offset = ctx.p - ctx.p_buffer;
			ctx.p += ctx.p_attrs->max_len;
		}
		else {
			/* If not readable, get the maximum length */
			ctx.p_attrs->max_len = p_attr->read(NULL, p_attr, NULL, BT_GATT_PDU_MAX, 0);
		}

		ctx.p_attrs++;

		idx++;
		p_attr = &attrs[idx];

		/* iterate over each additional the descriptors in this characteristic */
		idx += bt_gatt_foreach_attr_entry(p_attr, count - idx,
				copy_desc_data, &ctx);
	}

	assert(attr_tbl_len == (ctx.p - ctx.p_buffer));
	param.req = _ble_cb.svc_init_msg;
	_ble_cb.svc_init_msg = NULL;
	ble_gatt_register_req(&param, ctx.p_buffer, attr_tbl_len);
	bfree(ctx.p_buffer);
	return 0;
}

void on_ble_gatt_register_rsp(const struct ble_gatt_register *p_param,
			  uint8_t *p_result, uint8_t len)
{
	const struct ble_gatt_attr_idx_entry * attrs_entry =
					(__typeof__(attrs_entry))p_result;
	const struct bt_gatt_attr * attrs;

	/* if attr_count is 0, error occurred */
	assert(p_param->attr_count);

	attrs = m_services_db[p_param->service_idx].attrs;

	/* Store service handle (mainly debugging purposes). */
	m_services_db[p_param->service_idx].handle = attrs_entry->handle;

#ifdef BT_GATT_DEBUG
	{
		int idx;
		int attr_count = p_param->attr_count;
		pr_info(LOG_MODULE_BLE, "on_ble_gatt_register: svc_idx %d, attr_count: %d, service_handle: %d",
				p_param->service_idx, attr_count, m_services_db[p_param->service_idx].handle);

		for (idx = 1; idx < attr_count; idx++) {
			const struct ble_gatt_attr_idx_entry *att_entry =
					&attrs_entry[idx];

			/* The following order of declaration is assumed for this to
			   work (otherwise idx-2 will fail!):
			   BT_GATT_CHARACTERISTIC -> ble core returns invalid handle
			   BT_GATT_DESCRIPTOR -> value handle of characteristic
			   BT_GATT_CCC -> cccd handle is ignored as no storage but
			   reference value is updated in CCC with value handle from
			   descriptor */
			if (att_entry->handle != BT_GATT_INV_HANDLE) {
				pr_info(LOG_MODULE_BLE, "on_ble_gatt_register(): idx %d, h %d, type %d, u16 0x%x",
					idx, att_entry->handle, attrs[idx].uuid->type,
					attrs[idx].uuid->u16);
			}
		}
	}
#endif

	/* return response message to callee, init_svc_complete() free req buffer */
	assert(p_param->req);
	assert(p_param->req->init_svc_complete);
	p_param->req->init_svc_complete(p_param->req);
}

uint16_t ble_attr_idx_to_handle(const struct bt_gatt_attr *attrs, uint8_t index)
{
	size_t i;
	uint16_t last_h = 0;

	/* Loop on all registered services */
	for (i = 0; i < m_services_count; i++) {
		/* Check if this is the service looked for */
		if (m_services_db[i].attrs == attrs) {
			assert(index < m_services_db[i].attr_count);
			/* Return the index of the attribute in concatenated table */
			return last_h + index;
		}
		last_h += m_services_db[i].attr_count;
	}

	return BT_GATT_INV_HANDLE;
}

int bt_gatt_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		      void *buf, uint16_t buf_len, uint16_t offset,
		      const void *value, uint16_t value_len)
{
	uint16_t len;

	/* simply return the value length. used as max_value. */
	if (buf == NULL)
		return value_len;

	if (offset > value_len) {
		return E_OS_ERR;
	}

	len = min(buf_len, value_len - offset);

#ifdef BT_GATT_DEBUG
	pr_info(LOG_MODULE_BLE, "handle 0x%04x offset %u length %u",
		attr->handle, offset, len);
#endif

	memcpy(buf, value + offset, len);

	return len;
}

int bt_gatt_attr_read_service(struct bt_conn *conn,
			      const struct bt_gatt_attr *attr,
			      void *buf, uint16_t len, uint16_t offset)
{
	const struct bt_uuid *uuid = attr->user_data;

	if (buf == NULL)
		return bt_gatt_uuid_len(uuid);

	return bt_gatt_uuid_memcpy(buf, uuid);
}

int bt_gatt_attr_read_chrc(struct bt_conn *conn,
			   const struct bt_gatt_attr *attr, void *buf,
			   uint16_t len, uint16_t offset)
{
	struct bt_gatt_chrc *chrc = attr->user_data;
	uint8_t *p = buf;

	/* return length required, return here to avoid copy */
	if (buf == NULL)
		return sizeof(chrc->properties) + bt_gatt_uuid_len(chrc->uuid);

	/* copy property byte and characteristic attribute UUID */
	*p++ = chrc->properties;

	return (sizeof(chrc->properties) + bt_gatt_uuid_memcpy(p, chrc->uuid));
}

__attribute__((weak)) int bt_gatt_attr_read_ccc(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	return 0;
}

static inline void gatt_ccc_changed(struct _bt_gatt_ccc *ccc)
{
	if (ccc->cfg_changed) {
		ccc->cfg_changed(ccc->value);
	}
}

int bt_gatt_attr_write_ccc(struct bt_conn *conn,
			   const struct bt_gatt_attr *attr, const void *buf,
			   uint16_t len, uint16_t offset)
{
	struct _bt_gatt_ccc *ccc = attr->user_data;
	const uint16_t *data = buf;

	if (len != sizeof(*data) || offset) {
		return E_OS_ERR_NOT_ALLOWED;
	}

#ifdef BT_GATT_DEBUG
	pr_info(LOG_MODULE_BLE,"char CCC value 0x%x, new: 0x%x",
			ccc->value, *data);
#endif
	/* normally a check on per connection should be used.
	 * also look only at the 2 relevant bits to allow upper layer to use
	 * it for different purposes. */
	if ((ccc->value & (BT_GATT_CCC_NOTIFY | BT_GATT_CCC_INDICATE)) != *data) {
		ccc->value &= ~(BT_GATT_CCC_NOTIFY | BT_GATT_CCC_INDICATE);
		ccc->value |= *data;
		gatt_ccc_changed(ccc);
	}

	return len;
}

int bt_gatt_attr_read_cep(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	struct bt_gatt_cep *value = attr->user_data;
	uint16_t props = value->properties;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &props,
				 sizeof(props));
}

/* makes the assumption that the string is none-modifiable and the max len */
int bt_gatt_attr_read_cud(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	const struct bt_gatt_cud *value = attr->user_data;
	uint16_t value_len = strlen(value->string);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value->string,
				 value_len);
}

int bt_gatt_attr_read_pf(struct bt_conn *conn,
			 const struct bt_gatt_attr *attr, void *buf,
			 uint16_t len, uint16_t offset)
{
	const struct bt_gatt_pf *value = attr->user_data;
	/* TODO: fix length, either use packed or serialise into a buffer */

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}

void bt_gatt_foreach_attr(uint16_t start_handle, uint16_t end_handle,
			  bt_gatt_attr_func_t func, void *user_data)
{
	size_t i;
	uint8_t svc_idx;

	for (svc_idx = 0; svc_idx < m_services_count; svc_idx++) {
		size_t attr_count = m_services_db[svc_idx].attr_count;
		for (i = 0; i < attr_count; i++) {
			const struct bt_gatt_attr *attr = &m_services_db[m_services_count].attrs[i];

			/* Check if attribute handle is within range */
			if (attr->handle < start_handle || attr->handle > end_handle) {
				continue;
			}

			if (func(attr, user_data) == BT_GATT_ITER_STOP) {
				break;
			}
		}
	}
}

void on_ble_gatts_write_evt(const struct ble_gatt_wr_evt *p_evt,
			    const uint8_t *p_buf, uint8_t buflen)
{
	uint8_t svc_idx = p_evt->attr.svc_idx;
	uint8_t attr_idx = p_evt->attr.attr_idx;
	const struct bt_gatt_attr *attr;

	assert(svc_idx < m_services_count);
	assert(attr_idx < m_services_db[svc_idx].attr_count);

#ifdef BT_GATT_DEBUG
	pr_info(LOG_MODULE_BLE,"write %u %u", svc_idx, attr_idx);
#endif

	attr = &m_services_db[svc_idx].attrs[attr_idx];
	if (attr->write) {
		struct bt_conn conn;

		/* TODO: better mapping ble core handle to zephyr conn object */
		conn.handle = p_evt->conn_handle;
		attr->write(&conn, attr, p_buf, buflen, p_evt->offset);
	}
}

int bt_gatt_notify(struct bt_conn *conn, uint16_t handle, const void *data,
		   uint16_t len, bt_gatt_notify_func_t cb)
{
	struct ble_gatt_send_notif_ind_params notif;

	notif.conn_handle = conn->handle;
	notif.params.val_handle = handle;
	notif.params.offset = 0;
	notif.cback = cb;

	ble_gatt_send_notif_req(&notif, (uint8_t *)data, len);

	return 0;
}

void on_ble_gatts_send_notif_ind_rsp(const struct ble_gatt_notif_ind_rsp_msg *p_params)
{
	struct bt_conn conn;

	conn.handle = p_params->conn_handle;

	if (p_params->cback)
		p_params->cback(&conn, p_params->handle, p_params->status);
}

