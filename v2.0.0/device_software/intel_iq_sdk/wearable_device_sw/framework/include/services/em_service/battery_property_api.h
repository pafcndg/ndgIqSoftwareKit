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

#ifndef _BATT_PROP_API_H
#define _BATT_PROP_API_H


struct battprop_fuelgauge_t {
	bool is_charging;	/**< TRUE if device is charging else equal to FALSE*/
	int16_t temperature;	/**< current temperature */
};

struct battery_type_t {
/*
 * TODO
 * To be Defined
 */
};
/*
 * @enum batt_status_t
 * @brief battery error list
 */
typedef enum {
	BP_STATUS_SUCCESS = 0,
	BP_STATUS_ERROR_PARAMETER,
	BP_STATUS_ERROR_NOT_IMPLEMENTED,
	BP_STATUS_MEMORY_ERROR,
	BP_STATUS_WAITING_FOR_SERVICE,
	BP_STATUS_ERROR_LL_STORAGE_SERVICE,
	BP_STATUS_ERROR_PROP_SERVICE,
	BP_STATUS_ERROR	/* Generic Error */
} bp_status_t;

/*
 * @enum battprop_lookuptab_id_t
 * @brief Lookup table id
 */
typedef enum {
	BATTPROP_LOOKUPTAB_ID_CHARGE = 0,	/* Lookup table for charge at 25 degrees */
	BATTPROP_LOOKUPTAB_DISCHARGE_0 = 1,	/* Lookup table for discharge at 0 degrees */
	BATTPROP_LOOKUPTAB_DISCHARGE_12 = 2,	/* Lookup table for discharge at 12 degrees */
	BATTPROP_LOOKUPTAB_DISCHARGE_25 = 3,	/* Lookup table for discharge at 25 degrees */
	BATTPROP_LOOKUPTAB_ID_UNKNOWN
} battprop_lookuptab_id_t;

struct battery_properties_t
{
	uint32_t battery_id;
	struct battery_type_t battery_type;

	uint16_t battery_charge_cyles;
	uint32_t battery_overall_time_charging;
	uint32_t battery_overall_time_discharging;
	uint16_t battery_initial_capacity;
	uint16_t battery_terminate_voltage;
	uint16_t battery_charge_delta_soc;
};

typedef void (* battery_propeties_cb_t) (bp_status_t bp_status);

/*
 * @brief Get default battery lookup table
 * @param[in] battprop_lookuptab_id Current lookup table id
 * @param[in,out] lookuptable lookup table
 * @return BP_STATUS_SUCCESS if ok
 */
bp_status_t battprop_get_dflt_lookupTable(
						battprop_lookuptab_id_t battprop_lookuptab_id,
						uint16_t ** lookuptable);

/*
 * @brief Get battery lookup table
 * @param[in] battprop_fuelgauge current system state
 * @param[in,out] lookuptable lookup table
 * @return BP_STATUS_SUCCESS if ok
 */
bp_status_t battery_properties_get_lookupTable(struct battprop_fuelgauge_t
					   *battprop_fuelgauge,
					   uint16_t ** lookuptable);

/**
 * @brief Init battery properties
 * Read battery properties from the storage
 * @param battery_propeties_cb function to be call when properties are retrieved
 * @retval BP_STATUS_SUCCESS if success, error otherwise. List of errors to be specified
 * @remark shall call properties_service_add with BATTERY_SERVICE_ID and BATTERY_PROPERTY_ID
 */
bp_status_t battery_properties_init(battery_propeties_cb_t battery_propeties_cb);

/**
 * @brief Get battery properties
 * @param battery_properties battery_properties buffer where to write data
 * @retval BP_STATUS_SUCCESS if success, error otherwise. List of errors to be specified
 * @remark shall call properties_service_read with BATTERY_SERVICE_ID and BATTERY_PROPERTY_ID
 */

bp_status_t battery_properties_get( struct  battery_properties_t * battery_properties);

/**
 * @brief Save battery properties
 * @param battery_properties battery_properties where to read data
 * @retval BP_STATUS_SUCCESS if success, error otherwise. List of errors to be specified
 * @remark shall call properties_service_read with BATTERY_SERVICE_ID and BATTERY_PROPERTY_ID
 */
bp_status_t battery_properties_save( struct  battery_properties_t* battery_properties);
#endif
