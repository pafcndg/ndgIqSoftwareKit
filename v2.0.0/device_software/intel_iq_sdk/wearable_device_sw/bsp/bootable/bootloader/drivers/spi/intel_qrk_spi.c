/* --------------------------------------------------------------------
**
** Synopsys DesignWare AMBA Software Driver Kit and
** documentation (hereinafter, "Software") is an Unsupported
** proprietary work of Synopsys, Inc. unless otherwise expressly
** agreed to in writing between Synopsys and you.
**
** The Software IS NOT an item of Licensed Software or Licensed
** Product under any End User Software License Agreement or Agreement
** for Licensed Product with Synopsys or any supplement thereto. You
** are permitted to use and redistribute this Software in source and
** binary forms, with or without modification, provided that
** redistributions of source code must retain this notice. You may not
** view, use, disclose, copy or distribute this file or any information
** contained herein except pursuant to this license grant from Synopsys.
** If you do not agree with this notice, including the disclaimer
** below, then you are not authorized to use the Software.
**
** THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS"
** BASIS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
** FOR A PARTICULAR PURPOSE ARE HEREBY DISCLAIMED. IN NO EVENT SHALL
** SYNOPSYS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
** OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
** USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
**
** --------------------------------------------------------------------
*/

/****************************************************************************************
 *
 * Modifications Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 ***************************************************************************************/
/*
 * Intel SOC SPI driver
 *
 */

#include <spi/quark_spi.h>
#include "soc_spi_priv.h"
#include <printk.h>

/*!
 *  Clock speed into SPI peripheral - set at compile time
 */
#define FREQ_SPI_CLOCK_IN   (CONFIG_CLOCK_SPEED) /* CLOCK_SPEED in MHz */

/* Software workaround:
 * --------------------
 * We are currently encountering problems with the SPI0_Master chip select.
 * Although it should be drived automatically by the spi module itself during a transaction,
 * it is not the case.
 * This software workaround allows driving directly the cs pin using gpios.
 * Configuration is done in the pinmux.c file.
 *  */
#define ATP_HW_V1

const SOC_SPI_CONTROLLER reg_base = CONFIG_SPI_MASTER;


DRIVER_API_RC spi_fill_fifo(uint32_t *tx_count, uint32_t tx_total_count, uint8_t *tx_data, uint32_t tx_data_len);
DRIVER_API_RC spi_read_fifo(uint32_t *rx_count, uint32_t rx_total_count, uint8_t *rx_data, uint32_t rx_data_len);
void transfer_complete();

#ifdef ATP_HW_V1
static void cs_config(uint8_t gpio)
{
    MMIO_REG_VAL_FROM_BASE(SOC_GPIO_BASE_ADDR, SOC_GPIO_SWPORTA_DDR) |= (1<<gpio);
}

static void cs_low(uint8_t gpio)
{
    MMIO_REG_VAL_FROM_BASE(SOC_GPIO_BASE_ADDR, SOC_GPIO_SWPORTA_DR) &= ~(1<<gpio);
}

static void cs_high(uint8_t gpio)
{
    MMIO_REG_VAL_FROM_BASE(SOC_GPIO_BASE_ADDR, SOC_GPIO_SWPORTA_DR) |= (1<<gpio);
}
#endif

DRIVER_API_RC soc_spi_set_config(spi_cfg_data_t *config)
{

    /* disable controller */
    MMIO_REG_VAL_FROM_BASE(reg_base, SPIEN) &= SPI_DISABLE;

    /* Set frame size, bus mode and transfer mode */
    uint32_t reg = (config->data_frame_size << 16) |
          (config->bus_mode << 6) | (config->txfr_mode << 8 );

    MMIO_REG_VAL_FROM_BASE(reg_base, CTRL0) = reg;

    /* Set SPI Clock Divider */
    reg = (FREQ_SPI_CLOCK_IN / (config->speed*BAUD_DIVISOR));
    MMIO_REG_VAL_FROM_BASE(reg_base, BAUDR) = reg;

    /* Disable interrupts */
    MMIO_REG_VAL_FROM_BASE(reg_base, IMR) = SPI_DISABLE_INT;
    /* Enable device */
    MMIO_REG_VAL_FROM_BASE(reg_base, SPIEN) |= SPI_ENABLE;

    return DRV_RC_OK;
}


DRIVER_API_RC soc_spi_deconfig()
{

    /* disable controller */
    MMIO_REG_VAL_FROM_BASE(reg_base, SPIEN) &= SPI_DISABLE;


    return DRV_RC_OK;
}

#ifdef ATP_HW_V1
/* use gpio for spi chip select */
int soc_spi_cs_get_gpio(int slave)
{
    /* If SPI_M0, use GPIO mode */
    if(reg_base == SPI_0)
    {
        switch(slave)
        {
            case SPI_SE_1:
                return SPIM0_CS_1_GPIO24;
            case SPI_SE_2:
                return SPIM0_CS_2_GPIO25;
            case SPI_SE_3:
                return SPIM0_CS_3_GPIO26;
            case SPI_SE_4:
                return SPIM0_CS_4_GPIO27;
            default:
                break;
        }
    }
    return -1;
}
#endif
DRIVER_API_RC soc_spi_transfer(uint8_t *tx_data, uint32_t tx_data_len, uint8_t *rx_data, uint32_t rx_data_len, int full_duplex, SPI_SLAVE_ENABLE slave)
{
    /* Disable device */
    MMIO_REG_VAL_FROM_BASE(reg_base, SPIEN) &= SPI_DISABLE;


    /* Enable slave device */
    MMIO_REG_VAL_FROM_BASE(reg_base, SER) = slave;

    MMIO_REG_VAL_FROM_BASE(reg_base, IMR) = SPI_DISABLE_INT;

    /* Enable device */
    MMIO_REG_VAL_FROM_BASE(reg_base, SPIEN) |= SPI_ENABLE;

#ifdef ATP_HW_V1
    int gpio = soc_spi_cs_get_gpio(slave);
    if (gpio < 0) return DRV_RC_FAIL;

    cs_config(gpio);
    cs_low(gpio);

#endif
    uint32_t tx_count = 0;
    uint32_t rx_count = 0;
    uint32_t tx_total_count;

    if (full_duplex){
        if (tx_data_len >= rx_data_len)
        {
            tx_total_count = tx_data_len;
        }
        else
        {
            tx_total_count = rx_data_len;
        }
    }
    else
    {
        tx_total_count = tx_data_len + rx_data_len;
    }

    pr_debug("tx_data_lent %d; rx_data_len %d tx_total_count %d\r\n", tx_data_len, rx_data_len, tx_total_count);

    while(1){
        DRIVER_API_RC ret_fill, ret_read;
        ret_fill = spi_fill_fifo(&tx_count, tx_total_count, tx_data, tx_data_len);
        ret_read = spi_read_fifo(&rx_count, tx_total_count, rx_data, rx_data_len);
        if ((ret_fill == DRV_RC_OK) && (ret_read == DRV_RC_OK)) break;
    }

    transfer_complete();
#ifdef ATP_HW_V1
    cs_high(gpio);
#endif
    return DRV_RC_OK;
}

DRIVER_API_RC spi_fill_fifo(uint32_t *tx_count, uint32_t tx_total_count, uint8_t *tx_data, uint32_t tx_data_len)
{
    uint32_t status = MMIO_REG_VAL_FROM_BASE(reg_base, SR);
    while( (status & SPI_STATUS_TFNF) &&
           ((MMIO_REG_VAL_FROM_BASE(reg_base, TXFL) +
                MMIO_REG_VAL_FROM_BASE(reg_base, RXFL)) < SPI_TX_FIFO_THRESHOLD )) {
        if (*tx_count < tx_data_len)
        {
            pr_debug("push data %x\n",tx_data[*tx_count]);
            MMIO_REG_VAL_FROM_BASE(reg_base, DR) = tx_data[*tx_count];
            (*tx_count)++;
        }
        else if (*tx_count < tx_total_count)
        {
            pr_debug("push dummy 0\n");
            MMIO_REG_VAL_FROM_BASE(reg_base, DR) = 0;
            (*tx_count)++;
        }
        else
        {
            /*tx complete */
            return DRV_RC_OK;
        }
        status = MMIO_REG_VAL_FROM_BASE(reg_base, SR);
    }
    return DRV_RC_FAIL;
}

DRIVER_API_RC spi_read_fifo(uint32_t *rx_count, uint32_t rx_total_count, uint8_t *rx_data, uint32_t rx_data_len)
{


    /* Receive data */
    while(MMIO_REG_VAL_FROM_BASE(reg_base, RXFL) > 0)
    {
        if (rx_total_count - *rx_count > rx_data_len)
        {
            uint32_t tmp;
            tmp = MMIO_REG_VAL_FROM_BASE(reg_base, DR);
            (void) tmp;
            pr_debug("discard %x\n", tmp);
            (*rx_count)++;
        }
        else if (*rx_count < rx_total_count)
        {
            rx_data[*rx_count + rx_data_len - rx_total_count] = MMIO_REG_VAL_FROM_BASE(reg_base, DR);
            pr_debug("read[%d] %x\n",*rx_count + rx_data_len - rx_total_count , rx_data[*rx_count + rx_data_len - rx_total_count]);
            (*rx_count)++;
        }
    }
    if (*rx_count >= rx_total_count) return DRV_RC_OK;
    return DRV_RC_FAIL;
}

void transfer_complete()
{
    uint32_t tmp;
    do
    {
        tmp = MMIO_REG_VAL_FROM_BASE(reg_base, SR);
    } while((tmp & SPI_STATUS_BUSY));
}
