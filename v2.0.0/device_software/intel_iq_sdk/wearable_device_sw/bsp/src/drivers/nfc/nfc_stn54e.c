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

#include <string.h>
#include <microkernel.h>
#include "infra/log.h"
#include "drivers/soc_gpio.h"
#include "drivers/nfc_stn54e.h"

#define NFC_WAKELOCK_TIMEOUT    1000
#define SBA_TIMEOUT             1000

static nfc_stn54e_rx_handler_t nfc_rx_handler = NULL;

/* Device driver internal functions */
static void nfc_sba_completion_callback(struct sba_request*);
static void nfc_gpio_callback(bool, void*);
static DRIVER_API_RC i2c_sync(struct device*, struct sba_request*);

static void nfc_gpio_callback(bool state, void *priv)
{
    struct device *dev = (struct device*)priv;
    pr_debug(LOG_MODULE_DRV, "STN54E gpio event %d", state);

    if(nfc_rx_handler != NULL)
        nfc_rx_handler((void*)dev);
}

static int nfc_stn54e_init(struct device *device)
{
    struct sba_device *dev = (struct sba_device*)device;
    struct nfc_stn54e_info *nfc_dev = (struct nfc_stn54e_info*)device->priv;

    /* Init wakelock */
    pm_wakelock_init(&nfc_dev->wakelock, nfc_dev->wakelock.id);

    /* Configure GPIO pins */
    if (!nfc_dev->gpio_dev) {
        /* Cannot retrive gpio device */
        pr_error(LOG_MODULE_DRV, "STN54E %d gpio failed", device->id);
        return -1;
    }

    gpio_cfg_data_t pin_cfg = {.gpio_type = GPIO_OUTPUT};
    soc_gpio_set_config(nfc_dev->gpio_dev, nfc_dev->stn_reset_pin, &pin_cfg);
#ifdef CONFIG_STN54E_HAS_PWR_EN
    soc_gpio_set_config(nfc_dev->gpio_dev, nfc_dev->stn_pwr_en_pin, &pin_cfg);
#endif
#ifdef CONFIG_STN54E_HAS_BOOSTER
    soc_gpio_set_config(nfc_dev->gpio_dev, nfc_dev->booster_reset_pin, &pin_cfg);
#endif

    nfc_stn54e_power_down();

    /* Configure IRQ_REQ pin */
    pin_cfg.gpio_type = GPIO_INTERRUPT;
    pin_cfg.int_type = EDGE;
    pin_cfg.int_polarity = ACTIVE_HIGH;
    pin_cfg.int_debounce = DEBOUNCE_ON;
    pin_cfg.gpio_cb = nfc_gpio_callback;
    pin_cfg.gpio_cb_arg = device;
    soc_gpio_set_config(nfc_dev->gpio_dev, nfc_dev->stn_irq_pin, &pin_cfg);

    /* Init sba_request struct */
    nfc_dev->req.addr.slave_addr = dev->addr.slave_addr;

    /* Create a taken semaphore for sba transfers */
    if ((nfc_dev->i2c_sync_sem = semaphore_create(0, NULL)) == NULL) {
        pr_error(LOG_MODULE_DRV, "STN54E %d semaphore failed", device->id);
        return -1;
    }

    pr_debug(LOG_MODULE_DRV, "STN54E %d init done.", device->id);
    return 0;
}

struct driver nfc_stn54e_driver = {
    .init =    nfc_stn54e_init,
    .suspend = NULL,
    .resume =  NULL
};

static void nfc_sba_completion_callback(struct sba_request *req)
{
    /* Give nfc sba semaphore to notify that the i2c transfer is complete */
    semaphore_give((T_SEMAPHORE)(req->priv_data), NULL);
}

static DRIVER_API_RC i2c_sync(struct device *dev, struct sba_request *req)
{
    DRIVER_API_RC ret;
    OS_ERR_TYPE ret_os;
    struct nfc_stn54e_info *nfc_dev = (struct nfc_stn54e_info*)dev->priv;

    req->priv_data = nfc_dev->i2c_sync_sem;
    req->callback = nfc_sba_completion_callback;

    if((ret = sba_exec_dev_request((struct sba_device*)dev, req)) == DRV_RC_OK) {
        /* Wait for transfer to complete (timeout = 100ms) */
        if ((ret_os = semaphore_take(nfc_dev->i2c_sync_sem, SBA_TIMEOUT)) != E_OS_OK) {
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

void nfc_stn54e_power_down(void)
{
    pr_info(LOG_MODULE_DRV, "NFC power down.");

    struct device *dev = (struct device*)&pf_sba_device_nfc;
    struct nfc_stn54e_info *nfc_dev = (struct nfc_stn54e_info*)dev->priv;

    /* Set reset low */
    soc_gpio_write(nfc_dev->gpio_dev, nfc_dev->stn_reset_pin, 0);

#ifdef CONFIG_STN54E_HAS_PWR_EN
    /* Cut off power */
    soc_gpio_write(nfc_dev->gpio_dev, nfc_dev->stn_pwr_en_pin, 0);
#endif

#ifdef CONFIG_STN54E_HAS_BOOSTER
    /* Cut off booster */
    soc_gpio_write(nfc_dev->gpio_dev, nfc_dev->booster_reset_pin, 0);
#endif
}

void nfc_stn54e_power_up(void)
{
    pr_info(LOG_MODULE_DRV, "NFC power-up/reset.");

    struct device *dev = (struct device*)&pf_sba_device_nfc;
    struct nfc_stn54e_info *nfc_dev = (struct nfc_stn54e_info*)dev->priv;

    /* Reset STN and booster */
    soc_gpio_write(nfc_dev->gpio_dev, nfc_dev->stn_reset_pin, 0);
#ifdef CONFIG_STN54E_HAS_BOOSTER
    soc_gpio_write(nfc_dev->gpio_dev, nfc_dev->booster_reset_pin, 0);
#endif
    local_task_sleep_ms(2);

#ifdef CONFIG_STN54E_HAS_BOOSTER
    /* Power up the booster */
    soc_gpio_write(nfc_dev->gpio_dev, nfc_dev->booster_reset_pin, 1);
    local_task_sleep_ms(2);
#endif

#ifdef CONFIG_STN54E_HAS_PWR_EN
    /* Power up STN54 */
    soc_gpio_write(nfc_dev->gpio_dev, nfc_dev->stn_pwr_en_pin, 1);
    local_task_sleep_ms(10);
#endif

    /* Set reset high */
    soc_gpio_write(nfc_dev->gpio_dev, nfc_dev->stn_reset_pin, 1);
    local_task_sleep_ms(80);
}

void nfc_stn54e_set_rx_handler(nfc_stn54e_rx_handler_t handler)
{
    nfc_rx_handler = handler;
}

void nfc_stn54e_clear_rx_handler()
{
    nfc_rx_handler = NULL;
}

DRIVER_API_RC nfc_stn54e_write(uint8_t * buf, uint8_t size)
{
    DRIVER_API_RC ret;
    uint8_t retry_cnt = 2;

    struct device *dev = (struct device*)&pf_sba_device_nfc;
    struct nfc_stn54e_info *nfc_dev = (struct nfc_stn54e_info*)dev->priv;

    if(!buf){
        pr_error(LOG_MODULE_DRV, "Wrong arguments.");
        return DRV_RC_INVALID_CONFIG;
    }

    nfc_dev->req.request_type = SBA_TX;
    nfc_dev->req.tx_len = size;
    nfc_dev->req.tx_buff = buf;

    do {
        ret = i2c_sync(dev, &nfc_dev->req);
        if(ret == DRV_RC_OK)
            break;

        pr_debug(LOG_MODULE_DRV, "NFC: tx retry, prev status: %d", ret);

        retry_cnt--;
        local_task_sleep_ms(5);
    } while (retry_cnt > 0);

    if(ret != DRV_RC_OK){
        pr_error(LOG_MODULE_DRV, "NFC: Failed tx data!");
    }

    return ret;
}

DRIVER_API_RC nfc_stn54e_read(uint8_t * buf,  uint8_t size)
{
    if(!buf){
        pr_error(LOG_MODULE_DRV, "Wrong arguments.");
        return DRV_RC_INVALID_CONFIG;
    }

    struct device *dev = (struct device*)&pf_sba_device_nfc;
    struct nfc_stn54e_info *nfc_dev = (struct nfc_stn54e_info*)dev->priv;

    nfc_dev->req.request_type = SBA_RX;
    nfc_dev->req.rx_len = size;
    nfc_dev->req.rx_buff = buf;

    return i2c_sync(dev, &nfc_dev->req);
}
