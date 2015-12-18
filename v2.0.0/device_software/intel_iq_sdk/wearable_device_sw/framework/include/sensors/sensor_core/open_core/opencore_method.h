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

static inline sensor_handle_t* GetIntSensStruct(uint8_t type, uint8_t id)
{
	for(list_t* next = phy_sensor_list_int.head ; next != NULL ; next = next->next){
		sensor_handle_t* phy_sensor = (sensor_handle_t*)((void*)next - offsetof(sensor_handle_t, int_link));
		if((id == DEFAULT_ID && phy_sensor->type == type)
				 || (phy_sensor->type == type && phy_sensor->id == id))
			return phy_sensor;
	}
	return NULL;
}

static inline feed_general_t* GetFeedStruct(basic_algo_type_t type)
{
	for(list_t* next = feed_list.head ; next != NULL ; next = next->next)
		if(((feed_general_t*)next)->type == type)
			return (feed_general_t*)next;
	return NULL;
}

static inline uint16_t GetSensSamplingTime(sensor_handle_t* phy_sensor)
{
	uint16_t odr = 0;
	if(phy_sensor != NULL)
		phy_sensor_get_odr_value(phy_sensor, &odr);
	return odr;
}


static inline uint16_t SetSensSamplingTime(sensor_handle_t* phy_sensor, uint16_t odr_x10)
{
	int ret = 0;
	if (phy_sensor != NULL){
		ret = phy_sensor_set_odr_value(phy_sensor->ptr, odr_x10);
		phy_sensor->freq = odr_x10;
	}
	return ret;
}

static inline uint16_t GetCommonDivisor(uint16_t next_freq, uint16_t prev_multiple)
{
	uint16_t temp;
	if(next_freq < prev_multiple){
		temp = next_freq;
		next_freq = prev_multiple;
		prev_multiple = temp;
	}
	temp = next_freq % prev_multiple;
	while(temp != 0){
		next_freq = prev_multiple;
		prev_multiple = temp;
		temp = next_freq % prev_multiple;
	}
	return prev_multiple;
}

static inline uint16_t GetCommonMultiple(uint16_t next_freq, uint16_t prev_multiple)
{
	return next_freq * prev_multiple / GetCommonDivisor(next_freq,prev_multiple);
}

static inline int ValueRound(float value)
{
	int rslt = value;
	int temp = ((int)(value * 10)) % 10;
	if(temp >= 5)
		rslt++;
	return rslt;
}




