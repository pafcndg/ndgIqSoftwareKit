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
#include "opencore_algo_support.h"
#define RAW_SENSOR_DATA_COUNT 5
uint8_t raw_data_calibration_flag = 0;
uint8_t calibration_process_error = 0;
static int sensor_rawdata_prepare_exec(void** sensor_data, feed_general_t* feed)
{
	int ret = 0;
	for(int i = 0 ; i < feed->demand_length ; i++){

		void* data = sensor_data[i];
		exposed_sensor_t* exposed_sensor = GetExposedStruct(feed->demand[i].type, feed->demand[i].id);
		sensor_handle_t* phy_sensor = GetActivePollSensStruct(feed->demand[i].type, feed->demand[i].id);
		if(phy_sensor != NULL && data != NULL && exposed_sensor != NULL){
			int cnt = exposed_sensor->data_frame_count;
			memcpy(exposed_sensor->rpt_data_buf + cnt * phy_sensor->sensor_data_frame_size, data, phy_sensor->sensor_data_frame_size);
			exposed_sensor->data_frame_count++;

			if(exposed_sensor->data_frame_count * phy_sensor->sensor_data_frame_size == exposed_sensor->rpt_data_buf_len){
				exposed_sensor->data_frame_count = 0;
				exposed_sensor->ready_flag = 1;
				ret++;
			}

			if(raw_data_calibration_flag == 1){
				static int calibration_count;
				static int buffer[3];
				if(feed->demand[i].type == SENSOR_ACCELEROMETER){
					struct accel_phy_data* data_a = (struct accel_phy_data*)data;
					buffer[0] += data_a->ax;
					buffer[1] += data_a->ay;
					buffer[2] += data_a->az;
					calibration_count++;
				}else if(feed->demand[i].type == SENSOR_GYROSCOPE){
					struct gyro_phy_data* data_g = (struct gyro_phy_data*)data;
					buffer[0] += data_g->gx;
					buffer[1] += data_g->gy;
					buffer[2] += data_g->gz;
					calibration_count++;
				}

				if(calibration_count == 100){
					//send msg queue;
					int length = sizeof(struct ia_cmd) + phy_sensor->sensor_data_frame_size;
					struct ia_cmd* cmd = (struct ia_cmd*)balloc(length, NULL);
					if(cmd == NULL)
						continue;

					memset(cmd, 0, length);
					cmd->cmd_id = CMD_CALIBRATION_PROCESS_RESULT;
					if(feed->demand[i].type == SENSOR_ACCELEROMETER){
						int j, max_j = 0;
						int max_value = 0;
						buffer[0] = buffer[0] / 100;
						buffer[1] = buffer[1] / 100;
						buffer[2] = buffer[2] / 100;
						for(j = 0 ; j < 3 ; j++){
							short abs = buffer[j] >= 0 ? buffer[j] : -buffer[j];
							if(abs > max_value){
								max_value = abs;
								max_j = j;
							}
						}
						if(phy_sensor != NULL && max_value > 900 && max_value < 1100 && calibration_process_error == 0){
							cmd->tran_id = 0;
							cmd->length = phy_sensor->sensor_data_frame_size;
							short* ptr = (short*)phy_sensor->clb_data_buffer;
							ptr[max_j] += 1000 - (short)max_value;
							memcpy(cmd->param, phy_sensor->clb_data_buffer,
														phy_sensor->sensor_data_frame_size);
						}else{
							cmd->tran_id = 1;
						}

					}else if(feed->demand[i].type == SENSOR_GYROSCOPE){
						int j;
						buffer[0] = buffer[0] / 100;
						buffer[1] = buffer[1] / 100;
						buffer[2] = buffer[2] / 100;
						if(phy_sensor != NULL && calibration_process_error == 0){
							cmd->tran_id = 0;
							cmd->length = phy_sensor->sensor_data_frame_size;
							int* ptr = (int*)phy_sensor->clb_data_buffer;
							for(j = 0 ; j < 3 ; j++){
									ptr[j] += -buffer[j];
							}
							memcpy(cmd->param, phy_sensor->clb_data_buffer,
											phy_sensor->sensor_data_frame_size);
						}else{
							cmd->tran_id = 1;
						}
					}
					raw_data_calibration_flag = 0;
					ipc_2core_send(cmd);
					memset(&buffer, 0, sizeof(buffer));
					calibration_count = 0;
					calibration_process_error = 0;
				}
			}
		}
	}
	return ret;
}
static int sensor_rawdata_prepare_reset(feed_general_t* feed)
{
	for(int i = 0 ; i < feed->demand_length ; i++){
		exposed_sensor_t* exposed_sensor =
						GetExposedStruct(feed->demand[i].type, feed->demand[i].id);
		sensor_handle_t* phy_sensor = GetPollSensStruct(feed->demand[i].type, feed->demand[i].id);
		if (exposed_sensor == NULL || phy_sensor == NULL)
			continue;
		if(feed->demand[i].freq != 0){
			int count = feed->demand[i].rt / (1000 / feed->demand[i].freq);
			if(count >= RAW_SENSOR_DATA_COUNT)
				count = RAW_SENSOR_DATA_COUNT;

			int length = count * phy_sensor->sensor_data_frame_size;
			if(exposed_sensor->rpt_data_buf_len != length){
				if(exposed_sensor->rpt_data_buf != NULL)
					bfree(exposed_sensor->rpt_data_buf);
				exposed_sensor->rpt_data_buf = balloc(length, NULL);
				exposed_sensor->rpt_data_buf_len = length;
				exposed_sensor->data_frame_count = 0;
			}
		}else{
			if(exposed_sensor->rpt_data_buf != NULL){
				bfree(exposed_sensor->rpt_data_buf);
				exposed_sensor->rpt_data_buf = NULL;
			}
			exposed_sensor->rpt_data_buf_len = 0;
			exposed_sensor->data_frame_count = 0;
		}
	}
	return 0;
}

static int sensor_rawdata_prepare_goto_idle(feed_general_t* feed)
{
	for(int i = 0 ; i < feed->demand_length ; i++){
		if(feed->demand[i].freq != 0){
			exposed_sensor_t* exposed_sensor =
				GetExposedStruct(feed->demand[i].type, feed->demand[i].id);
			if (!exposed_sensor)
				continue;
			exposed_sensor->data_frame_count = 0;
		}
	}
	return 0;
}

static int sensor_rawdata_get_property(feed_general_t* feed, uint8_t sensor_type,
									uint8_t sensor_id, uint8_t property_type, void* ptr_return)
{

	return 0;
}


static int sensor_rawdata_set_property(feed_general_t* feed, uint8_t sensor_type,
									uint8_t sensor_id, uint8_t param_length, void* ptr_param)
{

	return 0;
}

feed_general_t atlsp_algoC={
		.type = BASIC_ALGO_RAWDATA,
		.ctl_api = {
					.exec = &sensor_rawdata_prepare_exec,
					.reset = &sensor_rawdata_prepare_reset,
					.goto_idle = &sensor_rawdata_prepare_goto_idle,
					.get_property = &sensor_rawdata_get_property,
					.set_property = &sensor_rawdata_set_property,
					},
};
define_feedinit(atlsp_algoC);
