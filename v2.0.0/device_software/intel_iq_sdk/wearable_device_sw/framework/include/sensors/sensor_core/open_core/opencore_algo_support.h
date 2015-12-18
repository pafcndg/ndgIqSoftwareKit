/****************************************************************************************
 *
 * BSD LICENSE
 *
 * Copyright(c) 2015 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************************/

/**
 * @defgroup sensor_core_open  Thunderdome Open Sensor Core API
 * Open core exposed api/structure
 * @ingroup SS_SERVICE
 * @{
 */
#include "opencore_algo_common.h"
#include "util/compiler.h"
extern list_head_t exposed_sensor_list;
extern list_head_t phy_sensor_poll_active_array[PHY_TYPE_KEY_LENGTH * PHY_ID_KEY_LENGTH];
extern list_head_t phy_sensor_list_poll;
/**
 * a macro used to assign the pointer of feed struct defined
 * by user into feed section.
 **/
#define define_feedinit(name) \
    static void* __feedinit_##name __used \
        __section(".openinit.feed") = &name
extern char _s_feedinit[], _e_feedinit[];


/**
 * a macro used to assign the pointer of exposed sensor struct
 * defined by user into exposed section.
 **/
#define define_exposedinit(name) \
    static void* __exposedinit_##name __used \
        __section(".openinit.exposed") = &name
extern char _s_exposedinit[], _e_exposedinit[];


/**
 * a macro used to hold into sleep by basis algo.
 **/
#define HOLD_IDLE(feed) do{	\
	feed->idle_hold_flag = 1; 	\
}while(0);

/**
 * a macro used to be ready into sleep by basis algo.
 **/
#define FREE_IDLE(feed) do{ \
	feed->idle_hold_flag = 0; \
}while(0);

/**
 *  @brief  Get the index of physical sensor type in the demand array of feed.
  * @param[in]  type : the physical sensor type
  * @return the demand index in feed that defined by user.
  */
static inline int GetDemandIdx(feed_general_t* feed, ss_sensor_type_t type)
{
	int i;
	for(i = 0 ; i < feed->demand_length ; i++)
		if(feed->demand[i].type == type)
			return i;
	return -1;
}

/**
 *  @brief  Get the pointer of exposed sensor struct defined by user.
  * @param[in]  type : the exposed sensor type
  * @return a pointer of exposed sensor struct.
  */
static inline exposed_sensor_t* GetExposedStruct(uint8_t type, uint8_t id)
{
	for(list_t* next = exposed_sensor_list.head ; next != NULL ; next = next->next){
		exposed_sensor_t* exposed_sensor = (exposed_sensor_t*)next;
		if(exposed_sensor->type == type && exposed_sensor->id == id)
			return exposed_sensor;
	}
	return NULL;
}

static inline int GetHashKey(uint8_t phy_sensor_type, uint8_t dev_id)
{
	return (phy_sensor_type % PHY_TYPE_KEY_LENGTH) * PHY_ID_KEY_LENGTH
				+ dev_id % PHY_ID_KEY_LENGTH;
}

static inline sensor_handle_t* GetActivePollSensStruct(uint8_t type, uint8_t id)
{
	int idx = GetHashKey(type, id);
	for(list_t* next = phy_sensor_poll_active_array[idx].head ; next != NULL ; next = next->next){
		sensor_handle_t* phy_sensor = (sensor_handle_t*)((void*)next - offsetof(sensor_handle_t, poll_active_array_link));
		if(phy_sensor->type == type && phy_sensor->id == id)
			return phy_sensor;
	}
	return NULL;
}

static inline sensor_handle_t* GetPollSensStruct(uint8_t type, uint8_t id)
{
	for(list_t* next = phy_sensor_list_poll.head ; next != NULL ; next = next->next){
		sensor_handle_t* phy_sensor = (sensor_handle_t*)((void*)next - offsetof(sensor_handle_t, poll_link));
		if(phy_sensor->type == type && phy_sensor->id == id)
			return phy_sensor;
	}
	return NULL;
}
static inline int GetSensorDataFrameSize(uint8_t type, uint8_t id)
{
	sensor_handle_t* phy_sensor = GetActivePollSensStruct(type, id);
	if(phy_sensor != NULL){
		return phy_sensor->sensor_data_frame_size;
	}
	return -1;
}
/** @} */
