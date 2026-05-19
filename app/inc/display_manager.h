/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file display_manager.h
 * @brief Display initialization, update and backlight control.
 */

#ifndef APP_DISPLAY_MANAGER_H
#define APP_DISPLAY_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define DISPLAY_MAX_BACKLIGHT_LEVEL 5

/**
 * @brief Initialize LVGL and turn on the display with default backlight.
 */
void display_init_screens(void);

/**
 * @brief Drive LVGL timer handler — call once per main loop tick.
 */
void display_update(void);

/**
 * @brief Map a raw input key code to an LVGL key.
 *
 * @param btn Raw input key code.
 * @return 0 on success, -EINVAL for unmapped keys.
 */
int display_map_button_cb(int btn);

/**
 * @brief Set backlight brightness.
 *
 * @param brightness Level from 0 to DISPLAY_MAX_BACKLIGHT_LEVEL.
 * @return 0 on success, -EINVAL if brightness is out of range.
 */
int display_set_backlight(uint8_t brightness);

/**
 * @brief Cycle backlight to the next brightness level.
 */
void display_inc_backlight_level(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_DISPLAY_MANAGER_H */
