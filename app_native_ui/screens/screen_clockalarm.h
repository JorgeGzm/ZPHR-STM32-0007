/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file screen_clockalarm.h
 * @brief Clock/Alarm screen — observer-driven display.
 */

#ifndef APP_NATIVE_UI_SCREEN_CLOCKALARM_H
#define APP_NATIVE_UI_SCREEN_CLOCKALARM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

/**
 * @brief Create the Clock/Alarm screen container as a child of parent.
 *        Visibility and content are driven by subjects in clockalarm_data.
 *
 * @param parent Root LVGL object (lv_scr_act()).
 */
void screen_clockalarm_create(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_SCREEN_CLOCKALARM_H */
