/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file screen_menu.h
 * @brief Menu screen — application selector.
 */

#ifndef APP_NATIVE_UI_SCREEN_MENU_H
#define APP_NATIVE_UI_SCREEN_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

/**
 * @brief Create the menu screen container as a child of parent.
 *        Visibility is bound to subj_active_screen automatically.
 *
 * @param parent Root LVGL object (lv_scr_act()).
 */
void screen_menu_create(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_SCREEN_MENU_H */
