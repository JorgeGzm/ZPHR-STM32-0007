/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file bsp.c
 * @brief Board Support Package — device handle acquisition and readiness validation.
 */

#include "bsp/bsp.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bsp, CONFIG_LOG_DEFAULT_LEVEL);

#define BACKLIGHT DT_ALIAS(pwm_led0)

FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(g_lfs_data);

static const struct device *const g_display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static const struct pwm_dt_spec g_pwm_backlight = PWM_DT_SPEC_GET(BACKLIGHT);
static const struct device *const g_adc = DEVICE_DT_GET(DT_NODELABEL(adc1));

static struct fs_mount_t g_lfs_mount = {
	.type        = FS_LITTLEFS,
	.fs_data     = &g_lfs_data,
	.storage_dev = (void *)FIXED_PARTITION_ID(storage_partition),
	.mnt_point   = "/lfs1",
};

const struct device *bsp_get_display(void)
{
	return g_display;
}

const struct pwm_dt_spec *bsp_get_pwm_backlight(void)
{
	return &g_pwm_backlight;
}

const struct device *bsp_get_adc(void)
{
	return g_adc;
}

struct fs_mount_t *bsp_get_lfs_mount(void)
{
	return &g_lfs_mount;
}

int bsp_init(void)
{
	if (!device_is_ready(g_display)) {
		LOG_ERR("display not ready");
		return -ENODEV;
	}

	if (!device_is_ready(g_pwm_backlight.dev)) {
		LOG_ERR("PWM backlight not ready");
		return -ENODEV;
	}

	if (!device_is_ready(g_adc)) {
		LOG_ERR("ADC not ready");
		return -ENODEV;
	}

	return 0;
}
