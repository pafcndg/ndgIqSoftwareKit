/*******************************************************************************
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
 ******************************************************************************/

/*******************************************************************************
 *
 * Modifications Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 ******************************************************************************/

#ifndef SS_ADC_H_
#define SS_ADC_H_

#include "drivers/data_type.h"
#include "infra/device.h"
#include "drivers/clk_system.h"

/**
 * @defgroup arc_driver Sensor Subsystem Drivers
 * Sensor Subsystem (ARC) drivers API accessible only through the ARC processor.
 * @ingroup drivers
 */

/**
 * @defgroup adc_arc_driver ADC
 * ARC Analog/Digital Converter driver API.
 * @ingroup arc_driver
 * @{
 */

/**
 * SS ADC power management driver.
 */
extern struct driver ss_adc_driver;

typedef enum {
	SINGLED_ENDED = 0,
	DIFFERENTIAL = 1
} INPUT_MODE;

typedef enum {
	PARALLEL = 0,
	SERIAL = 1
} OUTPUT_MODE;

typedef enum {
	SINGLESHOT = 0,
	REPETITIVE = 1
}SEQ_MODE;


typedef enum {
	RISING_EDGE = 0,
	FALLING_EDGE = 1
} CAPTURE_MODE;

typedef enum {
	WIDTH_6_BIT = 0x0,
	WIDTH_8_BIT = 0x1,
	WIDTH_10_BIT = 0x2,
	WIDTH_12_BIT = 0x3
} SAMPLE_WIDTH;


/**
 *  callback function signature
 */
typedef void (*adc_callback)(uint32_t, void *);

/* Structure representing AD converter configuration */
typedef struct {
	INPUT_MODE	in_mode;        	/**< ADC input mode: single ended or differential */
	OUTPUT_MODE	out_mode;       	/**< ADC output mode: parallel or serial */
	uint8_t		serial_dly;     	/**< Number of adc_clk the first bit of serial output is delayed for after start of conversion */
	CAPTURE_MODE	capture_mode;   /**< ADC controller capture mode: by rising or falling edge of adc_clk */
	SAMPLE_WIDTH	sample_width;   /**< ADC sample width */
	SEQ_MODE	seq_mode;       	/**< ADC sequence mode - single run/repetitive */
	uint32_t	clock_ratio;    	/**< TODO */
	adc_callback	cb_rx;          /**< callback function for notification of data received and available to application, NULL if callback not required by application code */
	adc_callback	cb_err;         /**< callback function on transaction error, NULL if callback not required by application code */
	void *		priv;           	/**< this will be passed back by the callback routine - can be used to keep information needed in the callback */
} ss_adc_cfg_data_t;

/* simple macro to convert resolution to sample width to be used in the
 * configuration structure. It converts from 6,8,10,12 to 0x0,0x01,0x2,0x3
 */
#define ss_adc_res_to_sample_width(_res_) \
 	((SAMPLE_WIDTH)(((_res_ - 6) / 2) & 0x3))

#define ADC_VREF 3300 /* mV = 3.3V */
/* result in mV is given by converting raw data:
 * result = (data * ADC_VREF) / (2^resolution)
 */
#define ss_adc_data_to_mv(_data_, _resolution_)	\
	((_data_ * ADC_VREF) / (1 << _resolution_))


#define ADC_BUFS_NUM               (2)

/* Structure representing ADC sequence table entry. */
typedef struct {
	uint32_t	sample_dly;     /**< delay to be introduced prior to start of conversion, in terms of adc clocks */
	uint8_t		channel_id;     /**< ADC input associated with the entry */
} io_adc_seq_entry_t;


/* Structure representing ADC sequence table. */
typedef struct {
	io_adc_seq_entry_t *	entries;
	uint8_t			num_entries;
} io_adc_seq_table_t;


/** Function to configure ADC controller
 *
 *  Configuration parameters must be valid or an error is returned - see return values below.
 *
 *  @param  dev             : current device
 *  @param  config          : pointer to configuration structure
 *
 *  @return
 *           - true            on success
 *           - false           error setting configuration (device in use)
 */
bool ss_adc_set_config(struct device *dev, ss_adc_cfg_data_t *config);

/** Function to read samples from the ADC.
 *
 *  @param  dev              : current device
 *  @param  seq_tbl          : pointer to io_adc_seq_table
 *  @param  data             : buffer for sampling output
 *  @param  data_len         : size of data to be read
 *
 *  @return
 *           - DRV_RC_OK                       on success
 *           - DRV_RC_CONTROLLER_IN_USE,       if ADC is already sampling
 */
DRIVER_API_RC ss_adc_read(struct device *dev, io_adc_seq_table_t *seq_tbl,
			  uint32_t *data,
			  uint32_t data_len);

struct adc_info_t {
	uint32_t reg_base;       /* base address of device register set */
	uint32_t creg_slv;       /* address of creg slave control register */
	uint32_t creg_mst;       /* address of creg master control register */
	uint8_t state;
	uint8_t seq_mode;
	uint8_t index;
	uint32_t seq_size;
	uint32_t rx_len;
	uint32_t * rx_buf[ADC_BUFS_NUM];
	/* Interrupt numbers and handlers */
	uint8_t rx_vector; /* ISR vectors */
	uint8_t err_vector;
	uint16_t fifo_tld;
	uint16_t fifo_depth;
	/* SSS Interrupt Routing Mask Registers */
	uint32_t adc_irq_mask;
	uint32_t adc_err_mask;
	ss_adc_cfg_data_t cfg;
	bool adc_in_use; /* whether the controller is in use */
	struct pm_wakelock wakelock;
	struct clk_gate_info_s* clk_gate_info;
};

/** @} */

#endif  /* SS_ADC_H_ */
