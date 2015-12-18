/* INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors.
 * The Material contains trade secrets and proprietary and confidential information
 * of Intel or its suppliers and licensors. The Material is protected by worldwide
 * copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published,
 * uploaded, posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.
 *
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing
 *
 ******************************************************************************/

#include "drivers/apds9190.h"
#include "infra/log.h"
#include <cfw/cfw.h>
#include <cfw/cfw_client.h>
#include <services/service_queue.h>
#include <services/gpio_service.h>
#include <microkernel.h>
#include "drivers/soc_gpio.h"

#define SBA_TIMEOUT		1000

#define REG_ENABLE         0x00
#define REG_PTIME          0x02
#define REG_WTIME          0x03
#define REG_PILT           0x08
#define REG_PIHT           0x0A
#define REG_PERS           0x0C
#define REG_CONFIG         0x0D
#define REG_PPCOUNT        0x0E
#define REG_CONTROL        0x0F
#define REG_REV            0x11
#define REG_STATUS         0x13
#define REG_PDATA          0x18

#define ENABLE_PIEN        (1<<5)
#define ENABLE_WEN         (1<<3)
#define ENABLE_PEN         (1<<2)
#define ENABLE_PON         (1<<0)

#define CONTROL_ENABLE_CH1 0x20

#define COMMAND_ENABLE     0x80
#define COMMAND_AI         0x20
#define COMMAND_CLEAR      0xe5

/* Device driver internal functions */
static void apds_sba_completion_callback(struct sba_request*);
static DRIVER_API_RC i2c_sync_write(struct device*, int len, uint8_t *data);
static int apds9190_set_enable(struct device *dev, uint8_t reg);
static int apds9190_clear_irq(struct device *dev);
void apds9190_update_config(struct device *dev);

static int apds9190_init(struct device *device)
{
	int ret;
	struct sba_device *dev = (struct sba_device*)device;
	struct apds9190_info *ir_dev = (struct apds9190_info*)device->priv;

	pr_debug(LOG_MODULE_DRV, "APDS9190 %d init (master=%d)", device->id, device->parent->id);

	/* Create a taken semaphore for sba transfers */
	if ((ir_dev->i2c_sync_sem = semaphore_create(0, NULL)) == NULL) {
		return -1;
	}
	/* Init sba_request struct */
	ir_dev->req.addr.slave_addr = dev->addr.slave_addr;
	ir_dev->req.request_type = SBA_TX;

	/* TODO: Enable comparator interrupt for SP2 */
	/* Enable device */
	ret = apds9190_set_enable(device, ENABLE_PON);

	if (ret != 0) {
		pr_error(LOG_MODULE_DRV, "apds9190 %d probe %d\n", device->id, ret);
		return ret;
	}
	apds9190_update_config(device);
	apds9190_clear_irq(device);
	return apds9190_set_enable(device, ENABLE_PON |
					   ENABLE_PIEN |
					   ENABLE_WEN |
					   ENABLE_PEN);
}

static int apds9190_suspend(struct device *dev, PM_POWERSTATE state)
{
	pr_debug(LOG_MODULE_DRV, "apds9190 suspend %d (%d)", dev->id, state);
	return 0;
}

static int apds9190_resume(struct device *dev)
{
	pr_debug(LOG_MODULE_DRV, "apds9190 resume %d (%d)", dev->id);
	return 0;
}

struct driver apds9190_driver = {
		.init =    apds9190_init,
		.suspend = apds9190_suspend,
		.resume =  apds9190_resume
};

static void apds_sba_completion_callback(struct sba_request *req)
{
	/* Give led sba semaphore to notify that the i2c transfer is complete */
	semaphore_give((T_SEMAPHORE)(req->priv_data), NULL);
}

static int i2c_read_reg16(struct device *dev, uint8_t reg)
{
	DRIVER_API_RC ret;
	OS_ERR_TYPE ret_os;
	struct apds9190_info *ir_dev = (struct apds9190_info*)dev->priv;
	struct sba_request * req = &ir_dev->req;
	uint8_t rsp[2];
	uint8_t cmd = COMMAND_ENABLE | reg;

	req->request_type = SBA_TRANSFER;
	req->tx_buff = &cmd;
	req->tx_len = 1;
	req->rx_buff = rsp;
	req->rx_len = 2;
	req->priv_data = ir_dev->i2c_sync_sem;
	req->callback = apds_sba_completion_callback;
	req->full_duplex = 1;

	if((ret = sba_exec_dev_request((struct sba_device*)dev, req)) == DRV_RC_OK) {
		/* Wait for transfer to complete (timeout = 100ms) */
		if ((ret_os = semaphore_take(ir_dev->i2c_sync_sem, SBA_TIMEOUT)) != E_OS_OK) {
			if (ret_os == E_OS_ERR_BUSY) {
				ret = DRV_RC_TIMEOUT;
			} else {
				ret = DRV_RC_FAIL;
			}
		} else {
			ret = req->status;
		}
	}

	if (ret < 0)
		return ret;
	else
		return rsp[0] | rsp[1] << 8;
}

static DRIVER_API_RC i2c_sync_write(struct device *dev, int len, uint8_t * data)
{
	DRIVER_API_RC ret;
	OS_ERR_TYPE ret_os;
	struct apds9190_info *ir_dev = (struct apds9190_info*)dev->priv;
	struct sba_request * req = &ir_dev->req;
	req->request_type = SBA_TX;
	req->priv_data = ir_dev->i2c_sync_sem;
	req->callback = apds_sba_completion_callback;
	req->tx_len = len;
	req->tx_buff = data;

	if((ret = sba_exec_dev_request((struct sba_device*)dev, req)) == DRV_RC_OK) {
		/* Wait for transfer to complete (timeout = 100ms) */
		if ((ret_os = semaphore_take(ir_dev->i2c_sync_sem, SBA_TIMEOUT)) != E_OS_OK) {
			if (ret_os == E_OS_ERR_BUSY) {
				ret = DRV_RC_TIMEOUT;
			} else {
				ret = DRV_RC_FAIL;
			}
		} else {
			ret = req->status;
		}
	}

	return ret;
}

static DRIVER_API_RC i2c_write_reg(struct device *dev, uint8_t reg, uint8_t data)
{
	uint8_t buf[2];
	buf[0] = COMMAND_ENABLE | reg;
	buf[1] = data;
	return i2c_sync_write(dev, 2, buf);
}

static int apds9190_clear_irq(struct device *dev)
{
	uint8_t cmd = COMMAND_CLEAR;
	return i2c_sync_write(dev, 1, &cmd);
}

static int apds9190_set_enable(struct device *dev, uint8_t reg)
{
	return i2c_write_reg(dev, REG_ENABLE, reg);
}

int apds9190_read_prox(struct device *dev)
{
	return i2c_read_reg16(dev, REG_PDATA);
}

void apds9190_update_config(struct device *dev)
{
	struct apds9190_info *ir_dev = (struct apds9190_info*)dev->priv;
	uint8_t cmd[5];

	i2c_write_reg(dev, REG_PTIME, ir_dev->ptime);
	i2c_write_reg(dev, REG_WTIME, ir_dev->wtime);
	i2c_write_reg(dev, REG_PERS, ir_dev->pers);
	i2c_write_reg(dev, REG_CONFIG, ir_dev->config);
	i2c_write_reg(dev, REG_PPCOUNT, ir_dev->ppcount);
	i2c_write_reg(dev, REG_CONTROL, CONTROL_ENABLE_CH1);

	/* Write thresholds */
	cmd[1] = ir_dev->pilt;
	cmd[2] = ir_dev->pilt >> 8;
	cmd[3] = ir_dev->piht;
	cmd[4] = ir_dev->piht >> 8;
	cmd[0] = COMMAND_ENABLE | COMMAND_AI | REG_PILT;
	i2c_sync_write(dev, 5, cmd);
}
