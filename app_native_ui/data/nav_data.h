/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file nav_data.h
 * @brief Navigation data — screen subject and navigation functions.
 */

#ifndef APP_NATIVE_UI_NAV_DATA_H
#define APP_NATIVE_UI_NAV_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

enum screen_id {
	SCREEN_MENU = 0,
	SCREEN_PROTIMER,
	SCREEN_CLOCKALARM,
};

extern lv_subject_t subj_active_screen;

void nav_data_init(void);
void nav_data_go_to(enum screen_id screen);
void nav_data_go_back(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_NAV_DATA_H */
