/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file display_manager.c
 * @brief Display initialization, update and backlight control.
 */

#include "display_manager.h"

#include <zephyr/drivers/display.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/kernel.h>
#include <lvgl.h>

#include "bsp/bsp.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(display_mgr, CONFIG_LOG_DEFAULT_LEVEL);

static uint8_t g_backlight_scale = 4;

void display_init_screens(void)
{
	int ret;

	lv_init();

	ret = display_blanking_off(bsp_get_display());
	if (ret) {
		LOG_ERR("display_blanking_off: %d", ret);
	}

	ret = display_set_backlight(5);
	if (ret) {
		LOG_ERR("display_set_backlight: %d", ret);
	}

	LOG_INF("LVGL initialized");
}

void display_update(void)
{
	lv_timer_handler();
}

int display_map_button_cb(int btn)
{
	uint32_t key = 0;

	switch (btn) {
	case INPUT_KEY_UP:
		key = LV_KEY_UP;
		break;
	case INPUT_KEY_LEFT:
		key = LV_KEY_LEFT;
		break;
	case INPUT_KEY_RIGHT:
		key = LV_KEY_RIGHT;
		break;
	case INPUT_KEY_DOWN:
		key = LV_KEY_DOWN;
		break;
	case INPUT_KEY_ENTER:
		key = LV_KEY_ENTER;
		break;
	case INPUT_KEY_R:
	default:
		return -EINVAL;
	}

	(void)key;
	return 0;
}

int display_set_backlight(uint8_t brightness)
{
	int ret;
	const struct pwm_dt_spec *pwm = bsp_get_pwm_backlight();

	if (brightness > DISPLAY_MAX_BACKLIGHT_LEVEL) {
		return -EINVAL;
	}

	ret = pwm_set_pulse_dt(pwm, (brightness * pwm->period) / DISPLAY_MAX_BACKLIGHT_LEVEL);
	return ret;
}

void display_inc_backlight_level(void)
{
	display_set_backlight(g_backlight_scale);

	if (++g_backlight_scale > DISPLAY_MAX_BACKLIGHT_LEVEL) {
		g_backlight_scale = 1;
	}
}
