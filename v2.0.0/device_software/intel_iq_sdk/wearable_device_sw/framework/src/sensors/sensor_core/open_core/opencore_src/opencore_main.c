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
#include "opencore_main.h"
#include "util/compiler.h"
#include "opencore_method.h"
static int poll_timeout = OS_WAIT_FOREVER;
static char stack[STACKSIZE] __aligned(4);
static uint8_t await_algo_idle_flag;
static char commit_buf[sizeof(struct ia_cmd) + sizeof(struct sensor_data) + COMMIT_DATA_MAX_LENGTH];
static uint8_t no_motion_flag;
static uint8_t calibration_flag;
extern uint8_t raw_data_calibration_flag;
extern uint8_t calibration_process_error;
extern uint8_t global_suspend_flag;
static uint32_t last_suspend_timestamp;

void OpencoreCommitSensData(uint8_t type, uint8_t id, void* data_ptr, int data_length)
{
	exposed_sensor_t* report_sensor = GetExposedStruct(type, id);
	if(report_sensor != NULL && (report_sensor->stat_flag & SUBSCRIBED) != 0
			&& data_ptr != NULL && data_length != 0){
		struct ia_cmd* cmd;
		struct sensor_data* data;
		memset(&commit_buf[0], 0, sizeof(commit_buf));
		cmd = (struct ia_cmd*)&commit_buf[0];
		cmd->cmd_id = SENSOR_DATA;
		cmd->length = sizeof(struct ia_cmd) + sizeof(struct sensor_data) + data_length;
		data = (struct sensor_data*)(cmd + 1);
		data->sensor.sensor_type = report_sensor->type;
		data->sensor.dev_id = report_sensor->id;
		data->timestamp = get_uptime_ms();
		data->data_length = data_length;
		if( 0 < data_length && data_length<= 60){
			memcpy(data->data, data_ptr, data_length);
			ipc_2svc_send(cmd);
		}
	}
}

static void HandleAlgo(feed_general_t* feed, void** data_ptr)
{
	int ret = feed->ctl_api.exec(data_ptr, feed);
	if(ret != 0){
		for(list_t* next = exposed_sensor_list.head ; next != NULL ; next = next->next){
			exposed_sensor_t* exposed_sensor = (exposed_sensor_t*)next;
			if(exposed_sensor->ready_flag != 0){
				OpencoreCommitSensData(exposed_sensor->type, exposed_sensor->id,
									exposed_sensor->rpt_data_buf, exposed_sensor->rpt_data_buf_len);
				exposed_sensor->ready_flag = 0;
			}
		}
	}
}

static void AddCaliData(uint8_t phy_type, sensor_handle_t* phy_sensor, void* ptr_from)
{
	switch(phy_type){
		case SENSOR_ACCELEROMETER:
			{
				short* cali_ptr = (short*)phy_sensor->clb_data_buffer;
				for(int k = 0 ; k < 3 ; k++)
					((short*)ptr_from)[k] += cali_ptr[k];
			}
			break;
		case SENSOR_GYROSCOPE:
		case SENSOR_MAGNETOMETER:
			{
				int* cali_ptr = (int*)phy_sensor->clb_data_buffer;
				for(int k = 0 ; k < 3 ; k++)
					((int*)ptr_from)[k] += cali_ptr[k];
			}
			break;
		default:
			break;
	}
}

static void FeedSensData2Algo(void)
{
	for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
		feed_general_t* feed = (feed_general_t*)next;
		if((feed->stat_flag & ON) != 0 && (feed->stat_flag & IDLE) == 0){
			sensor_data_demand_t* demand = feed->demand;
			uint8_t data_match_not_ready_flag = 0;
			uint8_t match_times = 0;
			uint16_t cm_time_consume = 1;

			for(int i = 0 ; i < feed->demand_length ; i++){

				if(demand[i].freq == 0)
					continue;

				sensor_handle_t* phy_sensor = GetActivePollSensStruct(demand[i].type, demand[i].id);
				if(phy_sensor != NULL){
					if(phy_sensor->buffer != NULL && phy_sensor->raw_sensor_data_count != 0){
						//gap and copy sensor data to delay buffer
						int gap = ValueRound((float)phy_sensor->freq / (demand[i].freq * 10));
						int frame_size = phy_sensor->sensor_data_frame_size;
						int count;
						void* ptr[feed->demand_length];
						memset(ptr, 0, sizeof(ptr));

						for(count = 0 ; gap * count + demand[i].raw_data_offset < phy_sensor->raw_sensor_data_count ; count++){
							int idx = gap * count + demand[i].raw_data_offset;

							if(feed->type != BASIC_ALGO_RAWDATA && feed->demand_length > 1 && demand[i].delay_buffer != NULL){
								memcpy(demand[i].delay_buffer + demand[i].put_idx * frame_size,
																	phy_sensor->buffer + idx * frame_size, frame_size);

								demand[i].put_idx++;
								demand[i].delay_data_count++;

								if(demand[i].put_idx == demand[i].delay_buffer_repo)
									demand[i].put_idx = 0;

								if(demand[i].put_idx == demand[i].get_idx){
									demand[i].get_idx++;
									if(demand[i].get_idx == demand[i].delay_buffer_repo)
										demand[i].get_idx = 0;
									demand[i].delay_data_count--;
								}
							}else{
								void* ptr_from = phy_sensor->feed_data_buffer;
								memcpy(ptr_from, phy_sensor->buffer + idx * frame_size, frame_size);
								//add cali offset
								AddCaliData(demand[i].type, phy_sensor, ptr_from);
								ptr[i] = ptr_from;
								if(feed->ctl_api.exec != NULL)
									HandleAlgo(feed, ptr);
							}
						}

						demand[i].raw_data_offset = gap - (phy_sensor->raw_sensor_data_count
													- (gap * (count - 1) + demand[i].raw_data_offset));
					}
				}
			}

			if(feed->type == BASIC_ALGO_RAWDATA || feed->demand_length == 1)
				continue;

			for(int i = 0 ; i < feed->demand_length ; i++)
				cm_time_consume = GetCommonMultiple(cm_time_consume, ValueRound((float)1000 / demand[i].freq));

			for(int i = 0 ; i < feed->demand_length ; i++){
				if(demand[i].delay_data_count < cm_time_consume / ValueRound((float)1000 / demand[i].freq)){
					data_match_not_ready_flag++;
				}else{
					int temp = demand[i].delay_data_count
										/ (cm_time_consume / ValueRound((float)1000 / demand[i].freq));
					if(match_times == 0 || temp < match_times)
						match_times = temp;
				}
			}

			if(data_match_not_ready_flag == 0){
				int vernier_length = 0;
				uint16_t cm_multi_freq = 1;
				int8_t scale[feed->demand_length];
				memset(scale, 0, sizeof(scale));

				for(int i = 0 ; i < feed->demand_length ; i++)
					cm_multi_freq = GetCommonMultiple(cm_multi_freq, demand[i].freq);

				for(int i = 0 ; i < feed->demand_length ; i++){
					int count;
					scale[i] = cm_multi_freq / demand[i].freq;
					count  = match_times * (cm_time_consume / ValueRound((float)1000 / demand[i].freq));

					if(vernier_length == 0 || vernier_length < count * scale[i])
						vernier_length = count * scale[i];
				}

				for(int v = 0 ; v < vernier_length ; v++){
					void* ptr[feed->demand_length];
					memset(ptr, 0, sizeof(ptr));
					int act = 0;
					for(int i = 0 ; i < feed->demand_length ; i++){
						sensor_handle_t* phy_sensor = GetActivePollSensStruct(demand[i].type, demand[i].id);
						if(phy_sensor != NULL && scale[i] != 0 && demand[i].delay_buffer != NULL){
							if(v % scale[i] == 0 && demand[i].get_idx != demand[i].put_idx){
								void* ptr_from = demand[i].delay_buffer
												+ phy_sensor->sensor_data_frame_size * demand[i].get_idx;
								//add the calibration offset value
								AddCaliData(demand[i].type, phy_sensor, ptr_from);
								ptr[i] = ptr_from;
								demand[i].get_idx++;
								demand[i].delay_data_count--;
								if(demand[i].get_idx >= demand[i].delay_buffer_repo)
									demand[i].get_idx = 0;
								act++;
							}
						}
					}

					if(act != 0 && feed->ctl_api.exec != NULL)
						HandleAlgo(feed, ptr);
				}
			}
		}
	}
}

static int GetSensData2Buf(void)
{
	int sum = 0;
	for(list_t* next = phy_sensor_poll_active_list.head ; next != NULL ; next = next->next){
		int ret = 0;
		sensor_handle_t* phy_sensor = (sensor_handle_t*)((void*)next
											- offsetof(sensor_handle_t, poll_active_link));
		phy_sensor->raw_sensor_data_count = 0;
		if(phy_sensor->fifo_length > 0 && phy_sensor->fifo_use_flag != 0 && phy_sensor->buffer != NULL){
			ret = phy_sensor_fifo_read(phy_sensor->ptr,
							(uint8_t*)phy_sensor->buffer, (uint16_t)phy_sensor->buffer_length);
			phy_sensor->raw_sensor_data_count = ret / phy_sensor->sensor_data_frame_size;
		}
		sum += ret;
	}
	return sum;
}

static int SensorCoreProcess(void)
{
	uint32_t ct, min_npp;
	int act1, act2;
	ct = get_uptime_ms();

again:
	act1 = act2 = min_npp = 0;
	for(list_t* next = phy_sensor_poll_active_list.head ; next != NULL ; next = next->next){
		sensor_handle_t* phy_sensor = (sensor_handle_t*)((void*)next
												- offsetof(sensor_handle_t, poll_active_link));

		if(phy_sensor->need_poll == 0 || (phy_sensor->stat_flag & IDLE) != 0)
			continue;

		phy_sensor->raw_sensor_data_count = 0;

		if(ct >= phy_sensor->npp && phy_sensor->npp != 0 && phy_sensor->buffer != NULL){
			int ret = 0;
			if(phy_sensor->fifo_length > 0 && phy_sensor->fifo_use_flag != 0){
				ret = phy_sensor_fifo_read(phy_sensor->ptr, (uint8_t*)phy_sensor->buffer,
																(uint16_t)phy_sensor->buffer_length);
			}else{
				void* temp_ptr = phy_sensor->buffer - offsetof(struct sensor_data, data);
				ret = phy_sensor_data_read(phy_sensor->ptr, (struct sensor_data*)temp_ptr);
			}
			phy_sensor->raw_sensor_data_count = ret / phy_sensor->sensor_data_frame_size;
			//updata npp
			phy_sensor->npp += phy_sensor->pi;

			act2++;
		}

		//updata the min_npp
		if(min_npp == 0 || min_npp > phy_sensor->npp)
			min_npp = phy_sensor->npp;

		act1++;
	}

	if(act1 == 0 || min_npp == 0)
		return FOREVER_VALUE;

	if(act2 != 0)
		FeedSensData2Algo();

	ct = get_uptime_ms();

	if(ct >= min_npp)
		goto again;

	return (min_npp - ct);
}

static struct ia_cmd* CoreSensorControl(struct sensor_id* sensor_id, core_sensor_ctl_t ctl)
{
	int act1, act2;
	int length = 0;
	int get_property_resp_param_length = 0;
	static uint8_t type_save;
	static int16_t id_save;
	static uint16_t exposed_stat_flag_save;
	static uint16_t feed_stat_flag_save;
	static sensor_data_demand_t* demand_array_save;
	struct ia_cmd* resp = NULL;
	struct return_value* rv;

	switch(ctl){
		case ALGO_CALIBRATION_STOP:
		case ALGO_CALIBRATION_SET:
		case ALGO_CALIBRATION_START:
			length = sizeof(struct ia_cmd) + sizeof(struct resp_calibration);
			break;
		case ALGO_CALIBRATION_PROCESS:
			{
				sensor_handle_t* phy_sensor = GetPollSensStruct(sensor_id->sensor_type, sensor_id->dev_id);
				if(phy_sensor != NULL)
					length = sizeof(struct ia_cmd) + sizeof(struct resp_calibration)
																+ phy_sensor->sensor_data_frame_size;
				else
					return NULL;
			}
			break;
		case ALGO_GET_PROPERTY:
			{
				uint8_t	property_type = ((struct get_property*)sensor_id)->property;
				/*according to property_type, define the length of  property_param of struct resp_get_property
					here, for example, assume the length is uyyint32_t
				*/
				switch(property_type){
					default:
						length = sizeof(struct ia_cmd) + sizeof(struct resp_calibration) + sizeof(uint32_t);
						get_property_resp_param_length = sizeof(uint32_t);
						break;
				}
			}
			break;
		default:
			length = sizeof(struct ia_cmd) + sizeof(struct return_value);
			break;
	}

	resp = (struct ia_cmd*)balloc(length, NULL);
	if(resp == NULL)
		return NULL;

	memset(resp, 0, length);
	resp->length = length;
	rv = (struct return_value*)resp->param;
	memcpy(&(rv->sensor), sensor_id, sizeof(struct sensor_id));
	rv->ret = RESPERROR_SUCCESS;

	if(ctl == ALGO_GET_PROPERTY)
		((struct resp_get_property*)rv)->length = get_property_resp_param_length;

	for(list_t* next = exposed_sensor_list.head ; next != NULL ; next = next->next){
		exposed_sensor_t* exposed_sensor = (exposed_sensor_t*)next;
		if(exposed_sensor->type == sensor_id->sensor_type && exposed_sensor->id == sensor_id->dev_id){
			switch(ctl){
				case ALGO_OPEN:
					{
#if 0
						if((exposed_sensor->state_flag & ON) == 0){
							exposed_sensor->state_flag |= ON;
							for(j = 0; j < feed_registered; j++)
								if(((1 << feed_list[j]->type)
									& exposed_sensor->depend_flag) != 0
									&& (feed_list[j]->state_flag & ON) == 0)
									feed_list[j]->state_flag |= ON;

							goto refresh;
						}
						goto out;
#endif
					}
				case ALGO_CLOSE:
					{
#if 0
						if((exposed_sensor->state_flag & ON) != 0){
							exposed_sensor->state_flag &= ~ON;
							for(j = 0; j < feed_registered; j++)
								if((1 << feed_list[j]->type
									& exposed_sensor->depend_flag) != 0
									&& (feed_list[j]->state_flag & ON) != 0)
									feed_list[j]->state_flag &= ~ON;
							goto refresh;
						}
						goto out;
#endif
					}
				case ALGO_SUBSCRIBE:
					{
						int	freq = ((struct subscription*)sensor_id)->sampling_interval;
						int	rt = ((struct subscription*)sensor_id)->reporting_interval;
						int rf_cnt_flag = 0;

						if((exposed_sensor->stat_flag & DIRECT_RAW) != 0){
							if(calibration_flag == 1 || rt <= 0 || freq <= 0 || (1000 / freq) > rt){
								rv->ret = RESPERROR_DEVICE_EXCEPTION;
								goto out;
							}
						}

						if((exposed_sensor->stat_flag & ON) != 0)
							rf_cnt_flag++;
						else
							exposed_sensor->stat_flag |= ON | SUBSCRIBED;

						act1 = act2 = 0;
						for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
							feed_general_t* feed = (feed_general_t*)next;
							if((1 << feed->type & exposed_sensor->depend_flag) != 0){
								if((exposed_sensor->stat_flag & DIRECT_RAW) != 0){
									for(int i = 0; i < feed->demand_length; i++)
										if(feed->demand[i].type == exposed_sensor->type &&
												feed->demand[i].id == exposed_sensor->id){
											feed->demand[i].freq = freq;
											feed->demand[i].rt = rt;
											if(feed->ctl_api.reset != NULL)
												feed->ctl_api.reset(feed);
											act1++;
									}
								}
								if((feed->stat_flag & ON) == 0){
									feed->stat_flag |= ON;
									if(global_suspend_flag == 1){
										sensor_handle_t* phy_sensor =
													GetIntSensStruct(SENSOR_MOTION_DETECTOR, DEFAULT_ID);
										if(phy_sensor != NULL){
											phy_sensor_enable(phy_sensor->ptr, 1);
											phy_sensor_data_register_callback(phy_sensor->ptr,
																		&motion_detect_callback, NULL, 0);
										}
										global_suspend_flag = 0;
									}
									if(feed->ctl_api.init != NULL)
										feed->ctl_api.init(feed);
									act1++;
								}
								if(rf_cnt_flag == 0)
									feed->rf_cnt++;
								act2++;
							}
						}
						if(act2 == 0)
							rv->ret = RESPERROR_DEVICE_EXCEPTION;

						if(act1 != 0)
							goto refresh;

						goto out;
					}
				case ALGO_UNSUBSCRIBE:
					{
						int rf_cnt_flag = 0;
						if((exposed_sensor->stat_flag & ON) != 0)
							exposed_sensor->stat_flag &= ~(SUBSCRIBED | ON);
						else
							rf_cnt_flag++;
						act1 = act2 = 0;
						for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
							feed_general_t* feed = (feed_general_t*)next;
							if((1 << feed->type & exposed_sensor->depend_flag) != 0){
								if((exposed_sensor->stat_flag & DIRECT_RAW) != 0){
									for(int i = 0 ; i < feed->demand_length ; i++)
										if(feed->demand[i].type == exposed_sensor->type &&
															feed->demand[i].id == exposed_sensor->id){
											feed->demand[i].freq = 0;
											feed->demand[i].rt = 0;
											if(feed->ctl_api.reset != NULL)
												feed->ctl_api.reset(feed);
											act1++;
										}
								}
								if(rf_cnt_flag == 0)
									feed->rf_cnt--;
								if(feed->rf_cnt == 0){
									feed->stat_flag &= ~ON;
									act1++;
								}
								act2++;
							}
						}
						if(act2 == 0)
							rv->ret = RESPERROR_DEVICE_EXCEPTION;

						if(act1 != 0)
							goto refresh;

						goto out;
					}
				case ALGO_GET_PROPERTY:
					{
						act1 = 0;
						for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
							feed_general_t* feed = (feed_general_t*)next;
							if((1 << feed->type & exposed_sensor->depend_flag) != 0){
								if(feed->ctl_api.get_property != NULL){
									uint8_t	property_type = ((struct get_property*)sensor_id)->property;
									struct resp_get_property* ptr_resp = (struct resp_get_property*)rv;
									feed->ctl_api.get_property(feed, sensor_id->sensor_type, sensor_id->dev_id,
																	property_type, (void*)ptr_resp->property_params);
								}
								act1++;
							}
						}

						if(act1 == 0)
							rv->ret = RESPERROR_DEVICE_EXCEPTION;

						goto out;
					}
				case ALGO_SET_PROPERTY:
					{
						act1 = 0;
						for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
							feed_general_t* feed = (feed_general_t*)next;
							if((1 << feed->type & exposed_sensor->depend_flag) != 0){
								if(feed->ctl_api.set_property != NULL){
									struct set_property* ptr = (struct set_property*)sensor_id;
									feed->ctl_api.set_property(feed, sensor_id->sensor_type, sensor_id->dev_id,
																	ptr->length, (void*)ptr->property_params);
								}
								act1++;
							}
						}

						if(act1 == 0)
							rv->ret = RESPERROR_DEVICE_EXCEPTION;

						goto out;
					}
					break;
				case ALGO_CALIBRATION_SET:
					{
						act1 = 0;
						if(calibration_flag == 1)
							break;
						switch(exposed_sensor->type){
							case SENSOR_ACCELEROMETER:
								{
									sensor_handle_t* phy_sensor =
												GetPollSensStruct(exposed_sensor->type, exposed_sensor->id);
									if(phy_sensor != NULL){
										short* cali_value = (short*)(((struct calibration*)sensor_id)->calib_params);
										short* ptr = (short*)phy_sensor->clb_data_buffer;
										if(ptr == NULL)
											break;
										for(int i = 0 ; i < 3 ; i++)
											ptr[i] = cali_value[i];
										act1++;
									}
								}
								break;
							case SENSOR_GYROSCOPE:
								{
									sensor_handle_t* phy_sensor =
												GetPollSensStruct(exposed_sensor->type, exposed_sensor->id);
									if(phy_sensor != NULL){
										int* cali_value = (int*)(((struct calibration*)sensor_id)->calib_params);
										int* ptr = (int*)phy_sensor->clb_data_buffer;
										if(ptr == NULL)
											break;
										for(int i = 0 ; i < 3 ; i++)
											ptr[i] = cali_value[i];
										act1++;
									}
								}
								break;
							default:
								break;
						}
						if(act1 != 0)
							goto out;
					}
					break;
				case ALGO_CALIBRATION_START:
					{
						act1 = 0;
						if(calibration_flag == 1)
							break;

						if(exposed_sensor->type != SENSOR_ACCELEROMETER && exposed_sensor->type != SENSOR_GYROSCOPE)
							break;

						for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
							feed_general_t* feed = (feed_general_t*)next;
							if((1 << feed->type & exposed_sensor->depend_flag) != 0){
								demand_array_save = balloc(sizeof(sensor_data_demand_t) * feed->demand_length, NULL);
								if(demand_array_save == NULL)
									break;

								memcpy(demand_array_save, feed->demand, feed->demand_length * sizeof(sensor_data_demand_t));
								for(int i = 0; i < feed->demand_length; i++){
									feed->demand[i].freq = feed->demand[i].rt = 0;
									if(feed->demand[i].type ==	exposed_sensor->type && feed->demand[i].id == exposed_sensor->id){
										feed->demand[i].freq = 10;
										feed->demand[i].rt = 1000;
										if(feed->ctl_api.reset != NULL)
											feed->ctl_api.reset(feed);
									}
								}
								type_save = exposed_sensor->type;
								id_save = exposed_sensor->id;
								calibration_flag = 1;
								pm_wakelock_acquire(&opencore_cali_wl);
								exposed_stat_flag_save = exposed_sensor->stat_flag;
								exposed_sensor->stat_flag |= ON;
								feed_stat_flag_save = feed->stat_flag;
								feed->stat_flag &= ~IDLE;
								feed->stat_flag |= ON;
								feed->idle_hold_flag = 1;

								act1++;
							}
						}
						if(act1 != 0)
							goto refresh;
					}
					break;
				case ALGO_CALIBRATION_PROCESS:
					{
						if(calibration_flag == 0 || no_motion_flag != 1)
							break;
						if(exposed_sensor->type != type_save || exposed_sensor->id != id_save)
							break;
						if(raw_data_calibration_flag == 1)
							break;
						raw_data_calibration_flag = 1;
						goto out;
					}
				case ALGO_CALIBRATION_STOP:
					{
						act1 = 0;
						if(calibration_flag == 0)
							break;
						if(exposed_sensor->type != type_save || exposed_sensor->id != id_save)
							break;
						if(raw_data_calibration_flag == 1)
							break;
						for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
							feed_general_t* feed = (feed_general_t*)next;
							if((1 << feed->type & exposed_sensor->depend_flag) != 0){
								feed->stat_flag = feed_stat_flag_save;
								if(no_motion_flag == 1){
									struct ia_cmd* cmd = (struct ia_cmd*)balloc(sizeof(struct ia_cmd), NULL);
									if(cmd != NULL){
										memset(cmd, 0, sizeof(struct ia_cmd));
										cmd->cmd_id = CMD_SUSPEND_SC;
										ipc_2core_send(cmd);
									}
								}else
									feed->stat_flag &= ~IDLE;

								feed->idle_hold_flag = 0;
								if(demand_array_save != NULL){
									memcpy(feed->demand, demand_array_save,
														feed->demand_length * sizeof(sensor_data_demand_t));
									bfree(demand_array_save);
									demand_array_save = NULL;
								}

								if(feed->ctl_api.reset != NULL)
									feed->ctl_api.reset(feed);
								act1++;
							}
						}
						exposed_sensor->stat_flag = exposed_stat_flag_save;
						calibration_flag = 0;
						if(act1 != 0)
							goto refresh;
					}
					break;
				default:
					break;
			}
		}
	}
	rv->ret = RESPERROR_NO_DEVICE;
	return resp;
refresh:
	RefleshSensorCore();
out:
	return resp;
}

static struct ia_cmd *GetCoreSensorList(uint32_t bitmap)
{
	int count = 0, index = 0;

	for(list_t* next = exposed_sensor_list.head ; next != NULL ; next = next->next){
		exposed_sensor_t* exposed_sensor = (exposed_sensor_t*)next;
		if((bitmap & (1 << exposed_sensor->type)) != 0)
			count++;
	}

	int len = sizeof(struct ia_cmd) + sizeof(struct sensor_list) + sizeof(struct sensor_id) * count;

	struct ia_cmd *resp = (struct ia_cmd*)balloc(len, NULL);
	if(resp == NULL)
		return NULL;

	memset(resp, 0, len);
	resp->length = len;
	struct sensor_list* list = (struct sensor_list*)resp->param;
	list->count = count;

	for(list_t* next = exposed_sensor_list.head ; next != NULL ; next = next->next){
		exposed_sensor_t* exposed_sensor = (exposed_sensor_t*)next;
		if((bitmap & (1 << exposed_sensor->type)) != 0){
			list->sensor_list[index].sensor_type = exposed_sensor->type;
			list->sensor_list[index].dev_id = exposed_sensor->id;
			index++;
		}
	}

	return resp;
}

static void PrepareSWtappingDetect(void)
{
	//check if tapping algo is on
	//if on, check if accel is fifo enable
	//if fifo disable, balloc buffer to accel sensor and enable accel fifo
	for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
		feed_general_t* feed = (feed_general_t*)next;
		if(feed->type == BASIC_ALGO_TAPPING && (feed->stat_flag & ON) != 0){
			for(int i = 0; i < feed->demand_length; i++){
				sensor_data_demand_t* demand = &(feed->demand[i]);
				if(demand->type == SENSOR_ACCELEROMETER){
					sensor_handle_t* phy_sensor = GetPollSensStruct(demand->type, demand->id);
					if(phy_sensor != NULL && phy_sensor->fifo_length > 0 && phy_sensor->fifo_use_flag == 0){
						if(phy_sensor->buffer_length != BUFFER_LIMIT_SIZE){
							if(phy_sensor->buffer != NULL){
								void* temp_ptr = phy_sensor->buffer - offsetof(struct sensor_data, data);
								bfree(temp_ptr);
								phy_sensor->buffer = NULL;
								phy_sensor->buffer_length = 0;
							}

							phy_sensor->buffer = balloc(BUFFER_LIMIT_SIZE, NULL);
							if(phy_sensor->buffer == NULL)
								break;

							memset(phy_sensor->buffer, 0, BUFFER_LIMIT_SIZE);
							phy_sensor->buffer_length = BUFFER_LIMIT_SIZE;
							phy_sensor_enable_hwfifo_with_buffer(phy_sensor->ptr, 1,
													(uint8_t *)phy_sensor->buffer, phy_sensor->buffer_length);
							phy_sensor->fifo_use_flag++;
						}
					}
					break;
				}
			}
			break;
		}
	}
}


int GetTargetFifoLength(void)
{
	int fifo_target_len = 0;
	for(list_t* next = phy_sensor_poll_active_list.head ; next != NULL ; next = next->next){
		sensor_handle_t* phy_sensor = (sensor_handle_t*)((void*)next - offsetof(sensor_handle_t, poll_active_link));
		if(phy_sensor->fifo_length > 0 && phy_sensor->fifo_use_flag != 0)
			phy_sensor->fifo_share_done_flag = 0;
	}

	for(list_t* next = phy_sensor_poll_active_list.head ; next != NULL ; next = next->next){
		sensor_handle_t* phy_sensor = (sensor_handle_t*)((void*)next - offsetof(sensor_handle_t, poll_active_link));
		if(phy_sensor->fifo_length > 0 && phy_sensor->fifo_use_flag != 0 && phy_sensor->fifo_share_done_flag == 0){
			//campare to last suspend timestamp
			uint32_t mode_change_interval = get_uptime_ms() - last_suspend_timestamp;
			int count = 0;

			if(phy_sensor->fifo_share_bitmap != 0){
				list_t* share_list_head = &phy_sensor->fifo_share_link;
				do{
					sensor_handle_t* phy_sensor_node = (sensor_handle_t*)((void*)share_list_head
														- offsetof(sensor_handle_t, fifo_share_link));
					if((phy_sensor_node->stat_flag & ON) != 0){
						int si = ValueRound((float)1000 * 10 / phy_sensor_node->freq);
						count += mode_change_interval / si;
						phy_sensor_node->fifo_share_done_flag++;
					}
					share_list_head = share_list_head->next;
				}while(share_list_head != &phy_sensor->fifo_share_link);
			}else{
				int si = ValueRound((float)1000 * 10 / phy_sensor->freq);
				count += mode_change_interval / si;
			}

			if(count * RAW_FRAME_LENGTH >= phy_sensor->fifo_length)
				fifo_target_len += phy_sensor->fifo_length * VALID_FRAME_LENGTH / RAW_FRAME_LENGTH;
			else
				fifo_target_len += count * VALID_FRAME_LENGTH;
		}
	}

	return fifo_target_len;
}

#ifdef SUSPEND_TEST
static uint8_t loop_flag;
#endif

static void SuspendJudge(void)
{
	await_algo_idle_flag = 0;
	for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
		feed_general_t* feed = (feed_general_t*)next;
		if((feed->stat_flag & IDLE) == 0){
			if(feed->idle_hold_flag == 0){
				feed->stat_flag |= IDLE;
				if((feed->stat_flag & ON) != 0){
					if(feed->ctl_api.goto_idle != NULL)
						feed->ctl_api.goto_idle(feed);
					feed->mark_flag |= IDLE;
					//phy_sensor ref--
					for(int i = 0; i < feed->demand_length; i++){
						sensor_data_demand_t* demand = &(feed->demand[i]);
						if(demand->freq != 0){
							sensor_handle_t* phy_sensor = GetPollSensStruct(demand->type, demand->id);
							if(phy_sensor != NULL){
								phy_sensor->idle_ref--;
								if(phy_sensor->idle_ref == 0){
									// phy_sensor IDLE
									phy_sensor->stat_flag |= IDLE;

#ifdef SUPPORT_INTERRUPT_MODE
									if(phy_sensor->fifo_length > 0 && phy_sensor->fifo_use_flag != 0){
										//if support fifo int, register cb, need_poll = 0
										if((phy_sensor->attri_mask & PHY_SENSOR_REPORT_MODE_INT_FIFO_MASK) != 0){
												phy_sensor_watermark_property_t temp_prop = {
															.count = 0,
															.callback = NULL,
															.priv_data = NULL,
															};
												phy_sensor_set_property(phy_sensor->ptr, SENSOR_PROP_FIFO_WATERMARK, &temp_prop);
										}
									}else{
										phy_sensor_enable(phy_sensor->ptr, 0);
										//if support reg int, register cb, need_poll = 0
										if((phy_sensor->attri_mask & PHY_SENSOR_REPORT_MODE_INT_REG_MASK) != 0)
											phy_sensor_data_unregister_callback(phy_sensor->ptr);
									}
#endif
								}
							}
						}
					}
				}
			}else
				await_algo_idle_flag++;
		}
	}

	if(await_algo_idle_flag == 0){
#ifdef SUSPEND_TEST
		loop_flag = 1;
#endif
		PrepareSWtappingDetect();

		last_suspend_timestamp = get_uptime_ms();
	}
}

static void ResumeJudge(void)
{
	for(list_t* next = feed_list.head ; next != NULL ; next = next->next){
		feed_general_t* feed = (feed_general_t*)next;
		if((feed->stat_flag & IDLE) != 0){
			feed->stat_flag &= ~IDLE;
			if(feed->ctl_api.out_idle != NULL && (feed->mark_flag & IDLE) != 0){
				feed->ctl_api.out_idle(feed);
				feed->mark_flag &= ~IDLE;
			}

			if((feed->stat_flag & ON) != 0){
				for(int i = 0; i < feed->demand_length; i++){
					sensor_data_demand_t* demand = &(feed->demand[i]);
					if(demand->freq != 0){
						sensor_handle_t* phy_sensor = GetPollSensStruct(demand->type, demand->id);
						if(phy_sensor != NULL){
							phy_sensor->idle_ref++;
							phy_sensor->stat_flag &= ~IDLE;
						}
					}
				}
			}
		}
	}

	await_algo_idle_flag = 0;

	int fifo_target_len = GetTargetFifoLength();
	int sum = 0;
//	uint32_t ct = get_uptime_ms();
	while(1){
		sum += GetSensData2Buf();
		FeedSensData2Algo();
		if(sum >= fifo_target_len)
			break;
	}

//	pr_info(LOG_MODULE_OPEN_CORE, "spin timeconsume=%d, targen_len=%d", get_uptime_ms() - ct, fifo_target_len);

	RefleshSensorCore();
}

static struct ia_cmd* ParseCmd(struct ia_cmd* inbound)
{
	struct ia_cmd *resp = NULL;
	static struct ia_cmd* resp_save;
	switch(inbound->cmd_id){
		case CMD_GET_SENSOR_LIST:
			{
				struct sensor_type_bit_map *bit_ptr =
							(struct sensor_type_bit_map*)(inbound->param);
				uint32_t bitmap = bit_ptr->bit_map;
				resp = GetCoreSensorList(bitmap);
			}
			break;
		case CMD_START_SENSOR:
			{
				struct sensor_id* param = (struct sensor_id*)inbound->param;
				resp = CoreSensorControl(param, ALGO_OPEN);
			}
			break;
		case CMD_STOP_SENSOR:
			{
				struct sensor_id* param = (struct sensor_id*)inbound->param;
				resp = CoreSensorControl(param, ALGO_CLOSE);
			}
			break;
		case CMD_SUBSCRIBE_SENSOR_DATA:
			{
				struct subscription* param = (struct subscription*)inbound->param;
				resp = CoreSensorControl((struct sensor_id*)param, ALGO_SUBSCRIBE);
			}
			break;
		case CMD_UNSUBSCRIBE_SENSOR_DATA:
			{
				struct subscription* param = (struct subscription*)inbound->param;
				resp = CoreSensorControl((struct sensor_id*)param, ALGO_UNSUBSCRIBE);
			}
			break;
		case CMD_GET_PROPERTY:
			{
				struct get_property* param = (struct get_property*)inbound->param;
				resp = CoreSensorControl((struct sensor_id*)param, ALGO_GET_PROPERTY);
			}
			break;
		case CMD_SET_PROPERTY:
			{
				struct set_property* param = (struct set_property*)inbound->param;
				resp = CoreSensorControl((struct sensor_id*)param, ALGO_SET_PROPERTY);
			}
			break;
		case CMD_CALIBRATION:
			{
				struct calibration* cali_param = (struct calibration*)inbound->param;
				switch(cali_param->clb_cmd){
					case START_CALIBRATION_CMD:
						{
							//check calibration_flag whether is 1, if 1 return error.
							//only support both a sensor and g sensor.
							//calibration_flag = 1.
							//pm_wakelock_acquire.
							//save exposed sensor stat , feed stat and feed demand array.
							//stop all the feeds but calibration target sensor.
							struct sensor_id* sensor_id = (struct sensor_id*)cali_param;
							resp = CoreSensorControl(sensor_id, ALGO_CALIBRATION_START);
							if(resp == NULL)
								break;
							struct resp_calibration* resp_param = (struct resp_calibration*)resp->param;
							resp_param->clb_cmd = cali_param->clb_cmd;
							resp_param->length = 0;
						}
						break;
					case GET_CALIBRATION_CMD:
						{
							//check calibration_flag whether 0. if 0 return error
							//check no_motion_flag whether is 1, if 0 return error.
							//raw_data_calibration_flag = 1;
							//process target sensor calibration and return the result.
							struct sensor_id* sensor_id = (struct sensor_id*)cali_param;
							resp = CoreSensorControl(sensor_id, ALGO_CALIBRATION_PROCESS);
							if(resp == NULL)
								break;
							struct resp_calibration* resp_param = (struct resp_calibration*)resp->param;
							resp_param->clb_cmd = cali_param->clb_cmd;
							resp_param->calibration_type = sensor_id->sensor_type;
							struct return_value* rv = (struct return_value*)resp->param;
							if(rv->ret == RESPERROR_SUCCESS){
								sensor_handle_t* phy_sensor =
													GetPollSensStruct(sensor_id->sensor_type, sensor_id->dev_id);
								if(phy_sensor != NULL)
									resp_param->length = phy_sensor->sensor_data_frame_size;
								else
									resp_param->length = 0;

								resp->cmd_id = inbound->cmd_id;
								resp->tran_id = inbound->tran_id;
								resp_save = resp;
								resp = NULL;
							}else
								resp_param->length = 0;
						}
						break;
					case STOP_CALIBRATION_CMD:
						{
							//check calibration_flag whether 0. if 0 return error
							//check raw_data_calibration_flag whether is 1 , if i return error
							//pm_wakelock_release
							//resume all the feeds previous status
							//restore exposed sensor stat, feed stat and feed demand array.
							//calibration_flag = 0;
							struct sensor_id* sensor_id = (struct sensor_id*)cali_param;
							resp = CoreSensorControl(sensor_id, ALGO_CALIBRATION_STOP);
							if(resp == NULL)
								break;

							struct resp_calibration* resp_param = (struct resp_calibration*)resp->param;
							resp_param->clb_cmd = cali_param->clb_cmd;
							resp_param->length = 0;
							pm_wakelock_release(&opencore_cali_wl);
						}
						break;
					case REBOOT_AUTO_CALIBRATION_CMD:
					case SET_CALIBRATION_CMD:
						{
							struct sensor_id* sensor_id = (struct sensor_id*)cali_param;
							resp = CoreSensorControl(sensor_id, ALGO_CALIBRATION_SET);
							if(resp == NULL)
								break;

							struct resp_calibration* resp_param = (struct resp_calibration*)resp->param;
							resp_param->clb_cmd = cali_param->clb_cmd;
							resp_param->length = 0;
						}
						break;
					default:
						break;
				}
			}
			break;
		case CMD_CALIBRATION_PROCESS_RESULT:
			{
				//check inbound->tran_id whether 0, if 0 calibration is succed otherwise fail.
				struct resp_calibration* param_to = (struct resp_calibration*)resp_save->param;
				if(inbound->tran_id == 0 && param_to->length > 0){
					memcpy(param_to->calib_params, inbound->param, inbound->length);
				}else if(inbound->tran_id == 1){
					struct return_value* rv = (struct return_value*)param_to;
					rv->ret = RESPERROR_DEVICE_EXCEPTION;
				}
				resp = resp_save;
				goto cali_get_data_out;
			}
		case CMD_SUSPEND_SC:
			{
				no_motion_flag = 1;
				SuspendJudge();
			}
			break;
		case CMD_RESUME_SC:
			{
				no_motion_flag = 0;
				if(raw_data_calibration_flag == 1)
					calibration_process_error = 1;
				ResumeJudge();
			}
			break;
#ifdef SUPPORT_INTERRUPT_MODE
		case CMD_RAWDATA_REG_INT_SC:
			{
				struct sensor_data* sensor_data = (struct sensor_data*)inbound->param;
				sensor_handle_t* phy_sensor = GetPollSensStruct(sensor_data->sensor.sensor_type, sensor_data->sensor.dev_id);
				if(phy_sensor != NULL && (phy_sensor->stat_flag & IDLE) == 0){
					memcpy(phy_sensor->buffer, sensor_data->data, sensor_data->data_length);
					phy_sensor->raw_sensor_data_count = 1;
					FeedSensData2Algo();
				}
			}
			break;
		case CMD_RAWDATA_FIFO_INT_SC:
			{
				sensor_handle_t* phy_sensor;
				memcpy(&phy_sensor, inbound->param, inbound->length);

				if(phy_sensor != NULL && (phy_sensor->stat_flag & IDLE) == 0){
					list_t* share_list_head = &phy_sensor->fifo_share_link;
					do{
						sensor_handle_t* phy_sensor_node = (sensor_handle_t*)((void*)share_list_head
																- offsetof(sensor_handle_t, fifo_share_link));
						if((phy_sensor_node->stat_flag & ON) != 0){
							phy_sensor_node->raw_sensor_data_count = 0;
							int ret = phy_sensor_fifo_read(phy_sensor_node->ptr,
										(uint8_t*)phy_sensor_node->buffer, (uint16_t)phy_sensor_node->buffer_length);
							phy_sensor_node->raw_sensor_data_count = ret / phy_sensor_node->sensor_data_frame_size;
						}
						share_list_head = share_list_head->next;
					}while(share_list_head != &phy_sensor->fifo_share_link);

					FeedSensData2Algo();
				}
			}
			break;
#endif
		default:
			break;
	}

	if(resp != NULL){
		resp->cmd_id = inbound->cmd_id;
		resp->tran_id = inbound->tran_id;
	}

cali_get_data_out:
	bfree(inbound);
	return resp;
}

static void opencore_fiber(void)
{
	struct ia_cmd *cmd;
	struct ia_cmd *resp;
	int ret;

	while(1){
#ifdef SUSPEND_TEST
		if(loop_flag == 1){
			while(1){
				ipc_core_receive(&cmd, 500);
				uint8_t pmu_status = 0xff;
				bmi160_read_reg(0x03, &pmu_status);
				pr_info(LOG_MODULE_OPEN_CORE, ">>>>>>>>>>>>>pms=%d", pmu_status);
			}
		}
#endif
		ret = ipc_core_receive(&cmd, poll_timeout);

		pm_wakelock_acquire(&opencore_main_wl);

		if(ret == 0){
			resp = ParseCmd(cmd);
			if(resp != NULL){
				ipc_2svc_send(resp);
				bfree(resp);
			}
		}

		poll_timeout = SensorCoreProcess();

		if(await_algo_idle_flag != 0)
			SuspendJudge();

		pm_wakelock_release(&opencore_main_wl);
	}
}

int sensor_core_create()
{
	ipc_svc_core_create();
	SensorCoreInit();
	task_fiber_start(&stack[0], STACKSIZE,
								(nano_fiber_entry_t)opencore_fiber, 0, 0, 7, 0);
	return 0;
}
