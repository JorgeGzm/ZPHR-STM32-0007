/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file bsp.h
 * @brief Board Support Package — single point of contact for device handles.
 */

#ifndef APP_BSP_H
#define APP_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/fs/fs.h>

/**
 * @brief Initialize BSP and validate all device readiness.
 *
 * @return 0 on success, negative errno if any device is not ready.
 */
int bsp_init(void);

/**
 * @brief Get display device handle.
 *
 * @return Pointer to display device.
 */
const struct device *bsp_get_display(void);

/**
 * @brief Get PWM backlight spec.
 *
 * @return Pointer to PWM DT spec for the backlight.
 */
const struct pwm_dt_spec *bsp_get_pwm_backlight(void);

/**
 * @brief Get ADC device handle.
 *
 * @return Pointer to ADC device.
 */
const struct device *bsp_get_adc(void);

/**
 * @brief Get LittleFS mount structure.
 *
 * @return Pointer to the LittleFS mount struct.
 */
struct fs_mount_t *bsp_get_lfs_mount(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_BSP_H */
