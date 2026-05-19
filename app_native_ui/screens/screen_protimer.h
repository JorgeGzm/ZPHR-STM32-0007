/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file screen_protimer.h
 * @brief Pro Timer screen — observer-driven display.
 */

#ifndef APP_NATIVE_UI_SCREEN_PROTIMER_H
#define APP_NATIVE_UI_SCREEN_PROTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

/**
 * @brief Create the Pro Timer screen container as a child of parent.
 *        Visibility and content are driven by subjects in protimer_data.
 *
 * @param parent Root LVGL object (lv_scr_act()).
 */
void screen_protimer_create(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_SCREEN_PROTIMER_H */
