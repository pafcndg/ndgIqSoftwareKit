/******************************************************************************
 *
 * Synopsys DesignWare Sensor and Control IP Subsystem IO Software Driver and
 * documentation (hereinafter, "Software") is an Unsupported proprietary work
 * of Synopsys, Inc. unless otherwise expressly agreed to in writing between
 * Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 *****************************************************************************/

/******************************************************************************
 *
 * Modifications Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 *****************************************************************************/

#include "drivers/ss_adc.h"

#include <stdio.h>
#include <stdlib.h>
#include <nanokernel.h>
#include <arch/cpu.h>

#include "machine.h"
#include "drivers/io_config.h"
#include "drivers/eiaextensions.h"
#include "adc_priv.h"
#include "infra/device.h"
#include "infra/log.h"
#include "drivers/clk_system.h"

#define DRV_NAME "adc"
#define ADC_MAX_CNT (1)

#define ADC_CLOCK_GATE          (1 << 31)
#define ADC_STANDBY             (0x02)
#define ADC_NORMAL_WO_CALIB     (0x04)
#define ADC_MODE_MASK           (0x07)

#define ONE_BIT_SET     (0x1)
#define FIVE_BITS_SET   (0x1f)
#define SIX_BITS_SET    (0x3f)
#define ELEVEN_BITS_SET (0x7ff)

#define INPUT_MODE_POS      (5)
#define CAPTURE_MODE_POS    (6)
#define OUTPUT_MODE_POS     (7)
#define SERIAL_DELAY_POS    (8)
#define SEQUENCE_MODE_POS   (13)
#define SEQ_ENTRIES_POS     (16)
#define THRESHOLD_POS       (24)

#define SEQ_DELAY_EVEN_POS  (5)
#define SEQ_MUX_ODD_POS     (16)
#define SEQ_DELAY_ODD_POS   (21)

#define ADC_PM_FSM_STATUS_MSK   (1 << 3)

static void adc_goto_normal_mode_wo_calibration(struct device *dev);
static void adc_goto_deep_power_down(struct device *dev);
static bool ss_adc_lock(struct device *dev);
static void ss_adc_enable(struct device *dev);
static void ss_adc_disable(struct device *dev);

DECLARE_INTERRUPT_HANDLER static void adc0_rx_ISR();
DECLARE_INTERRUPT_HANDLER static void adc0_err_ISR();


static void adc_rx_ISR_proc(struct device *dev)
{
	struct adc_info_t *info = dev->priv;
	uint32_t i, reg_val, rx_cnt;
	uint32_t idx = info->index;
	uint32_t rd = 0;

	if (IO_ADC_SEQ_MODE_REPETITIVE == info->seq_mode) {
		if (NULL == info->rx_buf[idx])
			goto cli;
		rx_cnt = (info->fifo_tld + 1);
	} else { /* IO_ADC_SEQ_MODE_SINGLESHOT mode */
		rx_cnt = info->seq_size;
	}

	if (rx_cnt > info->rx_len)
		rx_cnt = info->rx_len;
	for (i = 0; i < rx_cnt; i++) {
		reg_val = READ_ARC_REG(info->reg_base + ADC_SET);
		WRITE_ARC_REG(reg_val | ADC_POP_SAMPLE,
			      info->reg_base + ADC_SET);
		rd = READ_ARC_REG(info->reg_base + ADC_SAMPLE);
		info->rx_buf[idx][i] = rd;
	}

	info->rx_buf[idx] += i;
	info->rx_len -= i;

	if (0 == info->rx_len) {
		if (_Usually(NULL != info->cfg.cb_rx))
			info->cfg.cb_rx(dev->id, info->cfg.priv);
		if (IO_ADC_SEQ_MODE_SINGLESHOT == info->seq_mode) {
			WRITE_ARC_REG(ADC_INT_DSB | ADC_CLK_ENABLE |
				      ADC_SEQ_PTR_RST, info->reg_base +
				      ADC_CTRL);
			reg_val = READ_ARC_REG(info->reg_base + ADC_SET);
			WRITE_ARC_REG(reg_val | ADC_FLUSH_RX,
				      info->reg_base + ADC_SET);
			info->state = ADC_STATE_IDLE;
			goto cli;
		}
		info->rx_buf[idx] = NULL;
		idx++;
		idx %= ADC_BUFS_NUM;
		info->index = idx;
	} else if (IO_ADC_SEQ_MODE_SINGLESHOT == info->seq_mode) {
		WRITE_ARC_REG(ADC_INT_DSB | ADC_CLK_ENABLE | ADC_SEQ_PTR_RST,
			      info->reg_base + ADC_CTRL);

		info->state = ADC_STATE_IDLE;
		if (_Usually(NULL != info->cfg.cb_rx))
			info->cfg.cb_rx(dev->id, info->cfg.priv);
	}
cli:
	reg_val = READ_ARC_REG(info->reg_base + ADC_CTRL);
	WRITE_ARC_REG(reg_val | ADC_CLR_DATA_A, info->reg_base + ADC_CTRL);
	ss_adc_disable(dev);
	pm_wakelock_release(&info->wakelock);
}

static void adc_err_ISR_proc(struct device *dev)
{
	struct adc_info_t *info = dev->priv;
	uint32_t reg_val = READ_ARC_REG(info->reg_base + ADC_SET);

	WRITE_ARC_REG(ADC_INT_DSB | ADC_CLK_ENABLE | ADC_SEQ_PTR_RST,
		      info->reg_base + ADC_CTRL);
	WRITE_ARC_REG(reg_val | ADC_FLUSH_RX, info->reg_base + ADC_SET);
	info->state = ADC_STATE_IDLE;

	WRITE_ARC_REG(ADC_INT_DSB | ADC_CLK_ENABLE | ADC_CLR_OVERFLOW |
		      ADC_CLR_UNDRFLOW, info->reg_base +
		      ADC_CTRL);

	if (_Usually(NULL != info->cfg.cb_err))
		info->cfg.cb_err(dev->id, info->cfg.priv);
	ss_adc_disable(dev);
	pm_wakelock_release(&info->wakelock);
}


/* ADC devices private data structures */
/* TODO: For now this drive can only be used with one controller. Remove once
 * we can handle interrupts with parameters.
 */
struct device *adc_handle = NULL;

DECLARE_INTERRUPT_HANDLER static void adc0_rx_ISR()
{
	adc_rx_ISR_proc(adc_handle);
}
DECLARE_INTERRUPT_HANDLER static void adc0_err_ISR()
{
	adc_err_ISR_proc(adc_handle);
}

ss_adc_cfg_data_t drv_config[ADC_MAX_CNT];

bool ss_adc_set_config(struct device *dev, ss_adc_cfg_data_t *config)
{
	struct adc_info_t *info = dev->priv;
	uint32_t reg_val = 0, val = 0, ctrl = 0;
	ss_adc_cfg_data_t *cfg = &info->cfg;
	bool locked = ss_adc_lock(dev);

	if (!locked)
		return false;

	/* TODO: remove?? */
	adc_handle = dev;

	*cfg = *config;

	ctrl = ADC_INT_DSB | ADC_CLK_ENABLE;
	WRITE_ARC_REG(ctrl, info->reg_base + ADC_CTRL);

	/* set sample width, input mode, output mode and serial delay */
	reg_val = READ_ARC_REG(info->reg_base + ADC_SET);
	reg_val &= ADC_CONFIG_SET_MASK;
	val = (cfg->sample_width) & FIVE_BITS_SET;
	val |= ((cfg->in_mode & ONE_BIT_SET) << INPUT_MODE_POS);
	val |= ((cfg->capture_mode & ONE_BIT_SET) << CAPTURE_MODE_POS);
	val |= ((cfg->out_mode & ONE_BIT_SET) << OUTPUT_MODE_POS);
	val |= ((cfg->serial_dly & FIVE_BITS_SET) << SERIAL_DELAY_POS);
	val |= ((cfg->seq_mode & ONE_BIT_SET) << SEQUENCE_MODE_POS);
	WRITE_ARC_REG(reg_val | val, info->reg_base + ADC_SET);

	info->rx_len = 0;
	info->seq_mode = cfg->seq_mode;
	info->seq_size = 1;
	info->state = ADC_STATE_IDLE;

	/* set  clock ratio */
	WRITE_ARC_REG(cfg->clock_ratio & ADC_CLK_RATIO_MASK,
		      info->reg_base + ADC_DIVSEQSTAT);


	/* disable clock once setup done */
	WRITE_ARC_REG(ADC_INT_ENABLE & ~(ADC_CLK_ENABLE),
		      info->reg_base + ADC_CTRL);

	/*
	 * SoC ADC config
	 */
	/* TODO: move to init? */
	/* Setup ADC Interrupt Routing Mask Registers to allow interrupts through */
	MMIO_REG_VAL(info->adc_irq_mask) &= ENABLE_SSS_INTERRUPTS;
	MMIO_REG_VAL(info->adc_err_mask) &= ENABLE_SSS_INTERRUPTS;

	info->state = ADC_STATE_IDLE;
	return true;
}

static bool ss_adc_lock(struct device *dev)
{
	struct adc_info_t *info = dev->priv;
	uint32_t saved = interrupt_lock();

	if (info->adc_in_use) {
		interrupt_unlock(saved);
		return false;
	}
	info->adc_in_use = true;
	interrupt_unlock(saved);
	return true;
}

static void ss_adc_enable(struct device *dev)
{
	struct adc_info_t *info = dev->priv;

	adc_goto_normal_mode_wo_calibration(dev);
	/* Enable adc clock and reset sequence pointer */
	set_clock_gate(info->clk_gate_info, CLK_GATE_ON);
	WRITE_ARC_REG(ADC_INT_ENABLE | ADC_CLK_ENABLE | ADC_SEQ_TABLE_RST,
		      info->reg_base + ADC_CTRL);
}

static void ss_adc_disable(struct device *dev)
{
	struct adc_info_t *info = dev->priv;
	uint32_t saved;

	adc_goto_deep_power_down(dev);
	WRITE_ARC_REG(ADC_INT_DSB | ADC_SEQ_PTR_RST, info->reg_base + ADC_CTRL);

	/* No need to protect ADC_SET using lock and unlock of interruptions,
	 * we call it qith interrupts locked already or in interrupt context.
	 */
	saved = interrupt_lock();
	info->cfg.cb_err = NULL;
	info->cfg.cb_rx = NULL;
	/* TODO sak should we do this on disable ?? */
	WRITE_ARC_REG(READ_ARC_REG(info->reg_base + ADC_SET) | ADC_FLUSH_RX,
		      info->reg_base + ADC_SET);
	set_clock_gate(info->clk_gate_info, CLK_GATE_OFF);
	interrupt_unlock(saved);

	info->state = ADC_STATE_DISABLED;
	info->adc_in_use = false;
}


DRIVER_API_RC ss_adc_read(struct device *dev, io_adc_seq_table_t *seq_tbl,
			  uint32_t *data, uint32_t data_len)
{
	struct adc_info_t *info = dev->priv;
	uint32_t ctrl = 0, reg_val = 0;
	uint32_t i = 0, num_iters = 0;
	io_adc_seq_entry_t *entry = NULL;
	uint32_t saved;
	/* Protect ADC_SET and ADC_CTRL using lock and unlock of interruptions */
	saved = interrupt_lock();

	if (!info->adc_in_use) {
		/* lock before read */
		interrupt_unlock(saved);
		return DRV_RC_INVALID_CONFIG;
	}
	if (ADC_STATE_IDLE != info->state) {
		interrupt_unlock(saved);
		return DRV_RC_CONTROLLER_IN_USE;
	}
	/* Prevent device shutdown via wakelock */
	pm_wakelock_acquire(&info->wakelock);

	ss_adc_enable(dev);

	/* Reset Sequence Pointer */
	ctrl = READ_ARC_REG(info->reg_base + ADC_CTRL);
	ctrl |= ADC_SEQ_PTR_RST;
	WRITE_ARC_REG(ctrl, (info->reg_base + ADC_CTRL));

	/* Setup sequence table */
	info->seq_size = seq_tbl->num_entries;

	reg_val = READ_ARC_REG(info->reg_base + ADC_SET);
	reg_val &= ADC_SEQ_SIZE_SET_MASK;
	reg_val |=
		(((seq_tbl->num_entries - 1) & SIX_BITS_SET) << SEQ_ENTRIES_POS);
	reg_val |= ((info->seq_size - 1) << THRESHOLD_POS);
	WRITE_ARC_REG(reg_val, info->reg_base + ADC_SET);

	num_iters = seq_tbl->num_entries / 2;

	for (i = 0, entry = seq_tbl->entries;
	     i < num_iters;
	     i++, entry += 2) {
		reg_val =
			((entry[1].sample_dly &
			  ELEVEN_BITS_SET) << SEQ_DELAY_ODD_POS);
		reg_val |=
			((entry[1].channel_id &
			  FIVE_BITS_SET) << SEQ_MUX_ODD_POS);
		reg_val |=
			((entry[0].sample_dly &
			  ELEVEN_BITS_SET) << SEQ_DELAY_EVEN_POS);
		reg_val |= (entry[0].channel_id & FIVE_BITS_SET);
		WRITE_ARC_REG(reg_val, info->reg_base + ADC_SEQ);
	}
	if (0 != (seq_tbl->num_entries % 2)) {
		reg_val =
			((entry[0].sample_dly &
			  ELEVEN_BITS_SET) << SEQ_DELAY_EVEN_POS);
		reg_val |= (entry[0].channel_id & FIVE_BITS_SET);
		WRITE_ARC_REG(reg_val, info->reg_base + ADC_SEQ);
	}
	WRITE_ARC_REG(ctrl | ADC_SEQ_PTR_RST, info->reg_base + ADC_CTRL);

	if (ADC_STATE_IDLE == info->state) {
		for (i = 0; i < ADC_BUFS_NUM; i++)
			info->rx_buf[i] = NULL;
		info->rx_buf[0] = data;
		info->rx_len = data_len;
		info->index = 0;
		info->state = ADC_STATE_SAMPLING;
		/* enable AD converter, start sequencer */
		WRITE_ARC_REG(ADC_SEQ_START | ADC_ENABLE | ADC_CLK_ENABLE,
			      info->reg_base + ADC_CTRL);
	} else if (IO_ADC_SEQ_MODE_REPETITIVE == info->seq_mode) {
		uint32_t idx = info->index;

		if (NULL == info->rx_buf[idx]) {
			info->rx_buf[idx] = data;
			info->rx_len = data_len;
		}
	}
	interrupt_unlock(saved);
	return DRV_RC_OK;
}
/*
 * Function to put the ADC controller into a working state.
 */
static void adc_goto_normal_mode_wo_calibration(struct device *dev)
{
	struct adc_info_t *info = dev->priv;
	uint32_t creg;
	uint32_t saved;

	// read creg slave to get current Power Mode
	creg = READ_ARC_REG(info->creg_slv);

	// perform power up to "Normal mode w/o calibration" cycle if not already there
	if ((creg & ADC_MODE_MASK) != ADC_NORMAL_WO_CALIB) {
		/* Protect AR_IO_CREG_MST0_CTRL using lock and unlock of interruptions */
		saved = interrupt_lock();
		// Read current CREG master
		creg = READ_ARC_REG(info->creg_mst);
		creg &= ~(ADC_MODE_MASK);
		// request ADC to go to Standby mode
		creg |= ADC_STANDBY | ADC_CLOCK_GATE;
		WRITE_ARC_REG(creg, info->creg_mst);
		interrupt_unlock(saved);

		// Poll CREG Slave 0 for Power Mode status = requested status
		while (((creg = READ_ARC_REG(info->creg_slv)) &
			ADC_PM_FSM_STATUS_MSK) == 0) ;

		/* Protect AR_IO_CREG_MST0_CTRL using lock and unlock of interruptions */
		saved = interrupt_lock();
		creg = READ_ARC_REG(info->creg_mst);

		creg &= ~(ADC_MODE_MASK);
		// request ADC to go to Normal mode w/o calibration
		creg |= ADC_NORMAL_WO_CALIB | ADC_CLOCK_GATE;
		WRITE_ARC_REG(creg, info->creg_mst);
		interrupt_unlock(saved);

		// Poll CREG Slave 0 for Power Mode status = requested status
		while (((creg = READ_ARC_REG(info->creg_slv)) &
			ADC_PM_FSM_STATUS_MSK) == 0) ;
	}
}

static void adc_goto_deep_power_down(struct device *dev)
{
	struct adc_info_t *info = dev->priv;
	uint32_t creg;
	uint32_t saved;

	// read creg slave to get current Power Mode
	creg = READ_ARC_REG(info->creg_slv);
	// perform cycle down to "Deep Power Down mode" if not already there
	if ((creg & ADC_MODE_MASK) != 0) {
		/* Protect AR_IO_CREG_MST0_CTRL using lock and unlock of interruptions */
		saved = interrupt_lock();

		// Read current CREG master
		creg = READ_ARC_REG(info->creg_mst);
		creg &= ~(ADC_MODE_MASK);
		// request ADC to go to Deep Power Down mode
		creg |= 0 | ADC_CLOCK_GATE;
		WRITE_ARC_REG(creg, info->creg_mst);
		interrupt_unlock(saved);

		// Poll CREG Slave 0 for Power Mode status = requested status
		while (((creg = READ_ARC_REG(info->creg_slv)) &
			ADC_PM_FSM_STATUS_MSK) == 0) ;
	}
}

static void ss_adc_setup_irq(struct device *dev)
{
	struct adc_info_t *info = dev->priv;

	/* set interrupt vector, mid/high priority */
	SET_INTERRUPT_HANDLER(info->rx_vector, adc0_rx_ISR);
	SET_INTERRUPT_HANDLER(info->err_vector, adc0_err_ISR);
}

static int ss_adc_init(struct device *dev)
{
	struct adc_info_t *info = dev->priv;

	ss_adc_setup_irq(dev);
	ss_adc_disable(dev); /* disable IP by default */
	info->adc_in_use = false;

	pr_debug(LOG_MODULE_DRV, "%s %d init", DRV_NAME, dev->id);
	return 0;
}


static int ss_adc_suspend(struct device *dev, PM_POWERSTATE state)
{
	pr_debug(LOG_MODULE_DRV, "%s %d suspend", DRV_NAME, dev->id);
	return 0;
}

static int ss_adc_resume(struct device *dev)
{
	struct adc_info_t *info = dev->priv;
	uint32_t tmp_state = info->state;
	pr_debug(LOG_MODULE_DRV, "%s %d resume", DRV_NAME, dev->id);

	/* disable IP by default */
	ss_adc_disable(dev);
	/* re-enable irqs */
	interrupt_enable(info->rx_vector);
	interrupt_enable(info->err_vector);

	/* reconfigure if necessary */
	if (tmp_state != ADC_STATE_DISABLED) {
		if (!ss_adc_set_config(dev, &info->cfg))
			return -1;
		info->state = tmp_state;
	}
	return 0;
}

struct driver ss_adc_driver = {
	.init = ss_adc_init,
	.suspend = ss_adc_suspend,
	.resume = ss_adc_resume,
};
