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

#include <ctype.h>
#include <string.h>
#include "services/ui_service/ui_svc.h"
#include "services/ui_service/ui_svc_api.h"
#include "infra/tcmd/handler.h"
#include "cfw/cproxy.h"
#include "drivers/data_type.h"

/* Max length of test command response */
#define BUFFER_LENGTH 64
/* Used to extract the king of pattern in tcmd */
#define VIBR_PATTERN   2

typedef struct {
	cfw_service_conn_t *ui_service_conn;
	struct tcmd_handler_ctx *context;
} ui_srv_data_t;

/*
 * Checks arguments list and fill pattern structure.
 * param [out] pattern_type Type of pattern
 * param [out] pattern      Pattern union
 * param [in]  argc         Number of arguments in the Test Command (including group and name),
 * param [in]  argv         Table of null-terminated buffers containing the arguments
 * return 0 if it fill the pattern structure or -1 if an error occured
 */
static int32_t _vibr_fill_pattern(vibration_type *pattern_type, union vibration_u *pattern, int argc, char *argv[])
{
	if (argc < 3)
		return -1;

	if (!strcmp(argv[VIBR_PATTERN], "x2")) {
		if (argc != 9)
			return -1;
		pattern->square_x2.amplitude        = strtoul(argv[3], NULL, 10);
		pattern->square_x2.duration_on_1    = strtoul(argv[4], NULL, 10);
		pattern->square_x2.duration_off_1   = strtoul(argv[5], NULL, 10);
		pattern->square_x2.duration_on_2    = strtoul(argv[6], NULL, 10);
		pattern->square_x2.duration_off_2   = strtoul(argv[7], NULL, 10);
		pattern->square_x2.repetition_count = strtoul(argv[8], NULL, 10);
		*pattern_type = VIBRATION_SQUARE_X2;
	} else if (!strcmp(argv[VIBR_PATTERN], "special")) {
		if (argc != 12)
			return -1;
		pattern->special_effect.effect_1       = strtoul(argv[3], NULL, 10);
		pattern->special_effect.duration_off_1 = strtoul(argv[4], NULL, 10);
		pattern->special_effect.effect_2       = strtoul(argv[5], NULL, 10);
		pattern->special_effect.duration_off_2 = strtoul(argv[6], NULL, 10);
		pattern->special_effect.effect_3       = strtoul(argv[7], NULL, 10);
		pattern->special_effect.duration_off_3 = strtoul(argv[8], NULL, 10);
		pattern->special_effect.effect_4       = strtoul(argv[9], NULL, 10);
		pattern->special_effect.duration_off_4 = strtoul(argv[10], NULL, 10);
		pattern->special_effect.effect_5       = strtoul(argv[11], NULL, 10);
		*pattern_type = VIBRATION_SPECIAL_EFFECTS;
	} else
		return -1;

	return 0;
}

/* Handles message for test command */
static void ui_svc_tcmd_handle_message(struct cfw_message *msg, void *param)
{
	int ui_status;
	ui_srv_data_t *priv = CFW_MESSAGE_PRIV(msg);
	struct tcmd_handler_ctx *ctx = priv->context;
	char *message = balloc(BUFFER_LENGTH, NULL);

	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_UI_LED_RSP:
	case MSG_ID_UI_VIBR_RSP:
		ui_status = ((struct cfw_rsp_message *)msg)->status;
		if (ui_status == DRV_RC_OK) {
			TCMD_RSP_FINAL(ctx, NULL);
		} else {
			snprintf(message, BUFFER_LENGTH, "KO (status: %d)", ui_status);
			TCMD_RSP_ERROR(ctx, message);
		}
		break;
	default:
		/* default cfw handler */
		snprintf(message, BUFFER_LENGTH,
			 "Wrong ui_svc rsp (id: 0x%X)", CFW_MESSAGE_ID(msg));
		TCMD_RSP_ERROR(ctx, message);
		break;
	}

	cproxy_disconnect(priv->ui_service_conn);
	cfw_msg_free(msg);
	bfree(priv);
	bfree(message);
}

/*
 * Checks if all parameters are digit (except the two first).
 *
 * return true if all parameters are digits (except the two first), false otherwise.
 */
static bool check_digit(int argc, char *argv[])
{
	int i;
	for (i = 2; i < argc; i++) {
		if (!isdigit(argv[i][0]))
			return false;
	}
	return true;
}

/*
 * Initializes UI service tests command.
 *
 * return   A cfw_service_conn_t variable returned by cproxy_connect if UI_SERVICE is registered, NULL otherwise.
 */
static cfw_service_conn_t *ui_svc_tcmd_init(void)
{
	return cproxy_connect(UI_SVC_SERVICE_ID,
			      ui_svc_tcmd_handle_message,
			      UISVC_MAIN_CLIENT_NAME);
}

/**
 * @addtogroup infra_tcmd
 * @{
 */

/**
 * @defgroup infra_tcmd_ui Test Commands
 * Interfaces to support ui Test Commands.
 * @{
 */

/**
 * Test command to launch led_fixed pattern:
 * ui led_fixed <led_number> <color_red> <color_green> <color_blue> <intensity> <duration_on>
 *
 * @param[in] argc Number of arguments in the Test Command (including group and name),
 * @param[in] argv Table of null-terminated buffers containing the arguments
 * @param[in] ctx The Test command response context
 */
void ui_led_fixed(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	led_s *pattern;
	int ret;
	char *message = balloc(sizeof(char) * BUFFER_LENGTH, NULL);

	ui_srv_data_t *ui_srv_values = balloc(sizeof(ui_srv_data_t), NULL);

	ui_srv_values->context = ctx;

	if (argc != 8) {
		snprintf(message, BUFFER_LENGTH,
			 "Not the correct number of parameters");
		goto err;
	}

	if (!(check_digit(argc, argv))) {
		snprintf(message, BUFFER_LENGTH,
			 "Parameters shall be only digit");
		goto err;
	}

	if ((ui_srv_values->ui_service_conn = ui_svc_tcmd_init()) == NULL) {
		snprintf(message, BUFFER_LENGTH, "service not registered");
		goto err;
	}

	pattern = balloc(sizeof *pattern, NULL);

#ifdef CONFIG_LED_MULTICOLOR
	/* Be careful not to put values higher than NCP5623C_PWM_MAX
	 * defined in ncp5623c.h. */
	pattern->rgb[0].r = (uint8_t) (strtoul(argv[3], NULL, 16));
	pattern->rgb[0].g = (uint8_t) (strtoul(argv[4], NULL, 16));
	pattern->rgb[0].b = (uint8_t) (strtoul(argv[5], NULL, 16));
#endif

	pattern->intensity = (uint8_t) (atoi(argv[6]));
	pattern->duration[0].duration_on = (uint16_t) (atoi(argv[7]));

	ret = ui_play_led_pattern(ui_srv_values->ui_service_conn,
				  (uint8_t) (atoi(argv[2])),
				  LED_FIXED, pattern, ui_srv_values);

	bfree(pattern);
	if (ret) {
		snprintf(message, BUFFER_LENGTH, "ui_play_led_pattern error %d",
			 ret);
		goto err;
	}
	bfree(message);
	return;
err:
	bfree(ui_srv_values);
	TCMD_RSP_ERROR(ctx, message);
	bfree(message);
}

DECLARE_TEST_COMMAND_ENG(ui, led_fixed, ui_led_fixed);

/**
 * Test command to launch led_blink_x2 pattern:
 * ui led_blink_x2 <led_number> <color_green> <color_blue> <color_red> <intensity> <duration_on_1> <duration_off_1> <duration_on_2> <duration_off_2> <repetition_count>
 *
 * @param[in] argc Number of arguments in the Test Command (including group and name),
 * @param[in] argv Table of null-terminated buffers containing the arguments
 * @param[in] ctx The Test command response context
 */
void ui_led_blink_x2(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	led_s *pattern;
	int8_t ret;
	char *message = balloc(sizeof(char) * BUFFER_LENGTH, NULL);

	ui_srv_data_t *ui_srv_values = balloc(sizeof(ui_srv_data_t), NULL);
	ui_srv_values->context = ctx;

	if (argc != 12) {
		snprintf(message, BUFFER_LENGTH,
			 "Not the correct number of parameters");
		goto err;
	}

	if (!(check_digit(argc, argv))) {
		snprintf(message, BUFFER_LENGTH,
			 "Parameters shall be only digit");
		goto err;
	}

	if ((ui_srv_values->ui_service_conn = ui_svc_tcmd_init()) == NULL) {
		snprintf(message, BUFFER_LENGTH, "service not registered");
		goto err;
	}

	pattern = balloc(sizeof *pattern, NULL);

#ifdef CONFIG_LED_MULTICOLOR
	/* Be careful not to put values higher than NCP5623C_PWM_MAX
	 * defined in ncp5623c.h.*/
	pattern->rgb[0].g = (uint8_t) (strtoul(argv[3], NULL, 16));
	pattern->rgb[0].b = (uint8_t) (strtoul(argv[4], NULL, 16));
	pattern->rgb[0].r = (uint8_t) (strtoul(argv[5], NULL, 16));
#endif

	pattern->intensity = (uint8_t) (atoi(argv[6]));
	pattern->duration[0].duration_on = (uint16_t) (atoi(argv[7]));
	pattern->duration[0].duration_off = (uint16_t) (atoi(argv[8]));
	pattern->duration[1].duration_on = (uint16_t) (atoi(argv[9]));
	pattern->duration[1].duration_off = (uint16_t) (atoi(argv[10]));
	pattern->repetition_count = (uint8_t) (atoi(argv[11]));

	ret = ui_play_led_pattern(ui_srv_values->ui_service_conn,
				  (uint8_t) (atoi(argv[2])),
				  LED_BLINK_X2, pattern, ui_srv_values);

	bfree(pattern);
	if (ret) {
		snprintf(message, BUFFER_LENGTH, "ui_play_led_pattern error %d",
			 ret);
		goto err;
	}
	bfree(message);
	return;
err:
	bfree(ui_srv_values);
	TCMD_RSP_ERROR(ctx, message);
	bfree(message);
}

DECLARE_TEST_COMMAND_ENG(ui, led_blink_x2, ui_led_blink_x2);

/**
 * Test command to launch led_blink_c2 pattern:
 * ui led_blink_c2 <led_number> <color_green_1> <color_blue_1> <color_red_1> <color_green_2> <color_blue_2> <color_red_2> <intensity> <duration_on> <duration_off_1> <duration_off_2> <repetition_count>
 *
 * @param[in] argc Number of arguments in the Test Command (including group and name),
 * @param[in] argv Table of null-terminated buffers containing the arguments
 * @param[in] ctx The Test command response context
 */
#if defined(CONFIG_UI_NCP5623_ENABLE) || defined (CONFIG_LP5562_LED)
void ui_led_blink_c2(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	led_s *pattern;
	int8_t ret;
	char * message = balloc(sizeof(char) * BUFFER_LENGTH, NULL);

	ui_srv_data_t *ui_srv_values = balloc(sizeof(ui_srv_data_t), NULL);
	ui_srv_values->context = ctx;

	if (argc != 14) {
		snprintf(message, BUFFER_LENGTH,
			 "Not the correct number of parameters");
		goto err;
	}

	if (!(check_digit(argc, argv))) {
		snprintf(message, BUFFER_LENGTH,
			 "Parameters shall be only digit");
		goto err;
	}

	if ((ui_srv_values->ui_service_conn = ui_svc_tcmd_init()) == NULL) {
		snprintf(message, BUFFER_LENGTH, "service not registered");
		goto err;
	}

	pattern = balloc(sizeof *pattern, NULL);

#ifdef CONFIG_LED_MULTICOLOR
	/* Be careful not to put values higher than NCP5623C_PWM_MAX
	 * defined in ncp5623c.h. */
	pattern->rgb[0].g = (uint8_t) (strtoul(argv[3], NULL, 16));
	pattern->rgb[0].b = (uint8_t) (strtoul(argv[4], NULL, 16));
	pattern->rgb[0].r = (uint8_t) (strtoul(argv[5], NULL, 16));
	pattern->rgb[1].g = (uint8_t) (strtoul(argv[6], NULL, 16));
	pattern->rgb[1].b = (uint8_t) (strtoul(argv[7], NULL, 16));
	pattern->rgb[1].r = (uint8_t) (strtoul(argv[8], NULL, 16));
#endif
	pattern->intensity = (uint8_t) (atoi(argv[9]));
	pattern->duration[0].duration_on = (uint16_t) (atoi(argv[10]));
	pattern->duration[0].duration_off = (uint16_t) (atoi(argv[11]));
	pattern->duration[1].duration_off = (uint16_t) (atoi(argv[12]));
	pattern->repetition_count = (uint8_t) (atoi(argv[13]));

	ret = ui_play_led_pattern(ui_srv_values->ui_service_conn,
				  (uint8_t) (atoi(argv[2])),
				  LED_BLINK_C2, pattern, ui_srv_values);

	bfree(pattern);
	if (ret) {
		snprintf(message, BUFFER_LENGTH, "ui_play_led_pattern error %d",
			 ret);
		goto err;
	}
	bfree(message);
	return;
err:
	bfree(ui_srv_values);
	TCMD_RSP_ERROR(ctx, message);
	bfree(message);
}

DECLARE_TEST_COMMAND_ENG(ui, led_blink_c2, ui_led_blink_c2);
#endif

/**
 * Test command to launch led_wave_x2 pattern:
 * ui led_wave_x2 <led_number> <color_green> <color_blue> <color_red> <intensity> <duration_on_1> <duration_off_1> <duration_on_2> <duration_off_2> <repetition_count>
 *
 * @param[in] argc Number of arguments in the Test Command (including group and name),
 * @param[in] argv Table of null-terminated buffers containing the arguments
 * @param[in] ctx The Test command response context
 */
void ui_led_wave_x2(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	led_s *pattern;
	int8_t ret;
	char *message = balloc(sizeof(char) * BUFFER_LENGTH, NULL);

	ui_srv_data_t *ui_srv_values = balloc(sizeof(ui_srv_data_t), NULL);
	ui_srv_values->context = ctx;

	if (argc != 12) {
		snprintf(message, BUFFER_LENGTH,
			 "Not the correct number of parameters");
		goto err;
	}

	if (!(check_digit(argc, argv))) {
		snprintf(message, BUFFER_LENGTH,
			 "Parameters shall be only digit");
		goto err;
	}

	if ((ui_srv_values->ui_service_conn = ui_svc_tcmd_init()) == NULL) {
		snprintf(message, BUFFER_LENGTH, "service not registered");
		goto err;
	}

	pattern = balloc(sizeof *pattern, NULL);

#ifdef CONFIG_LED_MULTICOLOR
	/* Be careful not to put values higher than NCP5623C_PWM_MAX
	 * defined in ncp5623c.h. */
	pattern->rgb[0].g = (uint8_t) (strtoul(argv[3], NULL, 16));
	pattern->rgb[0].b = (uint8_t) (strtoul(argv[4], NULL, 16));
	pattern->rgb[0].r = (uint8_t) (strtoul(argv[5], NULL, 16));
#endif

	pattern->intensity = (uint8_t) (atoi(argv[6]));
	pattern->duration[0].duration_on = (uint16_t) (atoi(argv[7]));
	pattern->duration[0].duration_off = (uint16_t) (atoi(argv[8]));
	pattern->duration[1].duration_on = (uint16_t) (atoi(argv[9]));
	pattern->duration[1].duration_off = (uint16_t) (atoi(argv[10]));
	pattern->repetition_count = (uint8_t) (atoi(argv[11]));

	ret = ui_play_led_pattern(ui_srv_values->ui_service_conn,
				  (uint8_t) (atoi(argv[2])),
				  LED_WAVE_X2, pattern, ui_srv_values);

	bfree(pattern);
	if (ret) {
		snprintf(message, BUFFER_LENGTH, "ui_play_led_pattern error %d",
			 ret);
		goto err;
	}
	bfree(message);
	return;

err:
	bfree(ui_srv_values);
	TCMD_RSP_ERROR(ctx, message);
	bfree(message);
}

DECLARE_TEST_COMMAND_ENG(ui, led_wave_x2, ui_led_wave_x2);

/**
 * Test command to launch led_none pattern:
 * ui led_none <led_number>
 *
 * @param[in] argc Number of arguments in the Test Command (including group and name),
 * @param[in] argv Table of null-terminated buffers containing the arguments
 * @param[in] ctx The Test command response context
 */
void ui_led_none(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	led_s *pattern;
	int8_t ret;
	char *message = balloc(sizeof(char) * BUFFER_LENGTH, NULL);

	ui_srv_data_t *ui_srv_values = balloc(sizeof(ui_srv_data_t), NULL);
	ui_srv_values->context = ctx;

	if (argc != 3) {
		snprintf(message, BUFFER_LENGTH,
			 "Not the correct number of parameters");
		goto err;
	}

	if (!(check_digit(argc, argv))) {
		snprintf(message, BUFFER_LENGTH,
			 "led_number shall be only digit");
		goto err;
	}

	if ((ui_srv_values->ui_service_conn = ui_svc_tcmd_init()) == NULL) {
		snprintf(message, BUFFER_LENGTH, "service not registered");
		goto err;
	}

	pattern = balloc(sizeof *pattern, NULL);

	ret = ui_play_led_pattern(ui_srv_values->ui_service_conn,
				  (uint8_t) (atoi(argv[2])),
				  LED_NONE, pattern, ui_srv_values);

	bfree(pattern);
	if (ret) {
		snprintf(message, BUFFER_LENGTH, "ui_play_led_pattern error %d",
			 ret);
		goto err;
	}
	bfree(message);
	return;
err:
	bfree(ui_srv_values);
	TCMD_RSP_ERROR(ctx, message);
	bfree(message);
}

DECLARE_TEST_COMMAND_ENG(ui, led_none, ui_led_none);

/**
 * Test command to launch vibra pattern : ui vibr x2|special <pattern parameters...>.
 * ui vibr x2 <intensity> <duration_on_1> <duration_off_1> <duration_on_2> <duration_off_2> <repetition>
 * ui vibr special <effect_1> <pause_1> <effect_2> <pause_2> <effect_3> <pause_3>
 *                 <effect_4> <pause_4> <effect_5>
 *
 * @param[in] argc Number of arguments in the Test Command (including group and name),
 * @param[in] argv Table of null-terminated buffers containing the arguments
 * @param[in] ctx The Test command response context
 */
void ui_vibr(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	int8_t ret;
	vibration_type pattern_type;
	union vibration_u *pattern = balloc(sizeof *pattern, NULL);;
	char *message = balloc(BUFFER_LENGTH, NULL);
	ui_srv_data_t *ui_srv_values = balloc(sizeof(ui_srv_data_t), NULL);

	ui_srv_values->context = ctx;

	if (_vibr_fill_pattern(&pattern_type, pattern, argc, argv) == -1) {
		snprintf(message, BUFFER_LENGTH,
			 "Usage: ui vibr x2|special <pattern parameters...>");
		goto err;
	}

	if ((ui_srv_values->ui_service_conn = ui_svc_tcmd_init()) == NULL) {
		snprintf(message, BUFFER_LENGTH, "service not registered");
		goto err;
	}

	ret = ui_play_vibr_pattern(ui_srv_values->ui_service_conn,
				  pattern_type, pattern, ui_srv_values);
	bfree(pattern);
	if (ret) {
		snprintf(message, BUFFER_LENGTH, "vibration pattern failed, status: %d", ret);
		goto err;
	}
	bfree(message);
	return;
err:
	TCMD_RSP_ERROR(ctx, message);
	bfree(ui_srv_values);
	bfree(message);
}

DECLARE_TEST_COMMAND_ENG(ui, vibr, ui_vibr);

/**
 * @}
 *
 * @}
 */
