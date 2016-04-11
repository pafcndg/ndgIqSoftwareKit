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
#include <errno.h>

#include <misc/byteorder.h>

#include "zephyr/bluetooth/gatt.h"
#include "gatt_internal.h"
#include "hci_core.h"
#include "conn_internal.h"

#include "ble_service_utils.h"
#include "ble_service_int.h"

#include "infra/log.h"

#include "util/misc.h"

//#define BT_GATT_DEBUG 1

/* Packed UUID 16 bit size. */
#define BT_UUID_16_SZ(u) (sizeof((u).type) + sizeof((u).u16))
/* Packed UUID 128 bit size. */
#define BT_UUID_128_SZ(u) (sizeof((u).type) + sizeof((u).u128))


/* TODO imported from zephyr/net/bluetooth/att.h : this is an error from Zephyr
 * which should have exposed this value
 */
#define BT_ATT_ERR_UNLIKELY	0x0e

struct ble_gatt_service {
	const struct bt_gatt_attr *attrs; /* Pointer to the array of attributes */
	uint16_t attr_count; /* Number of attributes in the array */
	uint16_t handle; /* Allocated handle of the service attribute */
};

/* Max number of GATTS services supported in thunderdome.
 *
 * BLE core has its own limits depending on its stack configuration. In this
 * case service registering will fail despite CONFIG_BT_GATT_BLE_MAX_SERVICES
 * being big enough. */
#define BLE_GATTS_MAX_SERVICES (CONFIG_BT_GATT_BLE_MAX_SERVICES)

static struct ble_gatt_service m_services_db[BLE_GATTS_MAX_SERVICES] = { { 0 }, };

static uint8_t m_services_count = 0;

struct attr_data_ctx {
	uint8_t *p_buffer; /* allocated buffer */
	uint8_t *p; /* data offset pointer into buffer */
	struct ble_gatt_attr *p_attrs; /* current attribute in buffer */
};

#if defined(CONFIG_BLUETOOTH_GATT_CLIENT)
struct bt_gatt_write {
	void (*func)(struct bt_conn *conn, uint8_t err, struct bt_gatt_write *user_data);
};

struct bt_gatt_ccc_write_params {
	struct bt_gatt_write params;
	struct bt_gatt_subscribe_params *subscribe_params;
};

struct bt_gatt_write_params {
	struct bt_gatt_write params;
	bt_gatt_write_rsp_func_t func; /* Callback function */
	const void *data;
};

static struct bt_gatt_subscribe_params *subscriptions = NULL;
#endif

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
	len = attr->read(NULL, attr, ctx->p, 0xFFFF, 0);

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
		    (attr->uuid->u16 == 0x2803)) {
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
		attrs_out[i].perm = attrs_in[i].perm;
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

	if (!attrs || !count) {
		return -EINVAL;
	}
	assert(m_services_count < BLE_GATTS_MAX_SERVICES);

	m_services_db[m_services_count].attrs = attrs;
	m_services_db[m_services_count].attr_count = count;
	param.service_idx = m_services_count++;
	param.attr_count = count;

	/* TABLE SIZE CALCULATION */
	p_attr = &attrs[0];

	/* Primary service description */
	assert((p_attr->uuid->type == BT_UUID_16) &&
		(p_attr->uuid->u16 == 0x2800));

	/* service UUID size to copy, assume 128bit */
	attr_tbl_len = p_attr->read(NULL, p_attr, NULL, 0, 0);

	idx = 1;
	while (idx < count) {
		struct bt_gatt_chrc *chrc;

		p_attr = &attrs[idx];

		/* Characteristic attribute */
		assert((p_attr->uuid->type == BT_UUID_16) &&
			(p_attr->uuid->u16 == 0x2803));

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
	ctx.p += p_attr->read(NULL, p_attr, ctx.p, 0xFFFF, 0);

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
		ctx.p += p_attr->read(NULL, p_attr, ctx.p, 0xFFFF, 0);

		idx++;
		p_attr = &attrs[idx];

		/* Characteristic value attribute, following characteristic attribute! */

		if (chrc->properties & BT_GATT_CHRC_READ) {
			/* If readable, fetch the init value */
			ctx.p_attrs->max_len = p_attr->read(NULL, p_attr, ctx.p, 0xFFFF, 0);
			ctx.p_attrs->user_data_offset = ctx.p - ctx.p_buffer;
			ctx.p += ctx.p_attrs->max_len;
		}
		else {
			/* If not readable, get the maximum length */
			ctx.p_attrs->max_len = p_attr->read(NULL, p_attr, NULL, 0xFFFF, 0);
		}

		ctx.p_attrs++;

		idx++;
		p_attr = &attrs[idx];

		/* iterate over each additional the descriptors in this characteristic */
		idx += bt_gatt_foreach_attr_entry(p_attr, count - idx,
				copy_desc_data, &ctx);
	}

	assert(attr_tbl_len == (ctx.p - ctx.p_buffer));

	/* This is a dirty hack to retrieve the message to send back */
	ble_gatt_register_req(&param, ctx.p_buffer, attr_tbl_len);
	bfree(ctx.p_buffer);
	return 0;
}

void on_ble_gatt_register_rsp(const struct ble_gatt_register *p_param,
		const struct ble_gatt_attr_idx_entry *p_attrs, uint8_t len)
{
	/* if attr_count is 0, error occurred */
	assert(p_param->attr_count != 0);

	/* Store service handle (mainly debugging purposes). */
	m_services_db[p_param->service_idx].handle = p_attrs->handle;
#ifdef BT_GATT_DEBUG
	{
		const struct bt_gatt_attr * attrs;
		int idx;
		int attr_count = p_param->attr_count;

		attrs = m_services_db[p_param->service_idx].attrs;

		pr_info(LOG_MODULE_BLE, "on_ble_gatt_register: svc_idx %d, attr_count: %d, service_handle: %d",
				p_param->service_idx, attr_count, m_services_db[p_param->service_idx].handle);

		for (idx = 1; idx < attr_count; idx++) {
			const struct ble_gatt_attr_idx_entry *att_entry =
					&p_attrs[idx];

			/* The following order of declaration is assumed for this to
			   work (otherwise idx-2 will fail!):
			   BT_GATT_CHARACTERISTIC -> ble core returns invalid handle
			   BT_GATT_DESCRIPTOR -> value handle of characteristic
			   BT_GATT_CCC -> cccd handle is ignored as no storage but
			   reference value is updated in CCC with value handle from
			   descriptor */
			if (att_entry->handle != 0) {
				pr_info(LOG_MODULE_BLE, "on_ble_gatt_register(): idx %d, h %d, type %d, u16 0x%x",
					idx, att_entry->handle, attrs[idx].uuid->type,
					attrs[idx].uuid->u16);
			}
		}
	}
#endif
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

	return 0;
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
		return -EINVAL;
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
	struct bt_uuid *uuid = attr->user_data;

	if (buf == NULL)
		return bt_gatt_uuid_len(uuid);

	return bt_gatt_uuid_memcpy(buf, uuid);
}


int bt_gatt_attr_read_included(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr,
			       void *buf, uint16_t len, uint16_t offset)
{
	return -EINVAL;
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

int bt_gatt_attr_read_ccc(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	return 0;
}

static void gatt_ccc_changed(struct _bt_gatt_ccc *ccc)
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

	if (offset > sizeof(*data)) {
		return -EINVAL;
	}

	if (offset + len > sizeof(*data)) {
		return -EFBIG;
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
	uint16_t props = sys_cpu_to_le16(value->properties);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &props,
				 sizeof(props));
}

int bt_gatt_attr_read_cud(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}

int bt_gatt_attr_read_cpf(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	struct bt_gatt_cpf *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}

void on_ble_gatts_send_notif_ind_rsp(const struct ble_gatt_notif_ind_rsp *p_params)
{
	struct bt_conn *conn;

	conn = bt_conn_lookup_handle(p_params->conn_handle);

	if (conn) {
		if (p_params->cback)
			p_params->cback(conn, p_params->handle, p_params->status);

		bt_conn_unref(conn);
	}
}

int bt_gatt_notify(struct bt_conn *conn, uint16_t handle, const void *data,
		   uint16_t len, bt_gatt_notify_func_t cb)
{
	struct ble_gatt_send_notif_ind_params notif;

	if (conn)
		notif.conn_handle = conn->handle;
	else
		notif.conn_handle = 0xFFFF;
	notif.params.val_handle = handle;
	notif.params.offset = 0;
	notif.cback = cb;

	ble_gatt_send_notif_req(&notif, (uint8_t *)data, len);

	return 0;
}

#if defined(CONFIG_BLUETOOTH_GATT_CLIENT)
void on_ble_gattc_value_evt(const struct ble_gattc_value_evt *p_evt,
		uint8_t *p_buf, uint8_t buflen)
{
	struct bt_gatt_subscribe_params *params;
	struct bt_conn *conn;

	conn = bt_conn_lookup_handle(p_evt->conn_handle);

	if (conn) {
		for (params = subscriptions; params; params = params->_next) {
			if (p_evt->handle != params->value_handle) {
				continue;
			}

			if (params->func(conn, 0, params, p_buf, buflen) == BT_GATT_ITER_STOP) {
				bt_gatt_unsubscribe(conn, params);
			}
		}
		bt_conn_unref(conn);
	}
}

#if NOT_USED_FOR_THE_TIME_BEING
static void gatt_subscription_remove(struct bt_gatt_subscribe_params *prev,
				     struct bt_gatt_subscribe_params *params)
{
	/* Remove subscription from the list*/
	if (!prev) {
		subscriptions = params->_next;
	} else {
		prev->_next = params->_next;
	}

	if (params->destroy)
		params->destroy(params);
}

static void remove_subscribtions(struct bt_conn *conn)
{
	struct bt_gatt_subscribe_params *params, *prev;

	/* Lookup existing subscriptions */
	for (params = subscriptions, prev = NULL; params;
	     prev = params, params = params->_next) {
		if (bt_addr_le_cmp(&params->_peer, &conn->le.dst)) {
			continue;
		}

		/* Remove subscription */
		gatt_subscription_remove(prev, params);
	}
}
#endif

int bt_gatt_exchange_mtu(struct bt_conn *conn, bt_gatt_rsp_func_t func)
{
	return -EINVAL;
}

// RPC function function call from NORDIC
void on_ble_gattc_discover_rsp(const struct ble_gattc_disc_rsp *rsp,
		const uint8_t *data, uint8_t data_len)
{

	uint16_t last_handle;
	int status = BT_GATT_ITER_STOP;
	struct bt_gatt_discover_params *params;
	struct bt_gatt_service svc_value;
	struct bt_gatt_include inc_value;
	struct bt_conn *conn = bt_conn_lookup_handle(rsp->conn_handle);


	assert(conn);

	params = conn->gattc.disc_param;

#ifdef BT_GATT_DEBUG
	pr_info(LOG_MODULE_BLE, "disc_rsp: s=%d", rsp->status);
#endif

	if (!rsp->status) {
		size_t i;
		uint8_t attr_count;

		if (rsp->type == BT_GATT_DISCOVER_PRIMARY) {
			attr_count = (data_len / sizeof(struct ble_gattc_prim_svc));
		} else if (rsp->type == BT_GATT_DISCOVER_INCLUDE) {
			attr_count = (data_len / sizeof(struct ble_gattc_incl_svc));
		} else if (rsp->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
			attr_count = (data_len / sizeof(struct ble_gattc_characteristic));
		} else if (rsp->type == BT_GATT_DISCOVER_DESCRIPTOR) {
			attr_count = (data_len / sizeof(struct ble_gattc_descriptor));
		}
		else
			goto done;

#ifdef BT_GATT_DEBUG
		pr_info(LOG_MODULE_BLE, "disc_rsp: c=%d", attr_count);
#endif

		last_handle = params->end_handle;

		for (i = 0; i < attr_count; i++) {
			struct bt_gatt_attr *attr = NULL;

			if (BT_GATT_DISCOVER_PRIMARY == rsp->type) {
				const struct ble_gattc_prim_svc *gattr = (void *)&data[i * sizeof(*gattr)];

				if ((gattr->handle_range.start_handle < params->start_handle) &&
				    (gattr->handle_range.end_handle > params->end_handle)) {
					/* Only the attributes with attribute handles between and including the Starting
					 Handle and the Ending Handle is returned*/
					goto done;
				}

				svc_value.end_handle = gattr->handle_range.end_handle;
				svc_value.uuid = params->uuid;

				attr = (&(struct bt_gatt_attr)BT_GATT_PRIMARY_SERVICE(&svc_value));
				attr->handle = gattr->handle;
				last_handle = svc_value.end_handle;


			} else if (BT_GATT_DISCOVER_INCLUDE == rsp->type) {
				const struct ble_gattc_incl_svc *gattr = (void *)&data[i * sizeof(*gattr)];

				inc_value.start_handle = gattr->svc.handle_range.start_handle;
				inc_value.end_handle = gattr->svc.handle_range.end_handle;

				/*
				 * 4.5.1 If the service UUID is a 16-bit Bluetooth UUID
				 *  it is also returned in the response.
				 */
				switch(gattr->svc.uuid.type) {
				case BT_UUID_16:
					inc_value.uuid = &gattr->svc.uuid;
					break;
				case BT_UUID_128:
					/* Data is not available at this point */
					break;
				}

				attr = (&(struct bt_gatt_attr)
						BT_GATT_INCLUDE_SERVICE(&inc_value));
				attr->handle = gattr->incl_handle;
				last_handle = gattr->incl_handle;


			} else if (BT_GATT_DISCOVER_CHARACTERISTIC == rsp->type) {
				const struct ble_gattc_characteristic *gattr = (void*)&data[i * sizeof(*gattr)];

				attr = (&(struct bt_gatt_attr)BT_GATT_CHARACTERISTIC(&gattr->uuid,
						gattr->prop));
				attr->handle = gattr->decl_handle;
				last_handle = gattr->decl_handle;

				/* Skip if UUID is set but doesn't match */
				if (params->uuid && bt_uuid_cmp(&gattr->uuid, params->uuid)) {
					continue;
				}

			} else if (BT_GATT_DISCOVER_DESCRIPTOR == rsp->type) {
				const struct ble_gattc_descriptor *gattr = (void *)&data[i * sizeof(*gattr)];

				attr = (&(struct bt_gatt_attr)
					BT_GATT_DESCRIPTOR(&gattr->uuid, 0, NULL, NULL, NULL));
				attr->handle = gattr->handle;
				last_handle = gattr->handle;

			}
			else
				goto done;
			status = params->func(conn, attr, params);
			if (BT_GATT_ITER_STOP == status)
				goto done;
		}
		if (last_handle < UINT16_MAX) {
			last_handle++;
		}

#ifdef BT_GATT_DEBUG
		pr_info(LOG_MODULE_BLE, "disc_rsp: l=%d", last_handle);
#endif
		params->start_handle = last_handle;

		if (params->start_handle < params->end_handle) {
			conn->gattc.disc_param = NULL;
			if (!bt_gatt_discover(conn, params))
				goto not_done;

		}
	}
done:
#ifdef BT_GATT_DEBUG
	pr_info(LOG_MODULE_BLE, "disc_rsp: done");
#endif
	/* discover process done */
	if (params->destroy)
		params->destroy(params);

	conn->gattc.disc_param = NULL;

not_done:
	bt_conn_unref(conn);
}

int bt_gatt_discover(struct bt_conn *conn,
		     struct bt_gatt_discover_params *params)
{
	struct ble_core_discover_params discover_params;

	if (!conn || conn->state != BT_CONN_CONNECTED || !params || !params->func ||
	    !params->start_handle || !params->end_handle || params->start_handle > params->end_handle) {
		return -EINVAL;
	}
	/* Sanity check */
	BUILD_BUG_ON(sizeof(conn->gattc) != sizeof(conn->gattc.opaque));
	if (conn->gattc.opaque)
		return -EBUSY;

#ifdef BT_GATT_DEBUG
	pr_info(LOG_MODULE_BLE, "disc: %d", params->start_handle);
#endif

	memset(&discover_params, 0, sizeof(discover_params));

	switch (params->type) {
	case BT_GATT_DISCOVER_PRIMARY:
	case BT_GATT_DISCOVER_CHARACTERISTIC:
		if (params->uuid) {
			discover_params.uuid = *params->uuid;
			discover_params.flags = DISCOVER_FLAGS_UUID_PRESENT;
		}
		break;

	case BT_GATT_DISCOVER_INCLUDE:
	case BT_GATT_DISCOVER_DESCRIPTOR:
		break;
	default:
		conn->gattc.disc_param = NULL;
		return -EINVAL;
		break;
	}

	discover_params.conn_handle = conn->handle;
	discover_params.type = params->type;
	discover_params.handle_range.start_handle = params->start_handle;
	discover_params.handle_range.end_handle = params->end_handle;

	conn->gattc.disc_param = params;

	ble_gattc_discover_req(&discover_params, NULL);

	return 0;
}

void on_ble_gattc_read_rsp(const struct ble_gattc_read_rsp *p_evt,
		uint8_t *data, uint8_t data_len, void *p_priv)
{
	struct bt_gatt_read_params *params;
	struct bt_conn *conn = bt_conn_lookup_handle(p_evt->conn_handle);

	assert(conn);

	params = conn->gattc.rd_params;

	if (BLE_STATUS_SUCCESS == p_evt->status) {
		if (params->func(conn, p_evt->status,
				params,
				data, data_len) == BT_GATT_ITER_STOP) {
			goto done;
		}

		/*
		 * Core Spec 4.2, Vol. 3, Part G, 4.8.1
		 * If the Characteristic Value is greater than (ATT_MTU – 1) octets
		 * in length, the Read Long Characteristic Value procedure may be used
		 * if the rest of the Characteristic Value is required.
		 */
		if (data_len < BLE_GATT_MTU_SIZE - 1) {
			goto done;
		}

		conn->gattc.rd_params = NULL;

		params->offset += data_len;
		if (bt_gatt_read(conn, params)) {
			params->func(conn, BT_ATT_ERR_UNLIKELY, params, NULL, 0);
			goto done;
		}
		goto not_done;
	}

done:
	if (params->destroy) {
		params->destroy(params);
	}
	conn->gattc.rd_params = NULL;

not_done:
	bt_conn_unref(conn);
}

int bt_gatt_read(struct bt_conn *conn, struct bt_gatt_read_params *params)
{

	struct ble_gattc_read_params req;

	if (!conn || conn->state != BT_CONN_CONNECTED || !params ||
	    !params->handle || !params->func || !params->destroy) {
		return -EINVAL;
	}
	/* Sanity check */
	BUILD_BUG_ON(sizeof(conn->gattc) != sizeof(conn->gattc.opaque));
	if (conn->gattc.opaque)
		return -EBUSY;

	conn->gattc.rd_params = params;

	req.conn_handle = conn->handle;
	req.char_handle = params->handle;
	req.offset = params->offset;

	ble_gattc_read_req(&req, NULL);

	return 0;
}

void on_ble_gattc_write_rsp(const struct ble_gattc_write_rsp *p_evt, void *p_priv)
{

	struct bt_conn *conn = bt_conn_lookup_handle(p_evt->conn_handle);

	assert(conn);

	if (conn->gattc.wr_params && conn->gattc.wr_params->func) {
		conn->gattc.wr_params->func(conn, p_evt->status, conn->gattc.wr_params);
		bfree(conn->gattc.wr_params);
	}

	conn->gattc.wr_params = NULL;

	bt_conn_unref(conn);
}

static void bt_gatt_write_without_response_complete(struct bt_conn *conn, uint8_t err, const void *data)
{
}

int bt_gatt_write_without_response(struct bt_conn *conn, uint16_t handle,
				   const void *data, uint16_t length, bool sign)
{
	return bt_gatt_write(conn, handle, 0, data, length, bt_gatt_write_without_response_complete);
}


static void on_bt_gatt_write_complete(struct bt_conn *conn, uint8_t err, struct bt_gatt_write *user_data)
{
	struct bt_gatt_write_params *wr_params = container_of(user_data, struct bt_gatt_write_params, params);

	assert(wr_params->func);
	wr_params->func(conn, err, wr_params->data);
}


static int _bt_gatt_write(struct bt_conn *conn, uint16_t char_handle, bool with_resp, uint16_t offset,
		  const void *data, uint16_t length, struct bt_gatt_write *wr_params)
{
	struct ble_gattc_write_params req;

	/* Sanity check */
	BUILD_BUG_ON(sizeof(conn->gattc) != sizeof(conn->gattc.opaque));
	if (conn->gattc.opaque) {
		bfree(wr_params);
		return -EBUSY;
	}

	conn->gattc.wr_params = wr_params;

	req.conn_handle = conn->handle;
	req.char_handle = char_handle;
	req.offset = offset;
	req.with_resp = with_resp;

	ble_gattc_write_req(&req, data, length, NULL);

	return 0;
}

int bt_gatt_write(struct bt_conn *conn, uint16_t handle, uint16_t offset,
		  const void *data, uint16_t length, bt_gatt_write_rsp_func_t func)
{
	struct bt_gatt_write_params *wr_params;

	if (!conn || conn->state != BT_CONN_CONNECTED  || !handle || !func) {
		return -EINVAL;
	}

	wr_params = balloc(sizeof(*wr_params), NULL);
	wr_params->params.func = on_bt_gatt_write_complete;
	wr_params->func = func;
	wr_params->data = data;

	/* Sanity check */
	BUILD_BUG_ON(offsetof(struct bt_gatt_write_params, params) != 0);

	return _bt_gatt_write(conn, handle,
			(func == bt_gatt_write_without_response_complete) ? false : true,
			offset, data, length, &wr_params->params);
}

static void gatt_subscription_add(struct bt_conn *conn,
				  struct bt_gatt_subscribe_params *params)
{
	bt_addr_le_copy(&params->_peer, &conn->le.dst);

	/* Prepend subscription */
	params->_next = subscriptions;
	subscriptions = params;
}

static void gatt_subscribe_rsp(struct bt_conn *conn, uint8_t err,
		struct bt_gatt_write *user_data)
{
	struct bt_gatt_ccc_write_params *wr_params = container_of(user_data,
			struct bt_gatt_ccc_write_params, params);
	struct bt_gatt_subscribe_params *params = wr_params->subscribe_params;

	params->func(conn, err, params, NULL, 0);

	if (err) {
		if (params->destroy)
			params->destroy(params);
	}
	else {
		gatt_subscription_add(conn, params);
	}
}

static void gatt_unsubscribe_rsp(struct bt_conn *conn, uint8_t err,
		struct bt_gatt_write *user_data)
{
	struct bt_gatt_ccc_write_params *wr_params = container_of(user_data,
			struct bt_gatt_ccc_write_params, params);
	struct bt_gatt_subscribe_params *params = wr_params->subscribe_params;

	if (params->destroy)
		params->destroy(params);
}

int bt_gatt_subscribe(struct bt_conn *conn,
		      struct bt_gatt_subscribe_params *params)
{
	struct bt_gatt_subscribe_params *tmp;
	bool has_subscription = false;
	struct bt_gatt_ccc_write_params *wr_params;

	if (!conn || conn->state != BT_CONN_CONNECTED) {
		return -ENOTCONN;
	}

	if (!params || !params->func || !params->value || !params->ccc_handle) {
		return -EINVAL;
	}

	/* Lookup existing subscriptions */
	for (tmp = subscriptions; tmp; tmp = tmp->_next) {
		/* Fail if entry already exists */
		if (tmp == params) {
			return -EALREADY;
		}

		/* Check if another subscription exists */
		if (!bt_addr_le_cmp(&tmp->_peer, &conn->le.dst) &&
		    tmp->value_handle == params->value_handle &&
		    tmp->value >= params->value) {
			has_subscription = true;
		}
	}

	/* Skip write if already subcribed */
	if (has_subscription) {
		gatt_subscription_add(conn, params);
		/* LCARON: Call function to indicate that subscription completed since we are returning 0 */
		params->func(conn, 0, params, NULL, 0);
		return 0;
	}

	wr_params = balloc(sizeof(*wr_params), NULL);
	wr_params->params.func = gatt_subscribe_rsp;
	wr_params->subscribe_params = params;

	/* Sanity check */
	BUILD_BUG_ON(offsetof(struct bt_gatt_write_params, params) != 0);

	return _bt_gatt_write(conn, params->ccc_handle, true, 0, &params->value,
			sizeof(params->value), &wr_params->params);
}

int bt_gatt_unsubscribe(struct bt_conn *conn,
			struct bt_gatt_subscribe_params *params)
{
	struct bt_gatt_subscribe_params *tmp;
	bool has_subscription = false, found = false;
	struct bt_gatt_ccc_write_params *wr_params;

	if (!conn || conn->state != BT_CONN_CONNECTED) {
		return -ENOTCONN;
	}

	if (!params) {
		return -EINVAL;
	}

	/* Check head */
	if (subscriptions == params) {
		subscriptions = params->_next;
		found = true;
	}

	/* Lookup existing subscriptions */
	for (tmp = subscriptions; tmp; tmp = tmp->_next) {
		/* Remove subscription */
		if (tmp->_next == params) {
			tmp->_next = params->_next;
			found = true;
		}

		/* Check if there still remains any other subscription */
		if (!bt_addr_le_cmp(&tmp->_peer, &conn->le.dst) &&
		    tmp->value_handle == params->value_handle) {
			has_subscription = true;
		}
	}

	if (!found) {
		return -EINVAL;
	}

	if (has_subscription) {
		if (params->destroy)
			params->destroy(params);
		return 0;
	}

	wr_params = balloc(sizeof(*wr_params), NULL);
	wr_params->params.func = gatt_unsubscribe_rsp;
	wr_params->subscribe_params = params;

	params->value = BLE_UNSUBSCRIBE;

	/* Sanity check */
	BUILD_BUG_ON(offsetof(struct bt_gatt_ccc_write_params, params) != 0);

	return _bt_gatt_write(conn, params->ccc_handle, true, 0, &params->value,
			sizeof(params->value), &wr_params->params);
}

int bt_gatt_read_multiple(struct bt_conn *conn, const uint16_t *handles,
			  size_t count, bt_gatt_read_func_t func) {
	/* TODO */
	return -EINVAL;
}

static uint8_t connected_cb(const struct bt_gatt_attr *attr, void *user_data)
{
	struct bt_conn *conn = user_data;
	struct _bt_gatt_ccc *ccc;
	size_t i;

	/* Check attribute user_data must be of type struct _bt_gatt_ccc */
	if (attr->write != bt_gatt_attr_write_ccc) {
		return BT_GATT_ITER_CONTINUE;
	}

	ccc = attr->user_data;

	/* If already enabled skip */
	if (ccc->value) {
		return BT_GATT_ITER_CONTINUE;
	}

	for (i = 0; i < ccc->cfg_len; i++) {
		/* Ignore configuration for different peer */
		if (bt_addr_le_cmp(&conn->le.dst, &ccc->cfg[i].peer)) {
			continue;
		}

		if (ccc->cfg[i].value) {
			gatt_ccc_changed(ccc);
			return BT_GATT_ITER_CONTINUE;
		}
	}

	return BT_GATT_ITER_CONTINUE;
}

void bt_gatt_connected(struct bt_conn *conn)
{
	bt_gatt_foreach_attr(0x0001, 0xffff, connected_cb, conn);
}

void bt_conn_put(struct bt_conn *conn)
{
	int old_ref;
	old_ref = conn->ref--;

	if (old_ref > 1) {
		return;
	}

	bt_addr_le_copy(&conn->le.dst, BT_ADDR_LE_ANY);
}

static uint8_t disconnected_cb(const struct bt_gatt_attr *attr, void *user_data)
{
	struct bt_conn *conn = user_data;
	struct _bt_gatt_ccc *ccc;
	size_t i;

	/* Check attribute user_data must be of type struct _bt_gatt_ccc */
	if (attr->write != bt_gatt_attr_write_ccc) {
		return BT_GATT_ITER_CONTINUE;
	}

	ccc = attr->user_data;

	/* If already disabled skip */
	if (!ccc->value) {
		return BT_GATT_ITER_CONTINUE;
	}

	for (i = 0; i < ccc->cfg_len; i++) {
		/* Ignore configurations with disabled value */
		if (!ccc->cfg[i].value) {
			continue;
		}

		if (bt_addr_le_cmp(&conn->le.dst, &ccc->cfg[i].peer)) {
			struct bt_conn *tmp;

			/* Skip if there is another peer connected */
			tmp = bt_conn_lookup_addr_le(&ccc->cfg[i].peer);
			if (tmp && tmp->state == BT_CONN_CONNECTED) {
				bt_conn_put(tmp);
				return BT_GATT_ITER_CONTINUE;
			}
		}
	}

	/* Reset value while disconnected */
	memset(&ccc->value, 0, sizeof(ccc->value));
	ccc->cfg_changed(ccc->value);

	return BT_GATT_ITER_CONTINUE;
}

void bt_gatt_disconnected(struct bt_conn *conn)
{
	bt_gatt_foreach_attr(0x0001, 0xffff, disconnected_cb, conn);

	/* If paired don't remove subscriptions */
	//if (bt_keys_find_addr(&conn->le.dst)) {
	//	return;
	//}
#if NOT_USED_FOR_THE_TIME_BEING
	remove_subscribtions(conn);
#endif
}

#else

void on_ble_gattc_discover_rsp(const struct ble_gattc_disc_rsp *p_evt,
		const uint8_t *data, uint8_t data_len)
{

}
void on_ble_gattc_write_rsp(const struct ble_gattc_write_rsp *p_evt, void *p_priv)
{
}

void on_ble_gattc_value_evt(const struct ble_gattc_value_evt *p_evt,
		uint8_t *p_buf, uint8_t buflen)
{
}

void on_ble_gattc_read_rsp(const struct ble_gattc_read_rsp *p_evt,
		uint8_t *data, uint8_t data_len, void *p_priv)
{
}

#endif

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

void on_ble_gatts_send_svc_changed_rsp(const struct ble_core_response *p_params)
{
}
